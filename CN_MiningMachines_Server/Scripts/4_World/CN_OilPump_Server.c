#ifdef DZ_SERVER

modded class CN_OilPump
{
    // Проверка, идет ли ток от РАБОТАЮЩЕГО генератора
    protected bool IsEnergySourceRunning()
    {
        CompEM energy_manager = GetCompEM();
        if (!energy_manager)
            return false;

        if (!energy_manager.IsSwitchedOn())
            return false;

        EntityAI power_source = energy_manager.GetEnergySource();
        if (!power_source)
            return false; 

        CompEM source_em = power_source.GetCompEM();
        if (source_em)
        {
            // Если генератор заглушен или в нем нет топлива — тока нет
            if (!source_em.IsSwitchedOn() || source_em.GetEnergy() <= 0)
            {
                return false;
            }
        }

        return true;
    }

    // Метод проверки условий для старта (вызывается из твоего ActionStartRecycler)
    override bool Server_CanStartProcess()
    {
        // 1. Базовая проверка твоего мода (проверка m_IsProcessing и JSON-конфига)
        if (!super.Server_CanStartProcess())
            return false;

        // ПРОВЕРКА ЭЛЕКТРИЧЕСТВА: Насос не запустится, если генератор выключен
        if (!IsEnergySourceRunning())
            return false;

        // 2. Ищем канистру в выходном слоте
        ItemBase outCanister = ItemBase.Cast(FindAttachmentBySlotName("RecycleOutput"));
        if (!outCanister || outCanister.IsFullQuantity())
            return false; // Канистры нет или она полная

        // Убеждаемся, что канистра либо пустая, либо в ней уже налита Сырая нефть
        if (outCanister.GetQuantity() > 0 && outCanister.GetLiquidType() != CN_LiquidTypes.CRUDE_OIL)
            return false;

        // 3. ПРОВЕРКА НЕФТЯНОЙ ВЫШКИ РЯДОМ ЧЕРЕЗ JSON-НАСТРОЙКИ
        if (!IsNearOilDerrick())
            return false; // Вышки рядом нет — качать неоткуда

        return true;
    }

    // Вызывается базовым классом мода, когда процесс успешно начался
    override void Server_StartProcess()
    {
        super.Server_StartProcess();

        // Переключаем сетевой флаг на сервере и шлем пакет клиентам
        m_IsPumpWorking = true;
        SetSynchDirty(); 
    }

    // Вызывается базовым классом мода, когда процесс завершился или прерван
    override void Server_StopProcess()
    {
        super.Server_StopProcess();

        // Тушим сетевой флаг на сервере и останавливаем анимацию у клиентов
        m_IsPumpWorking = false;
        SetSynchDirty(); 
    }

    // Функция сканирования окружения на наличие статических объектов карты на основе JSON-конфига
    protected bool IsNearOilDerrick()
    {
        CN_OilPumpConfig config = CN_OilPumpConfig.Cast(CN_MachineConfigManager.GetInstance().LoadConfig("CN_OilPump"));
        
        if (!config || !config.OilDerrickClassnames || config.OilDerrickClassnames.Count() == 0)
        {
            Print("[CN_MiningMachines] Ошибка: Конфигурация для CN_OilPump повреждена или не содержит вышки!");
            return false;
        }

        float checkRadius = config.OilDerrickCheckRadius;
        array<Object> nearbyObjects = new array<Object>;
        
        GetGame().GetObjectsAtPosition(GetPosition(), checkRadius, nearbyObjects, null);

        for (int i = 0; i < nearbyObjects.Count(); i++)
        {
            Object obj = nearbyObjects.Get(i);
            if (!obj) continue;

            for (int j = 0; j < config.OilDerrickClassnames.Count(); j++)
            {
                string allowedDerrickClass = config.OilDerrickClassnames.Get(j);
                if (obj.IsKindOf(allowedDerrickClass))
                {
                    return true; // Нашли вышку, одобренную администратором сервера!
                }
            }
        }

        return false; 
    }

    // ИНТЕРВАЛЬНЫЙ ЦИКЛ ДОБЫЧИ (Поштучный тик твоего Timer'а — Правило №5)
    override void Server_ExecuteCycleTick()
    {
        // Если пропало электричество (генератор заглох) или кто-то убрал вышку — останавливаемся
        if (!IsEnergySourceRunning() || !IsNearOilDerrick())
        {
            Server_StopProcess();
            return;
        }

        ItemBase outCanister = ItemBase.Cast(FindAttachmentBySlotName("RecycleOutput"));
        
        // Защитная проверка канистры
        if (!outCanister || outCanister.IsFullQuantity())
        {
            Server_StopProcess();
            return;
        }

        if (outCanister.GetQuantity() > 0 && outCanister.GetLiquidType() != CN_LiquidTypes.CRUDE_OIL)
        {
            Server_StopProcess();
            return;
        }

        // Скорость выкачки за один тик таймера: 300 мл нефти
        float fluidToPump = 300.0;
        
        // Проверяем, сколько свободного места осталось в канистре
        float itemFreeSpace = outCanister.GetFluidCap() - outCanister.GetQuantity(); 
        if (itemFreeSpace <= 0)
        {
            Server_StopProcess();
            return;
        }

        if (fluidToPump > itemFreeSpace)
            fluidToPump = itemFreeSpace;

        // Если канистра была абсолютно пустой, инициализируем тип жидкости перед заливкой
        if (outCanister.GetQuantity() == 0)
        {
            outCanister.SetLiquidType(CN_LiquidTypes.CRUDE_OIL); 
        }

        outCanister.AddQuantity(fluidToPump);

        if (outCanister.IsFullQuantity())
        {
            Server_StopProcess();
        }
    }
}

#endif
