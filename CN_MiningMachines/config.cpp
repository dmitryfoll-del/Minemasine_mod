class CfgPatches
{
    class CN_MiningMachines
    {
        units[] = {"CN_MiningMachineBase", "CN_OreExtractor"};
        weapons[] = {};
        requiredVersion = 0.1;
        requiredAddons[] = {"DZ_Data"};
    };
};

class CfgMods
{
    class CN_MiningMachines
    {
        dir = "CN_MiningMachines";
        name = "CN Mining Machines";
        type = "mod";
        dependencies[] = {"Game", "World", "Mission"};

        class defs
        {
            class gameScriptModule { value = ""; files[] = {"CN_MiningMachines/Scripts/3_Game"}; };
            class worldScriptModule { value = ""; files[] = {"CN_MiningMachines/Scripts/4_World"}; };
            class missionScriptModule { value = ""; files[] = {"CN_MiningMachines/Scripts/5_Mission"}; };
        };
    };
};

class CfgSlots
{
    class Slot_RecycleInput
    {
        name = "RecycleInput";
        displayName = "$STR_CN_Slot_RecycleInput";
        ghostIcon = "set:dayz_inventory image:cat_common";
    };

    class Slot_RecycleOutput
    {
        name = "RecycleOutput";
        displayName = "$STR_CN_Slot_RecycleOutput";
        ghostIcon = "set:dayz_inventory image:cat_common";
    };

    class Slot_RecycleRare
    {
        name = "RecycleRare";
        displayName = "$STR_CN_Slot_RecycleRare";
        ghostIcon = "set:dayz_inventory image:cat_common";
    };
};

class CfgLiquidDefinitions
{
    class Kerosene
    {
        type = 8388608; 
        displayName = "#STR_CN_LIQUID_KEROSENE"; // Ссылка на токен из stringtable.csv
        flammability = 30;
        class Nutrition
        {
            energy = 0;
            water = 0;
            toxicity = 100;
        };
    };
    class CrudeOil
    {
        type = 16777216; 
        displayName = "#STR_CN_LIQUID_CRUDE_OIL"; // Ссылка на токен из stringtable.csv
        flammability = 10;
        class Nutrition
        {
            energy = 0;
            water = 0;
            toxicity = 150;
        };
    };
};

class CfgVehicles
{
    class ItemBase;

    class CN_MiningMachineBase: ItemBase
    {
        scope = 0;
        displayName = "$STR_CN_MiningMachineBase_Name";
        descriptionShort = "$STR_CN_MiningMachineBase_Desc";
        model = "\DZ\gear\containers\woodencrate.p3d";
        weight = 10000;
        itemSize[] = {5,5};
        itemsCargoSize[] = {5,5};
        attachments[] = {"RecycleInput", "RecycleOutput", "RecycleRare"};

        class EnergyManager
        {
            hasIcon = 1;
            autoSwitchOffWhenInCargo = 1;
            energyUsagePerSecond = 0.05;
            plugType = 1;
            attachmentAction = 1;
        };

        class GUIInventoryAttachmentsProps
        {
            class RecycleInput
            {
                name = "$STR_CN_Slot_RecycleInput";
                description = "$STR_CN_GUI_RecycleInput_Desc";
                attachmentSlots[] = {"RecycleInput"};
                icon = "set:dayz_inventory image:cat_common";
            };

            class RecycleOutput
            {
                name = "$STR_CN_Slot_RecycleOutput";
                description = "$STR_CN_GUI_RecycleOutput_Desc";
                attachmentSlots[] = {"RecycleOutput"};
                icon = "set:dayz_inventory image:cat_common";
            };

            class RecycleRare
            {
                name = "$STR_CN_Slot_RecycleRare";
                description = "$STR_CN_GUI_RecycleRare_Desc";
                attachmentSlots[] = {"RecycleRare"};
                icon = "set:dayz_inventory image:cat_common";
            };
        };
    };
    class CN_OilDistiller: CN_MiningMachineBase
    {
        scope = 2;
        displayName = "#STR_CN_VEHICLE_OIL_DISTILLER"; // Ссылка на токен названия из stringtable.csv
        descriptionShort = "#STR_CN_VEHICLE_OIL_DISTILLER_DESC"; // Ссылка на токен описания из stringtable.csv
        model = "\DZ\structures\furniture\kitchen\stove\stove.p3d";

        attachments[] = {"RecycleInput", "RecycleOutput", "RecycleRare", "Firewood", "WoodenStick"};
        
        class GUIInventoryAttachmentsProps
        {
            class CN_ProcessingZones
            {
                name = "Линии перегонки";
                attachmentSlots[] = {"RecycleInput", "RecycleOutput", "RecycleRare"}; 
            };
            class CN_FireplaceZone
            {
                name = "Топка костра";
                attachmentSlots[] = {"Firewood", "WoodenStick"};
            };
        };

        class EnergyManager
        {
            switchOnAtSpawn = 0;
            isInteractive = 1;
            hasIcon = 1;
            plugType = 1;
            energyUsagePerSecond = 1.0; 
        };
    };
    class CN_OreExtractor: CN_MiningMachineBase
    {
        scope = 2;
        displayName = "$STR_CN_OreExtractor_Name";
        descriptionShort = "$STR_CN_OreExtractor_Desc";
    };
};
