class CN_OilPump : CN_MiningMachineBase
{
    // Сетевая переменная синхронизации стейта работы помпы
    protected bool m_IsPumpWorking;

    // Внутренние клиентские переменные для анимации
    protected float m_ShaftPhase = 0.0;
    protected bool m_AnimLoopActive = false;

    void CN_OilPump()
    {
        // Регистрируем логический флаг в сетевой системе DayZ
        RegisterNetSyncVariableBool("m_IsPumpWorking");

        // Включаем обновление кадров (FRAME) только на клиенте
        #ifndef DZ_SERVER
        SetEventMask(EntityEvent.FRAME);
        #endif
    }

    // Вызывается у клиентов, когда сервер прислал обновленный статус m_IsPumpWorking
    override void OnVariablesSynchronized()
    {
        super.OnVariablesSynchronized();

        if (m_IsPumpWorking)
            m_AnimLoopActive = true;
        else
            m_AnimLoopActive = false;
    }

    // Кадровая цикличная прокрутка кости "shaft"
    override void OnUpdate(float timeslice)
    {
        super.OnUpdate(timeslice);

        #ifndef DZ_SERVER
        if (m_AnimLoopActive)
        {
            // Смещение фазы. Настройка скорости (0.15)
            m_ShaftPhase += 0.15 * timeslice;

            // Сброс в 0 при достижении maxValue (0.08) из твоего model.cfg
            if (m_ShaftPhase > 0.08)
            {
                m_ShaftPhase = 0.0;
            }

            // Вращаем кость на клиенте
            SetAnimationPhase("shaft_rotation", m_ShaftPhase);
        }
        #endif
    }

    // Запуск анимации, если игрок прибежал к уже работающему станку
    override void EOnInit(IEntity other, int extra)
    {
        super.EOnInit(other, extra);

        #ifndef DZ_SERVER
        if (m_IsPumpWorking)
            m_AnimLoopActive = true;
        #endif
    }
     // --- ДОБАВЛЯЕМ СВЯЗКУ С ЭКШЕНАМИ ---
    override void SetActions()
    {
        super.SetActions(); // Наследуем базовые экшены (например, взять в руки, если разрешено)

        // Регистрируем твои кастомные действия автоматизации для этого насоса.
        // Замени имена классов экшенов на точные названия из твоего мода!
        AddAction(ActionStartRecycler); // Появляется кнопка "Включить / Запустить"
        AddAction(ActionStopRecycler);  // Появляется кнопка "Выключить / Остановить"
    }
}
