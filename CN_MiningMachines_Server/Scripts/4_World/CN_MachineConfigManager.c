#ifdef DZ_SERVER

class CN_MachineConfigManager
{
    protected static const string CONFIG_DIRECTORY = "$profile:ColdNight_SRV_Data\\Mine_Mashines\\";
    protected static ref CN_MachineConfigManager s_Instance;

    static CN_MachineConfigManager GetInstance()
    {
        if (!s_Instance)
            s_Instance = new CN_MachineConfigManager();

        return s_Instance;
    }

    void Initialize()
    {
        MakeDirectory(CONFIG_DIRECTORY);
        EnsureConfig("CN_OreExtractor");
    }

    protected string GetConfigPath(string machineClassName)
    {
        return CONFIG_DIRECTORY + machineClassName + ".json";
    }

    protected void EnsureConfig(string machineClassName)
    {
        string path = GetConfigPath(machineClassName);

        if (FileExist(path))
            return;

        CN_MachineConfig config = new CN_MachineConfig();
        JsonFileLoader<CN_MachineConfig>.JsonSaveFile(path, config);
    }

    CN_MachineConfig LoadConfig(string machineClassName)
    {
        string path = GetConfigPath(machineClassName);
        CN_MachineConfig config = new CN_MachineConfig();

        if (FileExist(path))
        {
            JsonFileLoader<CN_MachineConfig>.JsonLoadFile(path, config);
            return config;
        }

        EnsureConfig(machineClassName);
        JsonFileLoader<CN_MachineConfig>.JsonLoadFile(path, config);
        return config;
    }
};

#endif
