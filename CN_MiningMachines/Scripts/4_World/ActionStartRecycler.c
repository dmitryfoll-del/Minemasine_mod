class ActionStartRecycler : ActionInteractBase
{
    void ActionStartRecycler()
    {
        m_CommandUID = DayZPlayerConstants.CMD_ACTIONMOD_INTERACTONCE;
        m_StanceMask = DayZPlayerConstants.STANCEMASK_ERECT | DayZPlayerConstants.STANCEMASK_CROUCH;
        m_HUDCursorIcon = CursorIcons.CloseDoors;
		m_Text = "Включить";
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
        if (!machine || machine.IsProcessing()) 
            return false;

        // ПРОВЕРКА ДЛЯ ИНТЕРФЕЙСА (КЛИЕНТ):
        // Проверяем, что к станку физически подключен кабель питания (вилка в розетке).
        // Если провода нет вообще, действие не появится в GUI.
        if (!machine.GetCompEM() || !machine.GetCompEM().IsPlugged())
            return false;

        // Финальные тяжелые проверки (генератор, канистра, вышка из JSON) выполнит сервер при нажатии.
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
        if (machine) 
            machine.Server_StartProcess();
        #endif
    }
};
