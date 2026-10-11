class ActionStopRecycler : ActionInteractBase
{
    void ActionStopRecycler()
    {
        m_CommandUID        = DayZPlayerConstants.CMD_ACTIONMOD_OPENDOORFW; 
        m_StanceMask        = DayZPlayerConstants.STANCEMASK_CROUCH | DayZPlayerConstants.STANCEMASK_ERECT;
        m_HUDCursorIcon     = CursorIcons.CloseDoors; 
        m_Text              = "Выключить";
    }

    override void CreateConditionComponents()  
    {
        m_ConditionItem     = new CCINone();
        m_ConditionTarget   = new CCTObject(UAMaxDistances.DEFAULT);
    }

    override string GetText()
    {
        return "Выключить";
    }

    override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
    {
        if (!target || !target.GetObject())
            return false;

        CN_MiningMachineBase machine = CN_MiningMachineBase.Cast(target.GetObject());
        
        // Кнопка "Выключить" появится, только если станок существует и он СЕЙЧАС РАБОТАЕТ
        if (!machine || !machine.IsProcessing())
            return false;

        return true;
    }

    override void OnExecuteServer(ActionData action_data)
    {
        super.OnExecuteServer(action_data);

        CN_MiningMachineBase machine = CN_MiningMachineBase.Cast(action_data.m_Target.GetObject());
        if (machine && machine.GetCompEM())
        {
            // ИСПРАВЛЕНО: Вместо прямого вызова остановки скрипта, принудительно гасим 
            // тумблер энергоменеджера на сервере. Ванильное событие OnWorkStop само сделает всё остальное!
            machine.GetCompEM().SwitchOff();
            
            if (action_data.m_Player && action_data.m_Player.GetIdentity())
            {
                Print("[CN_MiningMachines] Игрок " + action_data.m_Player.GetIdentity().GetName() + " вручную выключил тумблер станка: " + machine.GetType());
            }
        }
    }
}
