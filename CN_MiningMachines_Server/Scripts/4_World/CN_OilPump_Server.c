modded class CN_OilPump
{
    // =================================================================
    // ВАНИЛЬНЫЕ СОБЫТИЯ ЭНЕРГОМЕНЕДЖЕРА (КАК В SPOTLIGHT / GENERATOR)
    // =================================================================

    // Автоматический вызов сервером, когда тумблер включен и есть ток в кабеле
    override void OnWorkStart()
    {
        super.OnWorkStart();
        Print("[CN_PUMP_LOG] Энергосистема: Насос получил питание и активировал цепь (OnWorkStart).");
        Server_StartProcess();
    }

    // Автоматический вызов сервером, когда насос выключили, вынули кабель или заглох генератор
    override void OnWorkStop()
    {
        super.OnWorkStop();
        Print("[CN_PUMP_LOG] Энергосистема: Насос потерял питание или отключен (OnWorkStop).");
        Server_StopProcess();
    }

    // =================================================================
    // ТВОИ МЕТОДЫ ПРОВЕРОК И ВЫЧИСЛЕНИЙ
    // =================================================================

    protected ItemBase GetOutputCanister()
    {
        return ItemBase.Cast(FindAttachmentBySlotName("RecycleOutput"));
    }

    protected bool CanPumpIntoCanister(ItemBase canister)
    {
        if (!canister)
        {
            Print("[CN_PUMP_LOG] Проверка тары: Слот 'RecycleOutput' пуст. Нет контейнера.");
            return false;
        }

        if (canister.IsFullQuantity())
        {
            Print("[CN_PUMP_LOG] Проверка тары: Контейнер '" + canister.GetType() + "' уже заполнен до краев.");
            return false;
        }

        if (canister.GetQuantity() > 0 && canister.GetLiquidType() != 16777216)
        {
            Print("[CN_PUMP_LOG] Проверка тары: В контейнере находится чужеродная жидкость (Маска: " + canister.GetLiquidType() + ").");
            return false;
        }

        return true;
    }

    protected bool IsNearOilDerrick()
    {
        CN_OilPumpConfig config = CN_OilPumpConfig.Cast(CN_MachineConfigManager.GetInstance().LoadConfig("CN_OilPump"));

        float checkRadius = 15.0;
        if (config) checkRadius = config.OilDerrickCheckRadius;

        Print("[CN_PUMP_LOG] Проверка окружения: Ищу вышку в радиусе " + checkRadius + " метров...");

        array<Object> nearbyObjects = new array<Object>;
        GetGame().GetObjectsAtPosition(GetPosition(), checkRadius, nearbyObjects, null);

        for (int i = 0; i < nearbyObjects.Count(); i++)
        {
            Object obj = nearbyObjects.Get(i);
            if (!obj) continue;

            if (obj.IsKindOf("cn_LAND_OilPump")) 
            {
                Print("[CN_PUMP_LOG] Вышка найдена! Обнаружен объект базового класса: cn_LAND_OilPump");
                return true;
            }

            if (config && config.OilDerrickClassnames)
            {
                for (int j = 0; j < config.OilDerrickClassnames.Count(); j++)
                {
                    string derrickClass = config.OilDerrickClassnames.Get(j);
                    if (derrickClass != "" && obj.IsKindOf(derrickClass)) 
                    {
                        Print("[CN_PUMP_LOG] Вышка найдена! Обнаружен объект из JSON конфига: " + derrickClass);
                        return true;
                    }
                }
            }
        }

        Print("[CN_PUMP_LOG] Сбой проверки окружения: Поблизости нет подходящих вышек.");
        return false;
    }

    // =================================================================
    // УПРАВЛЕНИЕ ТАЙМЕРОМ И ЦИКЛОМ ДОБЫЧИ
    // =================================================================

    override bool Server_CanStartProcess()
    {
        Print("[CN_PUMP_LOG] === ЗАПУСК СЕРВЕРНОЙ ПРОВЕРКИ УСЛОВИЙ СТАНКА ===");

        if (m_IsProcessing) 
            return false;

        ItemBase outputCanister = GetOutputCanister();
        if (!CanPumpIntoCanister(outputCanister)) 
            return false;

        if (!IsNearOilDerrick()) 
            return false;

        return true;
    }

    override void Server_StartProcess()
    {
        Print("[CN_PUMP_LOG] Запрос на инициализацию процесса добычи...");

        if (!Server_CanStartProcess())
        {
            Print("[CN_PUMP_LOG] Сбой: Условия не выполнены. Отключаю тумблер питания насоса.");
            if (GetCompEM()) 
                GetCompEM().SwitchOff();
            return;
        }

        if (!m_ProcessTimer) 
            m_ProcessTimer = new Timer(CALL_CATEGORY_SYSTEM);

        float processTime = 2.0;
        CN_OilPumpConfig config = CN_OilPumpConfig.Cast(CN_MachineConfigManager.GetInstance().LoadConfig("CN_OilPump"));
        if (config && config.ProcessTimeSeconds >= 0.1) 
            processTime = config.ProcessTimeSeconds;

        // ИСПРАВЛЕНО: Убрали m_IsPumpWorking, синхронизируемся чисто через базовый m_IsProcessing
        m_IsProcessing = true; 
        SetSynchDirty();

        Print("[CN_PUMP_LOG] УСПЕХ: Таймер добычи активирован. Цикл тиков: " + processTime + " сек.");
        m_ProcessTimer.Run(processTime, this, "Server_ExecuteCycleTick", NULL, true);
    }

    override void Server_StopProcess()
    {
        if (m_ProcessTimer) 
        {
            m_ProcessTimer.Stop();
            Print("[CN_PUMP_LOG] Логика: Циклический таймер добычи остановлен.");
        }

        if (!m_IsProcessing) 
            return;

        // ИСПРАВЛЕНО: Убрали m_IsPumpWorking, синхронизируемся чисто через базовый m_IsProcessing
        m_IsProcessing = false; 
        SetSynchDirty();
        
        Print("[CN_PUMP_LOG] Состояние: Насос переведен в пассивный режим.");
    }

    override void Server_ExecuteCycleTick()
    {
        if (!m_IsProcessing) 
            return;

        Print("[CN_PUMP_LOG] === ТИК ТАЙМЕРА ДОБЫЧИ ===");

        if (!GetCompEM() || !GetCompEM().IsWorking() || !IsNearOilDerrick())
        {
            Print("[CN_PUMP_LOG] Цикл прерван: Станок потерял питание или вышку в процессе тика.");
            Server_StopProcess();
            if (GetCompEM()) 
                GetCompEM().SwitchOff();
            return;
        }

        ItemBase outputCanister = GetOutputCanister();
        if (!CanPumpIntoCanister(outputCanister))
        {
            Print("[CN_PUMP_LOG] Цикл прерван: Канистра заполнена или извлечена во время тика.");
            Server_StopProcess();
            if (GetCompEM()) 
                GetCompEM().SwitchOff();
            return;
        }

        float fluidToPump = 300.0;
        CN_OilPumpConfig config = CN_OilPumpConfig.Cast(CN_MachineConfigManager.GetInstance().LoadConfig("CN_OilPump"));
        if (config && config.LiquidVolumePerTick > 0.0)
        {
            fluidToPump = config.LiquidVolumePerTick;
        }

        float freeSpace = outputCanister.GetQuantityMax() - outputCanister.GetQuantity();

        if (freeSpace <= 0)
        {
            Server_StopProcess();
            if (GetCompEM()) 
                GetCompEM().SwitchOff();
            return;
        }

        if (fluidToPump > freeSpace) 
            fluidToPump = freeSpace;

        if (outputCanister.GetQuantity() <= 0)
        {
            Print("[CN_PUMP_LOG] Наполнение: Контейнер пуст. Задаю тип жидкости — Нефть (16777216).");
            outputCanister.SetLiquidType(16777216);
        }

        outputCanister.AddQuantity(fluidToPump);
        Print("[CN_PUMP_LOG] Успешно добавлено " + fluidToPump + " мл нефти. Текущий объем: " + outputCanister.GetQuantity() + " мл.");

        if (outputCanister.IsFullQuantity())
        {
            Print("[CN_PUMP_LOG] Цикл завершен: Тара наполнилась до краев. Выключаю насос.");
            Server_StopProcess();
            if (GetCompEM()) 
                GetCompEM().SwitchOff();
        }
    }
}
