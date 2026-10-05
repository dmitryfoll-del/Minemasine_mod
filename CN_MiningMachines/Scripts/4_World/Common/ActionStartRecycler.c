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
        if (!target || !target.GetObject())
            return false;

        CN_MiningMachineBase machine = CN_MiningMachineBase.Cast(target.GetObject());
        if (!machine)
            return false;

        if (machine.IsProcessing())
            return false;

        if (!machine.IsPowered())
            return false;

        #ifdef DZ_SERVER
        if (!machine.Server_CanStartProcess())
            return false;
        #else
        ItemBase input = ItemBase.Cast(machine.GetInventory().FindAttachment(InventorySlots.GetSlotIdFromString("RecycleInput")));
        if (!input)
            return false;

        return input.GetQuantity() > 0;
        #endif
    }

    override void OnExecuteServer(ActionData action_data)
    {
        CN_MiningMachineBase machine = CN_MiningMachineBase.Cast(action_data.m_Target.GetObject());
        if (!machine)
            return;

        machine.Server_StartProcess();
    }
};
