#ifdef DZ_SERVER

class CN_MachineConfig
{
    float ProcessTimeSeconds = 1.0;
    string InputClass = "";
    string OutputClass = "";
    string RareClass = "";
    string WasteClass = "";
    float RareChance = 0.0;
    float WasteChance = 0.0;
};

// А ниже вы создаете ОТДЕЛЬНЫЙ класс конфига специально для насоса:
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

#endif
