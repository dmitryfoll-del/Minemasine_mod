class CN_MachineConfig
{
    float ProcessTimeSeconds = 1.0;
    float EnergyUsagePerSecond = 1.0; // ИСПРАВЛЕНО: Теперь переменная официально существует!
    string InputClass = "";
    string OutputClass = "";
    string RareClass = "";
    string WasteClass = "";
    float RareChance = 0.0;
    float WasteChance = 0.0;
}

class CN_OilPumpConfig : CN_MachineConfig
{
    ref array<string> OilDerrickClassnames;
    float OilDerrickCheckRadius;	
    float LiquidVolumePerTick; // ИСПРАВЛЕНО: Новая переменная для объёма добычи!


    void CN_OilPumpConfig()
    {
        OilDerrickClassnames = new array<string>;
        OilDerrickClassnames.Insert("Land_Ind_Oil_Derrick");
        OilDerrickClassnames.Insert("Land_FuelStation_Feed");
        OilDerrickCheckRadius = 15.0;
        LiquidVolumePerTick = 300.0; // Значение по умолчанию (дефолт)
    }
}
class CN_OilDistillerConfig : CN_MachineConfig
{
    float LiquidBurnVolumePerTick;  // Сколько мл сырья (нефти) поглощает за один тик (цикл)
    float LiquidOutVolumePerTick;   // Сколько мл готового продукта (топлива) выдает за один тик
    float LiquidWasteVolumePerTick; // Сколько мл жидкого брака заливать в тару (если WasteClass — жидкость)
    float WasteAmountPerTick;       // Сколько единиц твердого отхода добавлять в стак (если WasteClass — предмет)

    void CN_OilDistillerConfig()
    {
        // Время одного технологического цикла (тика) в секундах
        ProcessTimeSeconds = 3.0;
        
        // ВАЖНО: Теперь это строковые идентификаторы жидкостей из масок DayZ!
        OutputClass = "Gasoline";       // Основной продукт дистилляции (Ванильный бензин)
        RareClass = "Gasoline";         // Редкий продукт дистилляции (Можно заменить на кастом, например, "CN_Kerosene")
        
        // Универсальное поле отходов:
        // Вариант 1 (Твёрдый предмет): "CN_FuelWaste" (Мазут/гудрон, упадет в Cargo станка)
        // Вариант 2 (Жидкость):       "CN_LiquidWaste" или "Water" (Будет сливаться в слот отходов RecycleWasteOutput)
        WasteClass = "CN_FuelWaste";    
        
        // Настройка вероятностей (в процентах от 0.0 до 100.0)
        RareChance = 10.0;              // Шанс получить фракцию из RareClass вместо OutputClass
        WasteChance = 15.0;             // Шанс технологического сбоя/некондиции (выпадение отходов вместо топлива)
        
        // Настройка расходов и выходов жидкостей (в миллилитрах)
        LiquidBurnVolumePerTick = 250.0;  // За тик сгорает 250 мл нефти
        LiquidOutVolumePerTick = 200.0;   // При успехе выдает 200 мл топлива (50 мл уходит в летучий брак)
        
        // Настройка объёмов отходов
        LiquidWasteVolumePerTick = 150.0; // Если отход жидкость — нальет 150 мл за тик брака
        WasteAmountPerTick = 1.0;         // Если отход предмет — добавит 1 шт в стак за тик брака
    }

    bool Validate(out string errorMessage)
    {
        if (ProcessTimeSeconds < 0.1)
        {
            errorMessage = "ProcessTimeSeconds должен быть не меньше 0.1 секунды.";
            return false;
        }

        if (EnergyUsagePerSecond < 0.0)
        {
            errorMessage = "EnergyUsagePerSecond не может быть отрицательным.";
            return false;
        }

        if (OutputClass == "")
        {
            errorMessage = "OutputClass не должен быть пустым.";
            return false;
        }

        if (RareClass == "")
        {
            errorMessage = "RareClass не должен быть пустым.";
            return false;
        }

        if (WasteClass == "")
        {
            errorMessage = "WasteClass не должен быть пустым.";
            return false;
        }

        if (RareChance < 0.0 || RareChance > 100.0)
        {
            errorMessage = "RareChance должен находиться в диапазоне от 0 до 100.";
            return false;
        }

        if (WasteChance < 0.0 || WasteChance > 100.0)
        {
            errorMessage = "WasteChance должен находиться в диапазоне от 0 до 100.";
            return false;
        }

        if ((RareChance + WasteChance) > 100.0)
        {
            errorMessage = "Сумма RareChance и WasteChance не должна превышать 100.";
            return false;
        }

        if (LiquidBurnVolumePerTick <= 0.0)
        {
            errorMessage = "LiquidBurnVolumePerTick должен быть больше нуля.";
            return false;
        }

        if (LiquidOutVolumePerTick <= 0.0)
        {
            errorMessage = "LiquidOutVolumePerTick должен быть больше нуля.";
            return false;
        }

        if (LiquidWasteVolumePerTick <= 0.0)
        {
            errorMessage = "LiquidWasteVolumePerTick должен быть больше нуля.";
            return false;
        }

        if (WasteAmountPerTick <= 0.0)
        {
            errorMessage = "WasteAmountPerTick должен быть больше нуля.";
            return false;
        }

        errorMessage = "";
        return true;
    }
}


