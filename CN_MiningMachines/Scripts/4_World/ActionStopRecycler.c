class ActionStopRecycler : ActionInteractBase
{
    void ActionStopRecycler()
    {
        m_CommandUID        = DayZPlayerConstants.CMD_ACTIONMOD_OPENDOORFW; 
        m_StanceMask        = DayZPlayerConstants.STANCEMASK_CROUCH | DayZPlayerConstants.STANCEMASK_ERECT;
        m_HUDCursorIcon     = CursorIcons.CloseDoors; 
		m_Text = "Выключить";
    }

    override void CreateConditionComponents()  
    {
        m_ConditionItem     = new CCINone;
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
        
        // Кнопка появится, если станок существует и он сейчас работает.
        // Метод IsProcessing() у вас общий, поэтому клиент его видит без проблем.
        if (!machine || !machine.IsProcessing())
            return false;

        return true;
    }

    // ИСПРАВЛЕНО: Используем OnExecuteServer вместо OnStartServer
    override void OnExecuteServer(ActionData action_data)
    {
        super.OnExecuteServer(action_data);

        // Защищаем код от клиентского компилятора. 
        // Теперь эту строчку увидит ТОЛЬКО сервер, где функция Server_StopProcess точно существует!
        #ifdef DZ_SERVER
        CN_MiningMachineBase machine = CN_MiningMachineBase.Cast(action_data.m_Target.GetObject());
        if (machine)
        {
            // Сервер успешно останавливает таймеры и добычу
            machine.Server_StopProcess();
            
            Print("[CN_MiningMachines] Игрок " + action_data.m_Player.GetIdentity().GetName() + " вручную выключил станок: " + machine.GetType());
        }
        #endif
    }
}
