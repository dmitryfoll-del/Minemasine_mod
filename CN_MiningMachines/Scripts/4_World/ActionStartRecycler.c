class ActionStartRecycler : ActionInteractBase
{
    void ActionStartRecycler()
    {
        m_CommandUID = DayZPlayerConstants.CMD_ACTIONMOD_INTERACTONCE;
        m_StanceMask = DayZPlayerConstants.STANCEMASK_ERECT | DayZPlayerConstants.STANCEMASK_CROUCH;
        m_HUDCursorIcon = CursorIcons.CloseDoors;
    }

    override void CreateConditionComponents()
    {
        m_ConditionItem = new CCINone();
        m_ConditionTarget = new CCTObject(UAMaxDistances.DEFAULT);
    }

    override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
    {
        if (!target || !target.GetObject()) return false;
        CN_MiningMachineBase machine = CN_MiningMachineBase.Cast(target.GetObject());
        if (!machine || machine.IsProcessing() || !machine.IsPowered()) return false;
        #ifdef DZ_SERVER
        return machine.Server_CanStartProcess();
        #else
        return true;
        #endif
    }

    override void OnExecuteServer(ActionData action_data)
    {
        #ifdef DZ_SERVER
        CN_MiningMachineBase machine = CN_MiningMachineBase.Cast(action_data.m_Target.GetObject());
        if (machine) machine.Server_StartProcess();
        #endif
    }
};
