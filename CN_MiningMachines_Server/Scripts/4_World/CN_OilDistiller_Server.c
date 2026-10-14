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
    protected bool IsOutputContainerValid(string slotName, int expectedLiquidType)
    {
        ItemBase container = ItemBase.Cast(FindAttachmentBySlotName(slotName));
        if (!container)
            return false;

        if (!Liquid.CanFillContainer(container, expectedLiquidType))
            return false;

        float freeSpace = container.GetQuantityMax() - container.GetQuantity();
        return freeSpace >= 0;
    }

    protected void Server_StartProcess()
    {
        m_IsProcessing = true;
        SetSynchDirty();
        Print("[CN_DISTILL_LOG] Запуск процесса дистилляции.");
    }

    protected void Server_StopProcess()
    {
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

    protected void Server_ExecuteCycleTick()
    {
        CN_MachineConfig config = CN_MachineConfigManager.GetInstance().GetConfig("CN_OilDistiller");
        if (!config)
        {
            Print("[CN_DISTILL_LOG] Стоп: конфигурация CN_OilDistiller не загружена.");
            Server_StopProcess();
            if (GetCompEM()) GetCompEM().SwitchOff();
            return;
        }

        if (!GetCompEM() || !GetCompEM().IsWorking())
        {
            Print("[CN_DISTILL_LOG] Стоп: энергосистема не работает.");
            Server_StopProcess();
            return;
        }

        ItemBase inputContainer = ItemBase.Cast(FindAttachmentBySlotName("RecycleInput"));
        if (!inputContainer || inputContainer.GetQuantity() <= 0 || inputContainer.GetLiquidType() != LIQUID_CRUDE_OIL) return;

        int mainLiquidType = GetLiquidIdFromName(config.OutputClass);
        int rareLiquidType = GetLiquidIdFromName(config.RareClass);
        int wasteLiquidType = GetLiquidIdFromName(config.WasteClass);

        // 2. ПРОВЕРКА СЛОТА ОТХОДОВ (RecycleWaste) — если настроена жидкость
        if (wasteLiquidType > 0)
        {
            if (!IsOutputContainerValid("RecycleWaste", wasteLiquidType)) return false;
        }

        if (mainLiquidType <= 0 || rareLiquidType <= 0)
        {
            Print("[CN_DISTILL_LOG] Стоп: OutputClass или RareClass не зарегистрирован как жидкость.");
            Server_StopProcess();
            if (GetCompEM()) GetCompEM().SwitchOff();
            return;
        }

        if (config.LiquidBurnVolumePerTick <= 0 || config.LiquidOutVolumePerTick <= 0 || config.LiquidWasteVolumePerTick <= 0 || config.WasteAmountPerTick <= 0 || config.WasteChance < 0 || config.WasteChance > 100 || config.RareChance < 0 || config.RareChance > 100 || (config.WasteChance + config.RareChance) > 100 || config.OutputClass == "" || config.RareClass == "" || config.WasteClass == "")
        {
            Print("[CN_DISTILL_LOG] Стоп: конфигурация содержит недопустимые значения.");
            Server_StopProcess();
            if (GetCompEM()) GetCompEM().SwitchOff();
            return;
        }

        float roll = Math.RandomFloatInclusive(0.0, 100.0);
        string targetSlot = "";
        int targetLiquidType = 0;
        float resultAmount = 0;
        bool isLiquidWaste = wasteLiquidType > 0;
        ItemBase resultContainer = null;
        ItemBase solidWaste = null;
        bool createdSolidWaste = false;

        if (roll < config.WasteChance)
        {
            if (isLiquidWaste)
            {
                targetSlot = "RecycleWaste";
                targetLiquidType = wasteLiquidType;
                resultAmount = config.LiquidWasteVolumePerTick;
            }
            else
            {
                // Твёрдый отход создаётся непосредственно в инвентаре станка.
                // Не используем GetGame().IsKindOf() — фактическая возможность создания
                // будет проверена самим CreateInInventory().
                if (!GetInventory())
                {
                    Print("[CN_DISTILL_LOG] Стоп: инвентарь станка недоступен.");
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
            resultContainer.SetLiquidType(targetLiquidType);
            resultContainer.SetQuantity(resultContainer.GetQuantity() + resultAmount);
            resultAdded = true;
        }

        if (!resultAdded)
        {
            Print("[CN_DISTILL_LOG] Стоп: результат цикла не был начислен.");
            Server_StopProcess();
            if (GetCompEM()) GetCompEM().SwitchOff();
            return;
        }

        float inputOldQuantity = inputContainer.GetQuantity();
        inputContainer.SetQuantity(inputOldQuantity - config.LiquidBurnVolumePerTick);

        Print("[CN_DISTILL_LOG] Цикл завершён. Сырьё списано: " + config.LiquidBurnVolumePerTick.ToString() + ".");
    }
}