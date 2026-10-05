class CfgPatches
{
    class CN_MiningMachines
    {
        units[] = {"CN_MiningMachineBase", "CN_OreExtractor"};
        weapons[] = {};
        requiredVersion = 0.1;
        requiredAddons[] = {"DZ_Data", "DZ_Scripts"};
    };
};

class CfgMods
{
    class CN_MiningMachines
    {
        dir = "CN_MiningMachines";
        picture = "";
        action = "";
        type = "mod";
        dependencies[] = {"World"};

        class defs
        {
            class worldScriptModule
            {
                value = "";
                files[] = {"CN_MiningMachines/Scripts/4_World/Common"};
            };
        };
    };
};

class CfgSlots
{
    class Slot_RecycleInput
    {
        name = "RecycleInput";
        displayName = "Input";
        ghostIcon = "set:dayz_inventory image:cat_common";
    };

    class Slot_RecycleOutput
    {
        name = "RecycleOutput";
        displayName = "Output";
        ghostIcon = "set:dayz_inventory image:cat_common";
    };

    class Slot_RecycleRare
    {
        name = "RecycleRare";
        displayName = "Rare";
        ghostIcon = "set:dayz_inventory image:cat_common";
    };
};

class CfgVehicles
{
    class ItemBase;

    class CN_MiningMachineBase: ItemBase
    {
        scope = 0;
        displayName = "Mining Machine";
        descriptionShort = "Powered processing machine.";
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
                name = "Input";
                description = "Raw material";
                attachmentSlots[] = {"RecycleInput"};
                icon = "set:dayz_inventory image:cat_common";
            };

            class RecycleOutput
            {
                name = "Output";
                description = "Main product";
                attachmentSlots[] = {"RecycleOutput"};
                icon = "set:dayz_inventory image:cat_common";
            };

            class RecycleRare
            {
                name = "Rare";
                description = "Rare product";
                attachmentSlots[] = {"RecycleRare"};
                icon = "set:dayz_inventory image:cat_common";
            };
        };
    };

    class CN_OreExtractor: CN_MiningMachineBase
    {
        scope = 2;
        displayName = "Ore Extractor";
        descriptionShort = "Processes raw ore using external power.";
    };
};
