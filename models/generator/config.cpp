class CfgPatches
{
	class DZ_Gear_Camping
	{
		units[]=
		{
			"TentMedium_Packed",
			"TentMedium_Pitched"
		};
		weapons[]={};
		requiredVersion=0.1;
		requiredAddons[]=
		{
			"DZ_Data"
		};
	};
};
class CfgVehicles
{
	class Inventory_Base;
	class Container_Base;
	class WorldContainer_Base;
	class HouseNoDestruct;
	class Static;
		class PowerGenerator: Inventory_Base
	{
		scope=2;
		displayName="$STR_CfgVehicles_PowerGenerator0";
		descriptionShort="$STR_CfgVehicles_PowerGenerator1";
		model="\DZ\gear\camping\power_generator.p3d";
		rotationFlags=2;
		slopeTolerance=0.40000001;
		yawPitchRollLimit[]={45,45,45};
		weight=45000;
		itemSize[]={10,10};
		itemBehaviour=0;
		attachments[]=
		{
			"SparkPlug"
		};
		fuelTankCapacity=7000;
		carveNavmesh=1;
		heavyItem=1;
		hiddenSelections[]=
		{
			"socket_1_plugged",
			"socket_2_plugged",
			"socket_3_plugged",
			"socket_4_plugged",
			"sparkplug_installed",
			"placing"
		};
		hiddenSelectionsTextures[]=
		{
			"dz\gear\camping\data\plug_black_CO.paa",
			"dz\gear\camping\data\plug_yellow_CO.paa",
			"dz\gear\camping\data\plug_white_CO.paa",
			"dz\gear\camping\data\plug_orange_CO.paa",
			"dz\gear\camping\data\power_generator_CO.paa",
			"dz\gear\camping\data\power_generator_CO.paa"
		};
		hiddenSelectionsMaterials[]=
		{
			"dz\gear\camping\data\plug.rvmat",
			"dz\gear\camping\data\plug.rvmat",
			"dz\gear\camping\data\plug.rvmat",
			"dz\gear\camping\data\plug.rvmat",
			"dz\gear\camping\data\power_generator.rvmat",
			"dz\gear\camping\data\power_generator.rvmat"
		};
		hologramMaterial="power_generator";
		hologramMaterialPath="dz\gear\camping\data";
		repairableWithKits[]={7,10};
		repairCosts[]={25,30};
		soundImpactType="metal";
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints=200;
					healthLevels[]=
					{
						
						{
							1,
							
							{
								"DZ\gear\camping\data\power_generator.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"DZ\gear\camping\data\power_generator.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"DZ\gear\camping\data\power_generator_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"DZ\gear\camping\data\power_generator_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"DZ\gear\camping\data\power_generator_destruct.rvmat"
							}
						}
					};
				};
			};
		};
		class EnergyManager
		{
			hasIcon=1;
			autoSwitchOff=1;
			energyStorageMax=10000;
			energyUsagePerSecond=0.28;
			reduceMaxEnergyByDamageCoef=0.5;
			energyAtSpawn=5000;
			powerSocketsCount=4;
			compatiblePlugTypes[]={2,6};
		};
		class AnimationSources
		{
			class socket_1_plugged
			{
				source="user";
				animPeriod=0.0099999998;
				initPhase=1;
			};
			class socket_2_plugged
			{
				source="user";
				animPeriod=0.0099999998;
				initPhase=1;
			};
			class socket_3_plugged
			{
				source="user";
				animPeriod=0.0099999998;
				initPhase=1;
			};
			class socket_4_plugged
			{
				source="user";
				animPeriod=0.0099999998;
				initPhase=1;
			};
			class sparkplug
			{
				source="user";
				animPeriod=0.0099999998;
				initPhase=0;
			};
			class fuel_tank
			{
				source="user";
				animPeriod=0.0099999998;
				initPhase=0;
			};
			class sparkplug_installed
			{
				source="user";
				animPeriod=0.0099999998;
				initPhase=1;
			};
			class placing
			{
				source="user";
				animPeriod=0.0099999998;
				initPhase=0;
			};
			class dial_fuel
			{
				source="user";
				animPeriod=1;
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
	class BatteryCharger: Inventory_Base
	{
		scope=2;
		displayName="$STR_CfgVehicles_BatteryCharger0";
		descriptionShort="$STR_CfgVehicles_BatteryCharger1";
		model="\dz\gear\camping\battery_charger.p3d";
		slopeTolerance=0.15000001;
		yawPitchRollLimit[]={45,45,45};
		attachments[]=
		{
			"LargeBattery"
		};
		weight=5000;
		itemSize[]={3,3};
		itemBehaviour=1;
		rotationFlags=2;
		hiddenSelections[]=
		{
			"clips_detached",
			"clips_folded",
			"cord_plugged",
			"cord_folded",
			"placing"
		};
		hiddenSelectionsTextures[]=
		{
			"dz\gear\camping\data\battery_charger_co.paa",
			"dz\gear\camping\data\battery_charger_co.paa",
			"dz\gear\camping\data\battery_charger_co.paa",
			"dz\gear\camping\data\battery_charger_co.paa",
			"dz\gear\camping\data\battery_charger_co.paa",
			"dz\gear\camping\data\battery_charger_co.paa"
		};
		hiddenSelectionsMaterials[]=
		{
			"dz\gear\camping\data\battery_charger.rvmat",
			"dz\gear\camping\data\battery_charger.rvmat",
			"dz\gear\camping\data\battery_charger.rvmat",
			"dz\gear\camping\data\battery_charger.rvmat",
			"dz\gear\camping\data\battery_charger.rvmat"
		};
		hologramMaterial="battery_charger";
		hologramMaterialPath="dz\gear\camping\data";
		ChargeEnergyPerSecond=1;
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints=90;
					transferToAttachmentsCoef=0.5;
					healthLevels[]=
					{
						
						{
							1,
							
							{
								"DZ\gear\camping\data\battery_charger.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"DZ\gear\camping\data\battery_charger.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"DZ\gear\camping\data\battery_charger_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"DZ\gear\camping\data\battery_charger_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"DZ\gear\camping\data\battery_charger_destruct.rvmat"
							}
						}
					};
				};
			};
		};
		repairableWithKits[]={7};
		repairCosts[]={25};
		soundImpactType="metal";
		class EnergyManager
		{
			hasIcon=1;
			energyUsagePerSecond=0.0099999998;
			cordTextureFile="DZ\gear\camping\Data\plug_black_CO.paa";
			cordLength=5;
			plugType=2;
			compatiblePlugTypes[]={4};
			powerSocketsCount=1;
			attachmentAction=2;
			wetnessExposure=0.1;
		};
		class AnimationSources
		{
			class cord_folded
			{
				source="user";
				animPeriod=0.0099999998;
				initPhase=0;
			};
			class cord_plugged
			{
				source="user";
				animPeriod=0.0099999998;
				initPhase=1;
			};
			class clips_detached
			{
				source="user";
				animPeriod=0.0099999998;
				initPhase=1;
			};
			class clips_folded
			{
				source="user";
				animPeriod=0.0099999998;
				initPhase=0;
			};
			class switch_on
			{
				source="user";
				animPeriod=0.0099999998;
				initPhase=1;
			};
			class switch_off
			{
				source="user";
				animPeriod=0.0099999998;
				initPhase=0;
			};
			class clips_car_battery
			{
				source="user";
				animPeriod=0.0099999998;
				initPhase=1;
			};
			class clips_truck_battery
			{
				source="user";
				animPeriod=0.0099999998;
				initPhase=1;
			};
			class light_stand_by
			{
				source="user";
				animPeriod=0.0099999998;
				initPhase=0;
			};
			class light_stand_by_on
			{
				source="user";
				animPeriod=0.0099999998;
				initPhase=0;
			};
			class light_switch_on
			{
				source="user";
				animPeriod=0.0099999998;
				initPhase=0;
			};
			class light_charging
			{
				source="user";
				animPeriod=0.0099999998;
				initPhase=0;
			};
			class light_charging_on
			{
				source="user";
				animPeriod=0.0099999998;
				initPhase=0;
			};
			class light_charged
			{
				source="user";
				animPeriod=0.0099999998;
				initPhase=0;
			};
			class light_charged_on
			{
				source="user";
				animPeriod=0.0099999998;
				initPhase=0;
			};
			class placing
			{
				source="user";
				animPeriod=0.0099999998;
				initPhase=0;
			};
		};
		class AnimEvents
		{
			class SoundWeapon
			{
				class drop
				{
					soundset="batterycharger_drop_SoundSet";
					id=898;
				};
			};
		};
	};


};