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
    }

    void Initialize()
    {
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

        CN_MachineConfig config = new CN_MachineConfig();
        string errorMessage;

        if (JsonFileLoader<CN_MachineConfig>.SaveFile(path, config, errorMessage))
            Print("[CN_MiningMachines] Конфигурация создана: " + path);
        else
            ErrorEx("[CN_MiningMachines] Ошибка создания конфигурации: " + path + ". " + errorMessage);
    }

    CN_MachineConfig LoadConfig(string machineClassName)
    {
        string path = GetConfigPath(machineClassName);
        CN_MachineConfig config = new CN_MachineConfig();
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

        if (JsonFileLoader<CN_MachineConfig>.LoadFile(path, config, errorMessage))
        {
            Print("[CN_MiningMachines] Конфигурация загружена: " + path);
            return config;
        }

        ErrorEx("[CN_MiningMachines] Конфигурация имеет ошибку заполнения: " + path + ". " + errorMessage);
        return null;
    }
};

#endif
