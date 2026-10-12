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

    // Конвертирует строковое имя жидкости из JSON (например, "Gasoline") в числовой ID маски DayZ
    protected int GetLiquidIdFromName(string liquidName)
    {
        if (liquidName == "") return 0;
        return LIQUID_CRUDE_OIL.GroupToFluidType(liquidName);
    }

    // Валидация приёмного контейнера в указанном слоте на совместимость жидкостей и наличие места
    protected bool IsOutputContainerValid(string slotName, int targetLiquidType)
    {
        ItemBase container = ItemBase.Cast(FindAttachmentBySlotName(slotName));
        if (!container || container.IsFullQuantity()) return false;
        
        // Если в таре уже что-то налито, тип жидкости должен строго совпадать с целевым
        if (container.GetQuantity() > 0 && container.GetLiquidType() != targetLiquidType) return false;
        
        return true;
    }

    override bool Server_CanStartProcess()
    {
        if (m_IsProcessing) return false;

        CN_OilDistillerConfig config = CN_OilDistillerConfig.Cast(CN_MachineConfigManager.GetInstance().LoadConfig("CN_OilDistiller"));
        if (!config || !IsPowered()) return false;

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

        if (!hasValidMainOutput && !hasValidRareOutput) return false;

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

    override void Server_ExecuteCycleTick()
    {
        if (!m_IsProcessing) return;

        CN_OilDistillerConfig config = CN_OilDistillerConfig.Cast(CN_MachineConfigManager.GetInstance().LoadConfig("CN_OilDistiller"));
        if (!config || !IsPowered())
        {
            Server_StopProcess();
            if (GetCompEM()) GetCompEM().SwitchOff();
            return;
        }

        ItemBase inputContainer = ItemBase.Cast(FindAttachmentBySlotName("RecycleInput"));
        if (!inputContainer || inputContainer.GetLiquidType() != LIQUID_CRUDE_OIL || inputContainer.GetQuantity() <= 0)
        {
            Server_StopProcess();
            if (GetCompEM()) GetCompEM().SwitchOff();
            return;
        }

        // Текущий объём сырья на сжигание за этот тик
        float oilToBurn = config.LiquidBurnVolumePerTick;
        if (oilToBurn > inputContainer.GetQuantity()) 
            oilToBurn = inputContainer.GetQuantity();

        float roll = Math.RandomFloat(0.0, 100.0);

        // ================= СЛУЧАЙ 1: ВЫПАЛ ТЕХНОЛОГИЧЕСКИЙ БРАК (ОТХОДЫ В СЛОТ RECYCLEWASTE) =================
        if (roll < config.WasteChance)
        {
            int wasteLiquidType = GetLiquidIdFromName(config.WasteClass);

            if (wasteLiquidType > 0)
            {
                // --- ОТХОД ЯВЛЯЕТСЯ ЖИДКОСТЬЮ ---
                ItemBase wasteContainer = ItemBase.Cast(FindAttachmentBySlotName("RecycleWaste"));
                if (!wasteContainer || wasteContainer.IsFullQuantity() || (wasteContainer.GetQuantity() > 0 && wasteContainer.GetLiquidType() != wasteLiquidType))
                {
                    Print("[CN_DISTILL_LOG] Стоп: Слот RecycleWaste пуст или забит чужой жидкостью.");
                    Server_StopProcess();
                    if (GetCompEM()) GetCompEM().SwitchOff();
                    return;
                }

                float wasteToGive = config.LiquidWasteVolumePerTick;
                float freeWasteSpace = wasteContainer.GetQuantityMax() - wasteContainer.GetQuantity();

                if (wasteToGive > freeWasteSpace)
                {
                    wasteToGive = freeWasteSpace;
                    oilToBurn = (wasteToGive / config.LiquidWasteVolumePerTick) * config.LiquidBurnVolumePerTick;
                }

                if (wasteContainer.GetQuantity() <= 0) 
                    wasteContainer.SetLiquidType(wasteLiquidType);

                // Синхронное начисление и списание (Правило №6)
                wasteContainer.AddQuantity(wasteToGive);
                inputContainer.AddQuantity(-oilToBurn);
                Print("[CN_DISTILL_LOG] Брак! Залито жидкого отхода в RecycleWaste: " + wasteToGive + " мл.");
            }
            else
            {
                // --- ОТХОД ЯВЛЯЕТСЯ ТВЕРДЫМ СТАКАЕМЫМ ПРЕДМЕТОМ ---
                ItemBase wasteTarget = ItemBase.Cast(FindAttachmentBySlotName("RecycleWaste"));

                // Если в слоте уже лежит предмет, но это другой тип или стак заполнен — стопаем станок
                if (wasteTarget && (wasteTarget.GetType() != config.WasteClass || wasteTarget.IsFullQuantity()))
                {
                    Print("[CN_DISTILL_LOG] Стоп: Слот RecycleWaste занят или стак отходов переполнен.");
                    Server_StopProcess();
                    if (GetCompEM()) GetCompEM().SwitchOff();
                    return;
                }

                // Если слоте пусто — пытаемся заспавнить туда предмет отхода напрямую
                if (!wasteTarget)
                {
                    wasteTarget = ItemBase.Cast(GetInventory().CreateAttachmentEx(config.WasteClass, InventorySlots.GetSlotIdFromString("RecycleWaste")));
                    if (wasteTarget) wasteTarget.SetQuantity(0);
                }

                if (wasteTarget)
                {
                    float currentQty = wasteTarget.GetQuantity();
                    float maxQty = wasteTarget.GetQuantityMax();
                    float wasteToAdd = config.WasteAmountPerTick;

                    if ((currentQty + wasteToAdd) <= maxQty)
                    {
                        wasteTarget.SetQuantity(currentQty + wasteToAdd);
                        inputContainer.AddQuantity(-oilToBurn);
                        Print("[CN_DISTILL_LOG] Брак! Твердый отход добавлен в слот RecycleWaste.");
                    }
                    else
                    {
                        Server_StopProcess();
                        if (GetCompEM()) GetCompEM().SwitchOff();
                        return;
                    }
                }
                else
                {
                    Print("[CN_DISTILL_LOG] Стоп: Не удалось создать предмет отхода в слоте RecycleWaste.");
                    Server_StopProcess();
                    if (GetCompEM()) GetCompEM().SwitchOff();
                    return;
                }
            }
        }
        // ================= СЛУЧАЙ 2: УСПЕШНАЯ ДИСТИЛЛЯЦИЯ (СЛОТЫ RECYCLEOUTPUT И RECYCLERARE) =================
        else
        {
            int mainLiquidType = GetLiquidIdFromName(config.OutputClass);
            int rareLiquidType = GetLiquidIdFromName(config.RareClass);

            string targetSlot = "RecycleOutput";
            int targetLiquid = mainLiquidType;

            if (roll < (config.WasteChance + config.RareChance))
            {
                targetSlot = "RecycleRare";
                targetLiquid = rareLiquidType;
            }

            ItemBase outputContainer = ItemBase.Cast(FindAttachmentBySlotName(targetSlot));
            
            // Если выпала редкая фракция, но слот RecycleRare пуст или забит — делаем откат в RecycleOutput
            if (targetSlot == "RecycleRare" && (!outputContainer || outputContainer.IsFullQuantity() || (outputContainer.GetQuantity() > 0 && outputContainer.GetLiquidType() != rareLiquidType)))
            {
                targetSlot = "RecycleOutput";
                targetLiquid = mainLiquidType;
                outputContainer = ItemBase.Cast(FindAttachmentBySlotName(targetSlot));
            }

            if (!outputContainer || outputContainer.IsFullQuantity() || (outputContainer.GetQuantity() > 0 && outputContainer.GetLiquidType() != targetLiquid))
            {
                Print("[CN_DISTILL_LOG] Стоп: Целевой слот " + targetSlot + " недоступен или забит.");
                Server_StopProcess();
                if (GetCompEM()) GetCompEM().SwitchOff();
                return;
            }
float fuelToGive = config.LiquidOutVolumePerTick;
float freeSpace = outputContainer.GetQuantityMax() - outputContainer.GetQuantity();
if (fuelToGive > freeSpace)
{
fuelToGive = freeSpace;
oilToBurn = (fuelToGive / config.LiquidOutVolumePerTick) * config.LiquidBurnVolumePerTick;
}
if (outputContainer.GetQuantity() <= 0)
outputContainer.SetLiquidType(targetLiquid);
outputContainer.AddQuantity(fuelToGive);
inputContainer.AddQuantity(-oilToBurn);
Print("[CN_DISTILL_LOG] Успех! Продукт добавлен в слот " + targetSlot);
}
// Финальная проверка на опустошение сырья
if (inputContainer.GetQuantity() <= 0)
{
Server_StopProcess();
if (GetCompEM()) GetCompEM().SwitchOff();
}
}
}
