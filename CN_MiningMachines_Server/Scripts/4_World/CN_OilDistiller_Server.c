modded class CN_OilDistiller
{
    // Системный идентификатор (битовая маска) сырой нефти, полученной из насоса
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

        // 1. Проверяем входной слот питания (любая тара с нефтью)
        ItemBase inputContainer = ItemBase.Cast(FindAttachmentBySlotName("RecycleInput"));
        if (!inputContainer || inputContainer.GetQuantity() <= 0 || inputContainer.GetLiquidType() != LIQUID_CRUDE_OIL) return false;

        int mainLiquidType = GetLiquidIdFromName(config.OutputClass);
        int rareLiquidType = GetLiquidIdFromName(config.RareClass);
        int wasteLiquidType = GetLiquidIdFromName(config.WasteClass);

        // 2. Если отход настроен как жидкость, строго проверяем тару в слоте жидких отходов
        if (wasteLiquidType > 0)
        {
            if (!IsOutputContainerValid("RecycleWasteOutput", wasteLiquidType)) return false;
        }

        // 3. Проверяем готовность основных слотов-приёмников топлива
        bool hasValidMainOutput = IsOutputContainerValid("RecycleOutput", mainLiquidType);
        bool hasValidRareOutput = IsOutputContainerValid("RecycleRareOutput", rareLiquidType);

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

        // Контроль входной тары
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

        // ================= СЛУЧАЙ 1: ВЫПАЛ ТЕХНОЛОГИЧЕСКИЙ БРАК (ОТХОДЫ) =================
        if (roll < config.WasteChance)
        {
            int wasteLiquidType = GetLiquidIdFromName(config.WasteClass);

            if (wasteLiquidType > 0)
            {
                // --- АВТООПРЕДЕЛЕНИЕ: ОТХОД ЯВЛЯЕТСЯ ЖИДКОСТЬЮ ---
                ItemBase wasteContainer = ItemBase.Cast(FindAttachmentBySlotName("RecycleWasteOutput"));
                if (!wasteContainer || wasteContainer.IsFullQuantity() || (wasteContainer.GetQuantity() > 0 && wasteContainer.GetLiquidType() != wasteLiquidType))
                {
                    Print("[CN_DISTILL_LOG] Остановка: Слот RecycleWasteOutput пуст, забит или содержит чужую жидкость.");
                    Server_StopProcess();
                    if (GetCompEM()) GetCompEM().SwitchOff();
                    return;
                }

                float wasteToGive = config.LiquidWasteVolumePerTick;
                float freeWasteSpace = wasteContainer.GetQuantityMax() - wasteContainer.GetQuantity();

                // Корректируем порцию под свободное место в таре отходов
                if (wasteToGive > freeWasteSpace)
                {
                    wasteToGive = freeWasteSpace;
                    oilToBurn = (wasteToGive / config.LiquidWasteVolumePerTick) * config.LiquidBurnVolumePerTick;
                }

                if (wasteContainer.GetQuantity() <= 0) 
                    wasteContainer.SetLiquidType(wasteLiquidType);

                // Применяем изменения (Правило №6)
                wasteContainer.AddQuantity(wasteToGive);
                inputContainer.AddQuantity(-oilToBurn);
                Print("[CN_DISTILL_LOG] Брак! Залито жидкого отхода: " + wasteToGive + " мл.");
            }
            else
            {
                // --- АВТООПРЕДЕЛЕНИЕ: ОТХОД ЯВЛЯЕТСЯ ТВЕРДЫМ СТАКАЕМЫМ ПРЕДМЕТОМ ---
                ItemBase wasteTarget = null;
                Cargo machineCargo = GetInventory().GetCargo();
                if (machineCargo)
                {
                    // Ищем неполный стак отходов в Cargo станка
                    for (int i = 0; i < machineCargo.GetItemCount(); i++)
                    {
                        ItemBase cargoItem = ItemBase.Cast(machineCargo.GetItem(i));
                        if (cargoItem && cargoItem.GetType() == config.WasteClass && !cargoItem.IsFullQuantity())
                        {
                            wasteTarget = cargoItem;
                            break;
                        }
                    }
                }

                // Стак не найден — создаем новую позицию в Cargo
                if (!wasteTarget)
                {
                    wasteTarget = ItemBase.Cast(GetInventory().CreateInInventory(config.WasteClass));
                    if (wasteTarget) wasteTarget.SetQuantity(0);
                }

                if (wasteTarget)
                {
                    float currentQty = wasteTarget.GetQuantity();
                    float maxQty = wasteTarget.GetQuantityMax();
                    float wasteToAdd = config.WasteAmountPerTick;

                    if ((currentQty + wasteToAdd) <= maxQty)
                    {
                        // Сначала увеличиваем отход, затем списываем сырье
                        wasteTarget.SetQuantity(currentQty + wasteToAdd);
                        inputContainer.AddQuantity(-oilToBurn);
                        Print("[CN_DISTILL_LOG] Брак! Твердый отход добавлен в Cargo станка.");
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
                    Print("[CN_DISTILL_LOG] Остановка: В Cargo станка физически нет места для создания предмета брака.");
                    Server_StopProcess();
                    if (GetCompEM()) GetCompEM().SwitchOff();
                    return;
                }
            }
        }
        // ================= СЛУЧАЙ 2: УСПЕШНАЯ ДИСТИЛЛЯЦИЯ (ТОПЛИВО) =================
        else
        {
            int mainLiquidType = GetLiquidIdFromName(config.OutputClass);
            int rareLiquidType = GetLiquidIdFromName(config.RareClass);

            string targetSlot = "RecycleOutput";
            int targetLiquid = mainLiquidType;

            // Проверяем ролл на редкую фракцию
            if (roll < (config.WasteChance + config.RareChance))
            {
                targetSlot = "RecycleRareOutput";
                targetLiquid = rareLiquidType;
            }

            ItemBase outputContainer = ItemBase.Cast(FindAttachmentBySlotName(targetSlot));
            
            // Защитный механизм: если выпал редкий тип, но слот пуст или забит — перенаправляем в основной слот
            if (targetSlot == "RecycleRareOutput" && (!outputContainer || outputContainer.IsFullQuantity() || (outputContainer.GetQuantity() > 0 && outputContainer.GetLiquidType() != rareLiquidType)))
            {
                targetSlot = "RecycleOutput";
                targetLiquid = mainLiquidType;
                outputContainer = ItemBase.Cast(FindAttachmentBySlotName(targetSlot));
            }

            // Финальная проверка выбранного приёмника готового топлива
if (!outputContainer || outputContainer.IsFullQuantity() || (outputContainer.GetQuantity() > 0 && outputContainer.GetLiquidType() != targetLiquid))
{
Print("[CN_DISTILL_LOG] Остановка: Приёмная тара для готового топлива отсутствует или забита.");
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
// Отработка начисления и списания (Правило №6)
outputContainer.AddQuantity(fuelToGive);
inputContainer.AddQuantity(-oilToBurn);
Print("[CN_DISTILL_LOG] Успех! Продукт добавлен в слот " + targetSlot + ", списано сырье.");
}
// Аварийный стоп, если нефть полностью исчерпана по итогам тика
if (inputContainer.GetQuantity() <= 0)
{
Server_StopProcess();
if (GetCompEM()) GetCompEM().SwitchOff();
}
}
}
