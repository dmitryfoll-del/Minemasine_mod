modded class ModItemRegisterCallbacks
{
	override void RegisterHeavy(DayZPlayerType pType, DayzPlayerItemBehaviorCfg pBehavior)
	
	{
		super.RegisterHeavy( pType, pBehavior );
		pType.AddItemInHandsProfileIK("CN_MiningMachineBase",  "dz/anims/workspaces/player/player_main/props/player_main_heavy_generator.asi", pBehavior, "dz/anims/anm/player/ik/heavy/generator.anm");
		pType.AddItemInHandsProfileIK("CN_OilPump",  "dz/anims/workspaces/player/player_main/player_main_heavy.asi", pBehavior, "dz/anims/anm/player/ik/heavy/wooden_crate.anm");
	};
};