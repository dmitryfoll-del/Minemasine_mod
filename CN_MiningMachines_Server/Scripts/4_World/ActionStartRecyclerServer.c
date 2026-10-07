#ifdef DZ_SERVER

modded class ActionStartRecycler
{
    override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
    {
        if (!target || !target.GetObject()) return false;
        CN_MiningMachineBase machine = CN_MiningMachineBase.Cast(target.GetObject());
        if (!machine) return false;
        return machine.Server_CanStartProcess();
    }
};

#endif