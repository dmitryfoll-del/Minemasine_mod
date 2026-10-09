#ifdef DZ_SERVER
class CN_OilPump : CN_MiningMachineBase
{
    // Радиус проверки наличия нефтяной вышки (в метрах)
    protected const float DERRICK_CHECK_RADIUS = 15.0;

    // Метод проверки условий для старта (вызывается из вашего ActionStartRecycler)
    override bool Server_CanStartProcess()
    {
        // 1. Базовая проверка вашего мода (проверка m_IsProcessing, JSON-конфига и кабеля питания)
        if (!super.Server_CanStartProcess())
            return false;

        // 2. Ищем канистру в выходном слоте
        ItemBase outCanister = ItemBase.Cast(FindAttachmentBySlotName("RecycleOutput"));
        if (!outCanister || outCanister.IsFullQuantity())
            return false; // Канистры нет или она полная

        // Убеждаемся, что канистра либо пустая, либо в ней уже налита нефть (ID 16777216)
        if (outCanister.GetQuantity() > 0 && outCanister.GetLiquidType() != 16777216)
            return false;

        // 3. ПРОВЕРКА НЕФТЯНОЙ ВЫШКИ РЯДОМ
        if (!IsNearOilDerrick())
            return false; // Вышки рядом нет — качать неоткуда

        return true;
    }

    // Функция сканирования окружения на наличие статических объектов карты
    protected bool IsNearOilDerrick()
    {
        array<Object> nearbyObjects = new array<Object>;
        
        // Получаем список всех объектов в радиусе DERRICK_CHECK_RADIUS от насоса
        GetGame().GetObjectsAtPosition(GetPosition(), DERRICK_CHECK_RADIUS, nearbyObjects, null);

        for (int i = 0; i < nearbyObjects.Count(); i++)
        {
            Object obj = nearbyObjects.Get(i);
            if (obj)
            {
                // Проверяем тип объекта по его класснейму на карте
                if (obj.IsKindOf("Land_Ind_Oil_Derrick"))
                {
                    return true; // Нашли вышку!
                }
            }
        }

        return false; // Обошли все предметы вокруг и вышку не нашли
    }

    // ИНТЕРВАЛЬНЫЙ ЦИКЛ ДОБЫЧИ (Поштучный тик вашего Timer'а — Правило №5)
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

        if (outCanister.GetQuantity() > 0 && outCanister.GetLiquidType() != 16777216)
        {
            Server_StopProcess();
            return;
        }

        // Скорость выкачки за один тик таймера: 300 мл нефти
        float fluidToPump = 300.0;
        
        // Проверяем, сколько свободного места осталось в канистре (Правило №6)
        float itemFreeSpace = outCanister.GetFluidCap() - outCanister.GetQuantity(); // Для некоторых контейнеров используется GetQuantityMax()
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
            outCanister.SetLiquidType(16777216); // Задаем ID маски Сырой нефти
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
