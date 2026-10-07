#ifdef DZ_SERVER

modded class MissionServer
{
    override void OnInit()
    {
        super.OnInit();
        CN_MachineConfigManager.GetInstance().Initialize();
    }
};

#endif
