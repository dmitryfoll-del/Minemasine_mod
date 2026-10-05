class CfgPatches
{
    class CN_MiningMachines_Server
    {
        units[] = {};
        weapons[] = {};
        requiredVersion = 0.1;
        requiredAddons[] = {"DZ_Data", "DZ_Scripts", "CN_MiningMachines"};
    };
};

class CfgMods
{
    class CN_MiningMachines_Server
    {
        dir = "CN_MiningMachines_Server";
        picture = "";
        action = "";
        type = "mod";
        dependencies[] = {"World"};

        class defs
        {
            class worldScriptModule
            {
                value = "";
                files[] = {"CN_MiningMachines_Server/Scripts/4_World/Server"};
            };
        };
    };
};
