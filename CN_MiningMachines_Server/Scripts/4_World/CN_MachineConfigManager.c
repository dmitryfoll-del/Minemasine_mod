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

    protected string GetConfigPath(string machineClassName)
    {
        return CONFIG_DIRECTORY + machineClassName + ".json";
    }

    CN_MachineConfig LoadConfig(string machineClassName)
    {
        MakeDirectory(CONFIG_DIRECTORY);

        string path = GetConfigPath(machineClassName);
        CN_MachineConfig config = new CN_MachineConfig();

        if (FileExist(path))
        {
            JsonFileLoader<CN_MachineConfig>.JsonLoadFile(path, config);
            return config;
        }

        JsonFileLoader<CN_MachineConfig>.JsonSaveFile(path, config);
        return config;
    }
};

#endif
