class CfgPatches
{
    class CN_MiningMachines_Server
    {
        units[] = {};
        weapons[] = {};
        requiredVersion = 0.1;
        requiredAddons[] = {"DZ_Data", "CN_MiningMachines"};
    };
};

class CfgMods
{
    class CN_MiningMachines_Server
    {
        dir = "CN_MiningMachines_Server";
        name = "CN Mining Machines Server";
        type = "mod";
        dependencies[] = {"World", "Mission"};

        class defs
        {
            class worldScriptModule
            {
                value = "";
                files[] = {"CN_MiningMachines_Server/Scripts/4_World"};
            };

            class missionScriptModule
            {
                value = "";
                files[] = {"CN_MiningMachines_Server/Scripts/5_Mission"};
            };
        };
    };
};