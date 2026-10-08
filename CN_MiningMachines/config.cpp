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
        dependencies[] = {"World"};

        class defs
        {
            class worldScriptModule
            {
                value = "";
                files[] = {"CN_MiningMachines/Scripts/4_World"};
            };
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

    class CN_OreExtractor: CN_MiningMachineBase
    {
        scope = 2;
        displayName = "$STR_CN_OreExtractor_Name";
        descriptionShort = "$STR_CN_OreExtractor_Desc";
    };
};
