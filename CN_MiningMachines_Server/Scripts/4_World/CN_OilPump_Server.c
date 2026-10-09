#ifdef DZ_SERVER

class CN_OilPump : CN_MiningMachineBase
{
    // Метод проверки условий для старта (вызывается из твоего ActionStartRecycler)
    override bool Server_CanStartProcess()
    {
        // 1. Базовая проверка твоего мода (проверка m_IsProcessing, JSON-конфига и кабеля питания)
        if (!super.Server_CanStartProcess())
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

    // Функция сканирования окружения на наличие статических объектов карты на основе JSON-конфига
    protected bool IsNearOilDerrick()
    {
        // Загружаем специфический конфиг для насоса через твой менеджер синглтона
        CN_OilPumpConfig config = CN_OilPumpConfig.Cast(CN_MachineConfigManager.GetInstance().LoadConfig("CN_OilPump"));
        
        // Защитная проверка: если конфиг поврежден или массив пуст — блокируем работу во избежание краша
        if (!config || !config.OilDerrickClassnames || config.OilDerrickClassnames.Count() == 0)
        {
            Print("[CN_MiningMachines] Ошибка: Конфигурация для CN_OilPump повреждена или не содержит вышки!");
            return false;
        }

        float checkRadius = config.OilDerrickCheckRadius;
        array<Object> nearbyObjects = new array<Object>;
        
        // Получаем список всех объектов в радиусе из JSON-файла от насоса
        GetGame().GetObjectsAtPosition(GetPosition(), checkRadius, nearbyObjects, null);

        // Пробегаемся по объектам вокруг станка
        for (int i = 0; i < nearbyObjects.Count(); i++)
        {
            Object obj = nearbyObjects.Get(i);
            if (!obj) continue;

            // Сверяем объект со списком разрешенных класснеймов из JSON-файла
            for (int j = 0; j < config.OilDerrickClassnames.Count(); j++)
            {
                string allowedDerrickClass = config.OilDerrickClassnames.Get(j);
                if (obj.IsKindOf(allowedDerrickClass))
                {
                    return true; // Нашли вышку, одобренную администратором сервера!
                }
            }
        }

        return false; // Обошли все предметы вокруг и нужную вышку не нашли
    }

    // ИНТЕРВАЛЬНЫЙ ЦИКЛ ДОБЫЧИ (Поштучный тик твоего Timer'а — Правило №5)
    override void Server_ExecuteCycleTick()
    {
        // Если пропало электричество или кто-то сдвинул насос от вышки — останавливаемся
        if (!IsPowered() || !IsNearOilDerrick())
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
        
        // Проверяем, сколько свободного места осталось в канистре (Правило №6)
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
            outCanister.SetLiquidType(CN_LiquidTypes.CRUDE_OIL); // Задаем ID маски Сырой нефти
        }

        // Наполняем канистру нефтью из земли
        outCanister.AddQuantity(fluidToPump);

        // Если канистра наполнилась до краев — выключаем насос
        if (outCanister.IsFullQuantity())
        {
            Server_StopProcess();
        }
    }
}

#endif
