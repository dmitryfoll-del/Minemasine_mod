modded class CN_CrudeOilCanister
{
    // Этот метод вызывается движком DayZ каждый раз, когда игрок пытается что-то залить в канистру
    override int GetLiquidContainerType()
    {
        // 1. Получаем битовую маску всех ванильных жидкостей, разрешенных этой канистре (Бензин, Дизель и т.д.)
        int allowedLiquids = super.GetLiquidContainerType();
        
        // 2. С помощью оператора | (побитовое ИЛИ) добавляем к разрешенным Керосин и Нефть
        // Это и есть идеальный аналог += для масок жидкостей в коде DayZ!
        return allowedLiquids | CN_LiquidTypes.KEROSENE | CN_LiquidTypes.CRUDE_OIL;
    }
}
