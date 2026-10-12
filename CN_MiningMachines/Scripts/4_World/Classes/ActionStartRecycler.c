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

    override string GetText()
    {
        return "Включить";
    }

    override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
    {
        if (!target || !target.GetObject()) 
            return false;

        CN_MiningMachineBase machine = CN_MiningMachineBase.Cast(target.GetObject());
        if (!machine || machine.IsProcessing()) 
            return false;

        // ПРОВЕРКА ДЛЯ ИНТЕРФЕЙСА (И КЛИЕНТ, И СЕРВЕР):
        // Проверяем только базовый физический статус: кабель питания должен быть в розетке
        if (!machine.GetCompEM() || !machine.GetCompEM().IsPlugged())
            return false;

        // ИСПРАВЛЕНО: Убираем отсюда тяжелую серверную проверку вышки и конфигов.
        // Экшен должен беспрепятственно пропустить запрос на сервер!
        return true;
    }

		override void OnExecuteServer(ActionData action_data)
	{
		CN_MiningMachineBase machine = CN_MiningMachineBase.Cast(action_data.m_Target.GetObject());
		if (machine && machine.GetCompEM())
		{
			machine.GetCompEM().SwitchOn(); // Просто щелкаем тумблером, остальное сделает движок!
		}
	}

};
