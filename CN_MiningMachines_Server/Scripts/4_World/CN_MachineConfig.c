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

    void CN_OilPumpConfig()
    {
        OilDerrickClassnames = new array<string>;
        OilDerrickClassnames.Insert("Land_Ind_Oil_Derrick");
        OilDerrickClassnames.Insert("Land_FuelStation_Feed");
        OilDerrickCheckRadius = 15.0;
    }
}
