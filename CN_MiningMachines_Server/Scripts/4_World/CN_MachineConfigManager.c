class CN_MachineConfigManager
{
    protected static const string CONFIG_DIRECTORY = "$profile:ColdNight_SRV_Data\\Mine_Mashines\\";
    protected static ref CN_MachineConfigManager s_Instance;
    protected ref array<string> m_MachineClasses;

    static CN_MachineConfigManager GetInstance()
    {
        if (!s_Instance) s_Instance = new CN_MachineConfigManager();
        return s_Instance;
    }

    void CN_MachineConfigManager()
    {
        m_MachineClasses = new array<string>;
        m_MachineClasses.Insert("CN_OreExtractor");
        m_MachineClasses.Insert("CN_OilDistiller"); 
        m_MachineClasses.Insert("CN_OilPump");      
    }

    void Initialize()
    {
        if (!FileExist(CONFIG_DIRECTORY)) MakeDirectory(CONFIG_DIRECTORY);

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
        if (FileExist(path)) return;

        string errorMessage;

        if (machineClassName == "CN_OilPump")
        {
            CN_OilPumpConfig pumpConfig = new CN_OilPumpConfig();
            
            pumpConfig.OilDerrickClassnames = new array<string>;
            pumpConfig.OilDerrickClassnames.Insert("Land_Ind_Oil_Derrick");
            pumpConfig.OilDerrickClassnames.Insert("Land_FuelStation_Feed");
            pumpConfig.OilDerrickCheckRadius = 15.0;

            pumpConfig.ProcessTimeSeconds = 2.0; 
            pumpConfig.EnergyUsagePerSecond = 1.5;

            // ИСПРАВЛЕНО: Применен точный ванильный метод SaveFile движка DayZ
            JsonFileLoader<CN_OilPumpConfig>.SaveFile(path, pumpConfig, errorMessage);
        }
        else
        {
            CN_MachineConfig config = new CN_MachineConfig();
            
            config.ProcessTimeSeconds = 1.0;
            config.EnergyUsagePerSecond = 1.0;

            // ИСПРАВЛЕНО: Применен точный ванильный метод SaveFile движка DayZ
            JsonFileLoader<CN_MachineConfig>.SaveFile(path, config, errorMessage);
        }

        if (FileExist(path))
            Print("[CN_MiningMachines] Конфигурация успешно создана: " + path);
    }

    CN_MachineConfig LoadConfig(string machineClassName)
    {
        string path = GetConfigPath(machineClassName);
        string errorMessage;

        if (!FileExist(path))
        {
            EnsureConfig(machineClassName);
            if (!FileExist(path)) return null;
        }

        if (machineClassName == "CN_OilPump")
        {
            CN_OilPumpConfig pumpConfig = new CN_OilPumpConfig();
            // ИСПРАВЛЕНО: Применен точный ванильный метод LoadFile движка DayZ
            if (JsonFileLoader<CN_OilPumpConfig>.LoadFile(path, pumpConfig, errorMessage))
            {
                return pumpConfig;
            }
        }
        else
        {
            CN_MachineConfig config = new CN_MachineConfig();
            // ИСПРАВЛЕНО: Применен точный ванильный метод LoadFile движка DayZ
            if (JsonFileLoader<CN_MachineConfig>.LoadFile(path, config, errorMessage))
            {
                return config;
            }
        }

        return null;
    }
}
