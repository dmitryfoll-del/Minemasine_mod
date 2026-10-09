#ifdef DZ_SERVER

class CN_MachineConfigManager
{
    protected static const string CONFIG_DIRECTORY = "$profile:ColdNight_SRV_Data\\Mine_Mashines\\";
    protected static ref CN_MachineConfigManager s_Instance;
    protected ref array<string> m_MachineClasses;

    static CN_MachineConfigManager GetInstance()
    {
        if (!s_Instance)
            s_Instance = new CN_MachineConfigManager();

        return s_Instance;
    }

    void CN_MachineConfigManager()
    {
        m_MachineClasses = new array<string>;
        m_MachineClasses.Insert("CN_OreExtractor");
        m_MachineClasses.Insert("CN_OilDistiller"); // Регистрируем дестиллятор
        m_MachineClasses.Insert("CN_OilPump");      // Регистрируем наш новый насос
    }

    void Initialize()
    {
        if (!FileExist(CONFIG_DIRECTORY))
            MakeDirectory(CONFIG_DIRECTORY);

        foreach (string machineClassName : m_MachineClasses)
            EnsureConfig(machineClassName);
    }

    protected string GetConfigPath(string machineClassName)
    {
        return CONFIG_DIRECTORY + machineClassName + ".json";
    }

    protected void EnsureConfig(string machineClassName)
    {
        string path = GetConfigPath(machineClassName);

        if (FileExist(path))
        {
            Print("[CN_MiningMachines] Конфигурация инициализирована: " + path);
            return;
        }

        string errorMessage;
        bool success = false;

        // ПРОВЕРКА ДЛЯ НАСОСА: Создаем расширенный класс с дефолтными вышками
        if (machineClassName == "CN_OilPump")
        {
            CN_OilPumpConfig pumpConfig = new CN_OilPumpConfig();
            
            // Задаем базовые значения конкретно для насоса нефти
            pumpConfig.OilDerrickClassnames = new array<string>;
            pumpConfig.OilDerrickClassnames.Insert("Land_Ind_Oil_Derrick");
            pumpConfig.OilDerrickClassnames.Insert("Land_FuelStation_Feed");
            pumpConfig.OilDerrickCheckRadius = 15.0;

            // Пример твоих базовых настроек для насоса (подгони под свой конфиг)
            pumpConfig.ProcessTimeSeconds = 2.0; 
            pumpConfig.EnergyUsagePerSecond = 1.5;

            success = JsonFileLoader<CN_OilPumpConfig>.SaveFile(path, pumpConfig, errorMessage);
        }
        else
        {
            // Стандартная логика для всех остальных станков мода
            CN_MachineConfig config = new CN_MachineConfig();
            
            // Дефолтные настройки для обычных станков
            config.ProcessTimeSeconds = 1.0;
            config.EnergyUsagePerSecond = 1.0;

            success = JsonFileLoader<CN_MachineConfig>.SaveFile(path, config, errorMessage);
        }

        if (success)
            Print("[CN_MiningMachines] Конфигурация создана: " + path);
        else
            ErrorEx("[CN_MiningMachines] Ошибка создания конфигурации: " + path + ". " + errorMessage);
    }

    CN_MachineConfig LoadConfig(string machineClassName)
    {
        string path = GetConfigPath(machineClassName);
        string errorMessage;

        if (!FileExist(path))
        {
            EnsureConfig(machineClassName);

            if (!FileExist(path))
            {
                ErrorEx("[CN_MiningMachines] Конфигурация не найдена и не создана: " + path);
                return null;
            }
        }

        // ПРОВЕРКА ДЛЯ НАСОСА: Читаем файл в расширенную структуру CN_OilPumpConfig
        if (machineClassName == "CN_OilPump")
        {
            CN_OilPumpConfig pumpConfig = new CN_OilPumpConfig();
            if (JsonFileLoader<CN_OilPumpConfig>.LoadFile(path, pumpConfig, errorMessage))
            {
                Print("[CN_MiningMachines] Конфигурация насоса загружена: " + path);
                return pumpConfig; // Возвращаем как базовый класс (автоматическое приведение типов в Enforce Script)
            }
        }
        else
        {
            // Стандартная загрузка для остальных станков мода
            CN_MachineConfig config = new CN_MachineConfig();
            if (JsonFileLoader<CN_MachineConfig>.LoadFile(path, config, errorMessage))
            {
                Print("[CN_MiningMachines] Конфигурация загружена: " + path);
                return config;
            }
        }

        ErrorEx("[CN_MiningMachines] Конфигурация имеет ошибку заполнения: " + path + ". " + errorMessage);
        return null;
    }
};

#endif
