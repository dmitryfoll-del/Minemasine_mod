modded class CN_OilPump
{
    // Проверка, идет ли ток от РАБОТАЮЩЕГО генератора
    protected bool IsEnergySourceRunning()
    {
        ComponentEnergyManager energy_manager = GetCompEM();
        if (!energy_manager)
        {
            Print("[CN_PUMP_LOG] Сбой: У насоса нет ComponentEnergyManager!");
            return false;
        }

        if (!energy_manager.IsSwitchedOn())
        {
            Print("[CN_PUMP_LOG] Сбой: Энергоменеджер насоса выключен (IsSwitchedOn == false).");
            return false;
        }

        EntityAI power_source = energy_manager.GetEnergySource();
        if (!power_source)
        {
            Print("[CN_PUMP_LOG] Сбой: К насосу физически не подключен кабель питания!");
            return false; 
        }

        ComponentEnergyManager source_em = power_source.GetCompEM();
        if (source_em)
        {
            if (!source_em.IsSwitchedOn() || source_em.GetEnergy() <= 0)
            {
                Print("[CN_PUMP_LOG] Сбой: Генератор выключен или в нем нет бензина!");
                return false;
            }
        }

        return true;
    }

    // Метод проверки условий для старта
    override bool Server_CanStartProcess()
    {
        Print("[CN_PUMP_LOG] === ЗАПУСК ПРОВЕРКИ СТАНКА ===");

        // 1. Проверяем базовые флаги из CN_MiningMachineBase
        if (m_IsProcessing)
        {
            Print("[CN_PUMP_LOG] Сбой: Насос уже работает (m_IsProcessing == true)!");
            return false;
        }

        if (!m_ServerConfig)
        {
            Print("[CN_PUMP_LOG] Сбой: Конфигурация m_ServerConfig равна NULL!");
            return false;
        }

        // 2. Проверяем генератор
        if (!IsEnergySourceRunning())
        {
            Print("[CN_PUMP_LOG] Сбой: Проверка IsEnergySourceRunning() вернула false!");
            return false;
        }

        // 3. Проверяем канистру на выход продукции
        ItemBase outCanister = ItemBase.Cast(FindAttachmentBySlotName("RecycleOutput"));
        if (!outCanister)
        {
            Print("[CN_PUMP_LOG] Сбой: В слоте 'RecycleOutput' сервер НЕ ВИДИТ предмет! Проверь имя слота в config.cpp модели.");
            return false;
        }
        
        if (outCanister.IsFullQuantity())
        {
            Print("[CN_PUMP_LOG] Сбой: Канистра уже полная!");
            return false;
        }

        if (outCanister.GetQuantity() > 0 && outCanister.GetLiquidType() != 16777216)
        {
            Print("[CN_PUMP_LOG] Сбой: В канистре налита другая жидкость, тип: " + outCanister.GetLiquidType());
            return false;
        }

        // 4. Проверяем вышку рядом
        if (!IsNearOilDerrick())
        {
            Print("[CN_PUMP_LOG] Сбой: Метод IsNearOilDerrick() вернул false! В радиусе нет подходящей вышки.");
            return false; 
        }

        Print("[CN_PUMP_LOG] УСПЕХ: Все проверки пройдены! Насос одобряет запуск.");
        return true;
    }

    override void Server_StartProcess()
    {
        Print("[CN_PUMP_LOG] Сервер получил команду запустить таймер добычи...");
        
        if (!Server_CanStartProcess())
        {
            Print("[CN_PUMP_LOG] Сбой: Запуск прерван, так как Server_CanStartProcess() вернул false.");
            return;
        }

        float processTime = m_ServerConfig.ProcessTimeSeconds;
        if (processTime < 0.1) processTime = 0.1;

        m_IsProcessing = true;
        m_IsPumpWorking = true;
        SetSynchDirty(); 
        
        Print("[CN_PUMP_LOG] ТАЙМЕР ЗАПУЩЕН! Время цикла: " + processTime + " сек.");
        m_ProcessTimer.Run(processTime, this, "Server_ExecuteCycleTick", NULL, true);
    }

    override void Server_StopProcess()
    {
        if (m_ProcessTimer) m_ProcessTimer.Stop();
        if (!m_IsProcessing) return;

        m_IsProcessing = false;
        m_IsPumpWorking = false;
        SetSynchDirty(); 
        
        Print("[CN_PUMP_LOG] ТАЙМЕР ОСТАНОВЛЕН (Server_StopProcess).");
    }

    protected bool IsNearOilDerrick()
    {
        CN_OilPumpConfig config = CN_OilPumpConfig.Cast(CN_MachineConfigManager.GetInstance().LoadConfig("CN_OilPump"));
        
        float checkRadius = 15.0; 
        if (config)
        {
            checkRadius = config.OilDerrickCheckRadius;
        }

        Print("[CN_PUMP_LOG] Ищу вышку в радиусе: " + checkRadius + " метров...");

        array<Object> nearbyObjects = new array<Object>;
        GetGame().GetObjectsAtPosition(GetPosition(), checkRadius, nearbyObjects, null);
        
        Print("[CN_PUMP_LOG] Всего найдено объектов вокруг: " + nearbyObjects.Count());

        for (int i = 0; i < nearbyObjects.Count(); i++)
        {
            Object obj = nearbyObjects.Get(i);
            if (!obj) continue;

            if (obj.IsKindOf("cn_LAND_OilPump"))
            {
                Print("[CN_PUMP_LOG] Найдена вышка жесткого класса: cn_LAND_OilPump");
                return true; 
            }

            if (config && config.OilDerrickClassnames)
            {
                for (int j = 0; j < config.OilDerrickClassnames.Count(); j++)
                {
                    string allowedDerrickClass = config.OilDerrickClassnames.Get(j);
                    if (allowedDerrickClass != "" && obj.IsKindOf(allowedDerrickClass))
                    {
                        Print("[CN_PUMP_LOG] Найдена вышка из JSON конфига: " + allowedDerrickClass);
                        return true; 
                    }
                }
            }
        }

        return false; 
    }

    override void Server_ExecuteCycleTick()
    {
        Print("[CN_PUMP_LOG] === ТИК ЦИКЛА ДОБЫЧИ ===");

        if (!IsEnergySourceRunning() || !IsNearOilDerrick())
        {
            Print("[CN_PUMP_LOG] Цикл прерван: пропало питание или вышка!");
            Server_StopProcess();
            return;
        }

        ItemBase outCanister = ItemBase.Cast(FindAttachmentBySlotName("RecycleOutput"));
        if (!outCanister || outCanister.IsFullQuantity())
        {
            Print("[CN_PUMP_LOG] Цикл прерван: канистра пропала или заполнилась!");
            Server_StopProcess();
            return;
        }

        if (outCanister.GetQuantity() > 0 && outCanister.GetLiquidType() != 16777216)
        {
            Print("[CN_PUMP_LOG] Цикл прерван: в канистре обнаружена не нефть!");
            Server_StopProcess();
            return;
        }

        float fluidToPump = 300.0;
        float itemFreeSpace = outCanister.GetQuantityMax() - outCanister.GetQuantity(); 
        if (itemFreeSpace <= 0)
        {
            Server_StopProcess();
            return;
        }

        if (fluidToPump > itemFreeSpace)
            fluidToPump = itemFreeSpace;

        if (outCanister.GetQuantity() == 0)
        {
            outCanister.SetLiquidType(16777216); 
        }

        outCanister.AddQuantity(fluidToPump);
        Print("[CN_PUMP_LOG] Успешно накачано " + fluidToPump + " мл нефти.");

        if (outCanister.IsFullQuantity())
        {
            Print("[CN_PUMP_LOG] Канистра наполнилась до краев!");
            Server_StopProcess();
        }
    }
}
