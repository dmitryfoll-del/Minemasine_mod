modded class CN_OilDistiller : CN_MiningMachineBase
{
    // Системный идентификатор (битовая маска) сырой нефти
    protected const int LIQUID_CRUDE_OIL = 16777216;

    override void OnWorkStart()
    {
        super.OnWorkStart();
        Server_StartProcess();
    }

    override void OnWorkStop()
    {
        super.OnWorkStop();
        Server_StopProcess();
    }

    // Получает числовой тип жидкости по класснейму из JSON-конфига.
    // Если класснейм не зарегистрирован как жидкость, возвращает 0.
    protected int GetLiquidIdFromName(string liquidName)
    {
        if (liquidName == "")
            return 0;

        NutritionalProfile profile = Liquid.GetNutritionalProfileByName(liquidName);

        if (!profile || !profile.IsLiquid())
            return 0;

        return profile.GetLiquidType();
    }

    // Проверяет совместимость контейнера по liquidContainerType, его содержимому и наличию места.
    protected bool IsOutputContainerValid(string slotName, int targetLiquidType)
    {
        ItemBase container = ItemBase.Cast(FindAttachmentBySlotName(slotName));

        if (!container || targetLiquidType <= 0)
            return false;

        // Ванильная проверка учитывает liquidContainerType, заполненность и уже налитую жидкость.
        return Liquid.CanFillContainer(container, targetLiquidType);
    }

    override bool Server_CanStartProcess()
    {
        if (m_IsProcessing) return false;

        CN_OilDistillerConfig config = CN_OilDistillerConfig.Cast(CN_MachineConfigManager.GetInstance().LoadConfig("CN_OilDistiller"));
        if (!config || !GetCompEM() || !GetCompEM().IsWorking()) return false;

        // 1. ПРОВЕРКА ВХОДА (RecycleInput) — тара с нефтью
        ItemBase inputContainer = ItemBase.Cast(FindAttachmentBySlotName("RecycleInput"));
        if (!inputContainer || inputContainer.GetQuantity() <= 0 || inputContainer.GetLiquidType() != LIQUID_CRUDE_OIL) return false;

        int mainLiquidType = GetLiquidIdFromName(config.OutputClass);
        int rareLiquidType = GetLiquidIdFromName(config.RareClass);
        int wasteLiquidType = GetLiquidIdFromName(config.WasteClass);

        // 2. ПРОВЕРКА СЛОТА ОТХОДОВ (RecycleWaste) — если настроена жидкость
        if (wasteLiquidType > 0)
        {
            if (!IsOutputContainerValid("RecycleWaste", wasteLiquidType)) return false;
        }

        // 3. ПРОВЕРКА ВЫХОДНЫХ СЛОТОВ ТОПЛИВА (RecycleOutput и RecycleRare)
        bool hasValidMainOutput = IsOutputContainerValid("RecycleOutput", mainLiquidType);
        bool hasValidRareOutput = IsOutputContainerValid("RecycleRare", rareLiquidType);

        // Оба выходных слота обязательны: каждый должен принимать свою жидкость.
        if (!hasValidMainOutput || !hasValidRareOutput) return false;

        return true;
    }

    override void Server_StartProcess()
    {
        if (!Server_CanStartProcess())
        {
            if (GetCompEM()) GetCompEM().SwitchOff();
            return;
        }

        if (!m_ProcessTimer) 
            m_ProcessTimer = new Timer(CALL_CATEGORY_SYSTEM);

        float processTime = 3.0;
        CN_OilDistillerConfig config = CN_OilDistillerConfig.Cast(CN_MachineConfigManager.GetInstance().LoadConfig("CN_OilDistiller"));
        if (config && config.ProcessTimeSeconds >= 0.1) 
            processTime = config.ProcessTimeSeconds;

        m_IsProcessing = true;
        SetSynchDirty();

        m_ProcessTimer.Run(processTime, this, "Server_ExecuteCycleTick", NULL, true);
    }

    override void Server_StopProcess()
    {
        if (m_ProcessTimer) m_ProcessTimer.Stop();
        if (!m_IsProcessing) return;

        m_IsProcessing = false;
        SetSynchDirty();
    }

    // Ищет твёрдый отход по всему инвентарю станка, включая cargo и attachments.
    protected ItemBase FindSolidWasteInInventory(string wasteClass)
    {
        if (!GetInventory())
            return null;

        array<EntityAI> inventoryItems = new array<EntityAI>();
        GetInventory().EnumerateInventory(InventoryTraversalType.PREORDER, inventoryItems);

        foreach (EntityAI entity : inventoryItems)
        {
            ItemBase item = ItemBase.Cast(entity);
            if (item && item.GetType() == wasteClass)
                return item;
        }

        return null;
    }

    override void Server_ExecuteCycleTick()
    {
        if (!m_IsProcessing)
            return;

        CN_OilDistillerConfig config = CN_OilDistillerConfig.Cast(CN_MachineConfigManager.GetInstance().LoadConfig("CN_OilDistiller"));
        if (!config || !GetCompEM() || !GetCompEM().IsWorking())
        {
            Server_StopProcess();
            if (GetCompEM()) GetCompEM().SwitchOff();
            return;
        }

        // Защита от некорректной конфигурации и объёмов.
        if (config.LiquidBurnVolumePerTick <= 0 || config.LiquidOutVolumePerTick <= 0 || config.LiquidWasteVolumePerTick <= 0 || config.WasteAmountPerTick <= 0 || config.WasteChance < 0 || config.WasteChance > 100 || config.RareChance < 0 || config.RareChance > 100 || (config.WasteChance + config.RareChance) > 100 || config.OutputClass == "" || config.RareClass == "" || config.WasteClass == "")
        {
            Print("[CN_DISTILL_LOG] Стоп: конфигурация содержит недопустимые значения.");
            Server_StopProcess();
            if (GetCompEM()) GetCompEM().SwitchOff();
            return;
        }

        ItemBase inputContainer = ItemBase.Cast(FindAttachmentBySlotName("RecycleInput"));
        if (!inputContainer || inputContainer.GetLiquidType() != LIQUID_CRUDE_OIL)
        {
            Server_StopProcess();
            if (GetCompEM()) GetCompEM().SwitchOff();
            return;
        }

        // Неполный остаток не перерабатываем: для любого результата требуется полный расход сырья.
        float oilToBurn = config.LiquidBurnVolumePerTick;
        if (inputContainer.GetQuantity() < oilToBurn)
        {
            Print("[CN_DISTILL_LOG] Стоп: остатка сырья недостаточно для полного цикла.");
            Server_StopProcess();
            if (GetCompEM()) GetCompEM().SwitchOff();
            return;
        }

        int mainLiquidType = GetLiquidIdFromName(config.OutputClass);
        int rareLiquidType = GetLiquidIdFromName(config.RareClass);
        int wasteLiquidType = GetLiquidIdFromName(config.WasteClass);

        if (mainLiquidType <= 0 || rareLiquidType <= 0)
        {
            Print("[CN_DISTILL_LOG] Стоп: OutputClass или RareClass не зарегистрирован как жидкость.");
            Server_StopProcess();
            if (GetCompEM()) GetCompEM().SwitchOff();
            return;
        }

        // Оба топливных контейнера обязательны и должны принимать настроенные жидкости.
        if (!IsOutputContainerValid("RecycleOutput", mainLiquidType) || !IsOutputContainerValid("RecycleRare", rareLiquidType))
        {
            Print("[CN_DISTILL_LOG] Стоп: один из обязательных выходных контейнеров недоступен или несовместим.");
            Server_StopProcess();
            if (GetCompEM()) GetCompEM().SwitchOff();
            return;
        }

        // Выбираем ровно один результат до начисления и до списания сырья.
        float roll = Math.RandomFloat(0.0, 100.0);
        string targetSlot = "";
        int targetLiquidType = 0;
        bool isLiquidWaste = wasteLiquidType > 0;
        ItemBase resultContainer = null;
        ItemBase solidWaste = null;
        bool createdSolidWaste = false;
        float resultAmount = 0.0;

        if (roll < config.WasteChance)
        {
            if (isLiquidWaste)
            {
                targetSlot = "RecycleWaste";
                targetLiquidType = wasteLiquidType;
                resultAmount = config.LiquidWasteVolumePerTick;
                resultContainer = ItemBase.Cast(FindAttachmentBySlotName(targetSlot));

                if (!resultContainer || !Liquid.CanFillContainer(resultContainer, targetLiquidType))
                {
                    Print("[CN_DISTILL_LOG] Стоп: контейнер жидких отходов отсутствует или несовместим.");
                    Server_StopProcess();
                    if (GetCompEM()) GetCompEM().SwitchOff();
                    return;
                }

                float freeWasteSpace = resultContainer.GetQuantityMax() - resultContainer.GetQuantity();
                if (freeWasteSpace < resultAmount)
                {
                    Print("[CN_DISTILL_LOG] Стоп: в контейнере жидких отходов недостаточно места для полного цикла.");
                    Server_StopProcess();
                    if (GetCompEM()) GetCompEM().SwitchOff();
                    return;
                }
            }
            else
            {
                // Если WasteClass не жидкость, он обязан быть корректным ItemBase-классом.
                if (!GetGame().IsKindOf(config.WasteClass, "ItemBase") || !GetInventory())
                {
                    Print("[CN_DISTILL_LOG] Стоп: WasteClass не является зарегистрированным предметом или инвентарь станка недоступен.");
                    Server_StopProcess();
                    if (GetCompEM()) GetCompEM().SwitchOff();
                    return;
                }

                solidWaste = FindSolidWasteInInventory(config.WasteClass);
                if (solidWaste)
                {
                    float currentWasteQty = solidWaste.GetQuantity();
                    float maxWasteQty = solidWaste.GetQuantityMax();
                    if (maxWasteQty <= 0 || (currentWasteQty + config.WasteAmountPerTick) > maxWasteQty)
                    {
                        Print("[CN_DISTILL_LOG] Стоп: в стаке твёрдых отходов недостаточно места для полного цикла.");
                        Server_StopProcess();
                        if (GetCompEM()) GetCompEM().SwitchOff();
                        return;
                    }
                }
                else if (config.WasteAmountPerTick <= 0)
                {
                    Print("[CN_DISTILL_LOG] Стоп: WasteAmountPerTick должен быть больше нуля.");
                    Server_StopProcess();
                    if (GetCompEM()) GetCompEM().SwitchOff();
                    return;
                }
            }
        }
        else if (roll < (config.WasteChance + config.RareChance))
        {
            targetSlot = "RecycleRare";
            targetLiquidType = rareLiquidType;
            resultAmount = config.LiquidOutVolumePerTick;
        }
        else
        {
            targetSlot = "RecycleOutput";
            targetLiquidType = mainLiquidType;
            resultAmount = config.LiquidOutVolumePerTick;
        }

        // Для жидкого результата заранее проверяем место под полный объём.
        if (!(roll < config.WasteChance && !isLiquidWaste))
        {
            if (!resultContainer)
                resultContainer = ItemBase.Cast(FindAttachmentBySlotName(targetSlot));

            if (!resultContainer || !Liquid.CanFillContainer(resultContainer, targetLiquidType))
            {
                Print("[CN_DISTILL_LOG] Стоп: целевой контейнер " + targetSlot + " отсутствует или несовместим.");
                Server_StopProcess();
                if (GetCompEM()) GetCompEM().SwitchOff();
                return;
            }

            float freeSpace = resultContainer.GetQuantityMax() - resultContainer.GetQuantity();
            if (freeSpace < resultAmount)
            {
                Print("[CN_DISTILL_LOG] Стоп: в контейнере " + targetSlot + " недостаточно места для полного результата.");
                Server_StopProcess();
                if (GetCompEM()) GetCompEM().SwitchOff();
                return;
            }
        }

        // Начисляем выбранный результат. Сырьё списывается только после успешного начисления.
        bool resultAdded = false;
        if (roll < config.WasteChance && !isLiquidWaste)
        {
            if (solidWaste)
            {
                float oldWasteQty = solidWaste.GetQuantity();
                solidWaste.SetQuantity(oldWasteQty + config.WasteAmountPerTick);
                resultAdded = Math.AbsFloat(solidWaste.GetQuantity() - (oldWasteQty + config.WasteAmountPerTick)) < 0.001;
            }
            else
            {
                solidWaste = ItemBase.Cast(GetInventory().CreateInInventory(config.WasteClass));
                if (solidWaste)
                {
                    createdSolidWaste = true;
                    solidWaste.SetQuantity(config.WasteAmountPerTick);
                    resultAdded = Math.AbsFloat(solidWaste.GetQuantity() - config.WasteAmountPerTick) < 0.001;
                }
            }

            if (!resultAdded)
            {
                if (createdSolidWaste && solidWaste)
                    GetGame().ObjectDelete(solidWaste);

                Print("[CN_DISTILL_LOG] Стоп: не удалось создать или увеличить количество твёрдых отходов.");
                Server_StopProcess();
                if (GetCompEM()) GetCompEM().SwitchOff();
                return;
            }
        }
        else
        {
            if (resultContainer.GetQuantity() <= 0)
                resultContainer.SetLiquidType(targetLiquidType);

            float oldResultQty = resultContainer.GetQuantity();
            resultContainer.AddQuantity(resultAmount);
            resultAdded = Math.AbsFloat(resultContainer.GetQuantity() - (oldResultQty + resultAmount)) < 0.01;

            if (!resultAdded)
            {
                // При неуспешном начислении сырьё не списывается.
                Print("[CN_DISTILL_LOG] Стоп: не удалось начислить полный объём результата.");
                Server_StopProcess();
                if (GetCompEM()) GetCompEM().SwitchOff();
                return;
            }
        }

        // Единственное место списания сырья: один раз и только после подтверждённого результата.
        inputContainer.AddQuantity(-oilToBurn);
        Print("[CN_DISTILL_LOG] Цикл завершён. Сырьё списано один раз: " + oilToBurn + " мл.");

        if (inputContainer.GetQuantity() <= 0)
        {
            Server_StopProcess();
            if (GetCompEM()) GetCompEM().SwitchOff();
        }
    }

}
