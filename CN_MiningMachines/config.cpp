class CfgPatches
{
    class CN_MiningMachines
    {
        units[] = {"CN_MiningMachineBase", "CN_OreExtractor"};
        weapons[] = {};
        requiredVersion = 0.1;
        requiredAddons[] = {"DZ_Data", "DZ_Vehicles_Parts"};
    };
};

class CfgMods
{
    class CN_MiningMachines
    {
        dir = "CN_MiningMachines";
		picture = "";
		action = "";
		hideName = 1;
		hidePicture = 1;
		name = "CN_MiningMachines";
		credits = "Cold_Night";
		author = "Kaolo";
		authorID = 0;
		version = 0.1;
		extra = 0;
		type = "mod";

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
        ghostIcon = "set:dayz_inventory image:barrel";
    };

    class Slot_RecycleOutput
    {
        name = "RecycleOutput";
        displayName = "$STR_CN_Slot_RecycleOutput";
        ghostIcon = "set:dayz_inventory image:gascanister";
		selection="recycleoutput";
    };

    class Slot_RecycleRare
    {
        name = "RecycleRare";
        displayName = "$STR_CN_Slot_RecycleRare";
        ghostIcon = "set:dayz_inventory image:gascanister";
    };
};

class CfgLiquidDefinitions
{
    class Kerosene
    {
        type = 8388608; 
        displayName = "#STR_CN_LIQUID_KEROSENE"; // Ссылка на токен из stringtable.csv
        flammability = 30;
		liquidBoilingThreshold=999;
        class Nutrition
        {
            fullnessIndex=1;
			energy=5;
			water=0;
			nutritionalIndex=75;
			toxicity=1;
			digestibility=2;
			agents=16;
			agentsPerDigest=300;
        };
    };
    class CrudeOil
    {
        type = 16777216; 
        displayName = "#STR_CN_LIQUID_CRUDE_OIL"; // Ссылка на токен из stringtable.csv
        flammability = 10;
        class Nutrition
        {
            fullnessIndex=1;
			energy=5;
			water=0;
			nutritionalIndex=75;
			toxicity=1;
			digestibility=2;
			agents=16;
			agentsPerDigest=300;
        };
    };
};

class CfgVehicles
{
    class ItemBase;
	class Inventory_Base;
	class Bottle_Base;
	class Container_Base;
    class CanisterGasoline;	
	class Barrel_ColorBase;

    
    
    // Создаем правильный класс канистры для вашего мода
    class CN_CrudeOilCanister: CanisterGasoline
    {
        scope = 2; // Предмет доступен в спавнере и типах (types.xml)
        displayName = "#STR_CN_ITEM_CRUDE_OIL_CANISTER"; 
        descriptionShort = "#STR_CN_ITEM_CRUDE_OIL_CANISTER_DESC"; 
		model="\dz\vehicles\parts\oil_bottle.p3d";
		inventorySlot[] = {"CanisterGasoline", "RecycleInput", "RecycleOutput", "RecycleRare"};
        
        // Меняем цвет канистры на лету (dz текстура jerrycan)
        hiddenSelectionsTextures[] = {"\dz\gear\containers\data\jerrycan_co.paa"}; 

        // --- ВАНИЛЬНЫЙ СПОСОБ ИНИЦИАЛИЗАЦИИ ЖИДКОСТИ ---
        // Задаем тип жидкости при спавне (ID Сырой нефти из вашего CN_LiquidTypes)
        varLiquidTypeInit = 16777216; 

        // Задаем изначальное количество жидкости (20 литров)
        varQuantityInit = 0; 

        // (Опционально) Если хотите, чтобы в ЭТУ канистру игроки могли наливать ТОЛЬКО нефть,
        // раскомментируйте строчку ниже. Если оставить её закомментированной, в пустую канистру можно будет налить бензин или воду.
        liquidContainerType = "1 + 2 + 4 + 8 + 16 + 32 + 64 + 128 + 256 + 512 + 1024 + 2048 + 4096 + 8192 + 16384 + 32768 + 65536 + 131072 + 262144 + 524288 + 2097152 + 4194304 - (1 + 2 + 4 + 8 + 16 + 32 + 64 + 128 + 256) - 32768 + 8388608 + 16777216";

    };
	
	class CN_CrudeOilBarrel : Barrel_ColorBase{
		scope = 2;
		varQuantityInit=0;
		varQuantityMin=0;
		varQuantityMax=200000;
		varLiquidTypeInit = 16777216; 
		liquidContainerType = "1 + 2 + 4 + 8 + 16 + 32 + 64 + 128 + 256 + 512 + 1024 + 2048 + 4096 + 8192 + 16384 + 32768 + 65536 + 131072 + 262144 + 524288 + 2097152 + 4194304 - (1 + 2 + 4 + 8 + 16 + 32 + 64 + 128 + 256) - 32768 + 8388608 + 16777216";
		hiddenSelectionsTextures[]=
		{
			"\dz\gear\containers\data\barrel_red_co.paa"
		};
		inventorySlot[] = {"55galDrum_1", "55galDrum_2", "55galDrum_3", "55galDrum_4", "55galDrum_5", "55galDrum_6", "55galDrum_7", "55galDrum_8", "55galDrum_9", "55galDrum_10", "55galDrum_11", "55galDrum_12", "Truck_01_Barrel1", "Truck_01_Barrel2", "Truck_01_Barrel3", "Truck_01_Barrel4", "Truck_01_Barrel5", "Truck_01_Barrel6", "Truck_01_Barrel7", "Truck_01_Barrel8", "Truck_01_Barrel9", "Truck_01_Barrel10", "Truck_01_Barrel11", "Truck_01_Barrel12", "BarrelColorBaseSlot", "Barrel_1a", "Barrel_2b", "Barrel_3c", "Barrel_4d", "Barrel_5e", "Barrel_6f", "RecycleInput", "RecycleOutput", "RecycleRare"};
	};
		



    class CN_MiningMachineBase: Inventory_Base
    {
        scope = 0;
        displayName = "$STR_CN_MiningMachineBase_Name";
        descriptionShort = "$STR_CN_MiningMachineBase_Desc";
        model = "DZ\gear\containers\woodencrate.p3d";
        weight = 10000;
        itemSize[] = {5,5};
		heavyItem=1;
        //itemsCargoSize[] = {5,5};
		physLayer = "item_large";
        attachments[] = {"RecycleInput", "RecycleOutput", "RecycleRare"};
		class Cargo
		{
			itemsCargoSize[]={10,5};
			openable=0;
			allowOwnedCargoManipulation=1;
		};
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

    class CN_OilPump: CN_MiningMachineBase
    {
        scope = 2;
        displayName = "#STR_CN_VEHICLE_OIL_PUMP";
        descriptionShort = "#STR_CN_VEHICLE_OIL_PUMP_DESC";
        model = "CN_MiningMachines\cn_OilPump.p3d";
		soundImpactType="metal";
		itemSize[] = {10,15};
		heavyItem=1;
        //itemsCargoSize[] = {2,3};
        // Для работы нужна только канистра на выход
        attachments[] = 
		{
			"RecycleOutput"
		};
        
        class GUIInventoryAttachmentsProps
        {
            class CN_ProcessingZones
            {
                name = "Выход продукции";
				icon = "cat_common_cargo";
                attachmentSlots[] = 
				{
					"RecycleOutput"
				}; 
            };
        };

        class EnergyManager
        {
            cordLength=15;
			plugType=2;
			updateInterval=50;
			hasIcon=1;
			energyUsagePerSecond=0.14;
			cordTextureFile="DZ\gear\camping\Data\plug_black_CO.paa";
        };
		class AnimationSources
		{
			class shaft_rotation
			{
				source="user";
				animPeriod=0.08;
				initPhase=0;
			};
		};
		class AnimEvents
		{
			class SoundWeapon
			{
				class walk
				{
					soundSet="powergenerator_movement_walk_SoundSet";
					id=1;
				};
				class pickUpItem_Light
				{
					soundSet="pickUpPowerGenerator_SoundSet";
					id=796;
				};
				class pickUpItem
				{
					soundSet="pickUpPowerGenerator_SoundSet";
					id=797;
				};
				class drop
				{
					soundset="powergenerator_drop_SoundSet";
					id=898;
				};
			};
		};
    };

    class CN_OilDistiller: CN_MiningMachineBase
    {
        scope = 2;
        displayName = "#STR_CN_VEHICLE_OIL_DISTILLER"; // Ссылка на токен названия из stringtable.csv
        descriptionShort = "#STR_CN_VEHICLE_OIL_DISTILLER_DESC"; // Ссылка на токен описания из stringtable.csv
        model = "DZ\structures\furniture\kitchen\stove\stove.p3d";

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


class CfgNonAIVehicles
{
	class ProxyAttachment;
	class ProxyRecycleOutput: ProxyAttachment
	{
		model="CN_MiningMachines\data\proxy\RecycleOutput.p3d";
		inventorySlot[]=
		{
			"RecycleOutput"
		};
	};
};
