class CN_OilPump : CN_MiningMachineBase
{
    // Текущая фаза анимации маховика
    protected float m_ShaftPhase = 0.0;
    // Флаг, работает ли анимация в текущий момент на клиенте
    protected bool m_AnimLoopActive = false;

    void CN_OilPump()
    {
        // Нам нужен апдейт кадра на клиенте для плавного изменения фазы анимации
        #ifndef DZ_SERVER
        SetEventMask(EntityEvent.FRAME);
        #endif
    }

    // Метод, который DayZ вызывает при изменении состояния синхронизированных переменных 
    // (например, когда сервер переключает режим работы станка)
    override void OnVariablesSynchronized()
    {
        super.OnVariablesSynchronized();

        // Проверяем статус работы станка из твоего базового класса.
        // Замени IsProcessing() на твой базовый геттер состояния работы машины!
        if (IsProcessing()) 
        {
            if (!m_AnimLoopActive)
            {
                m_AnimLoopActive = true;
            }
        }
        else
        {
            m_AnimLoopActive = false;
        }
    }

    // Кадровая отрисовка на клиенте для обеспечения идеальной плавности 60+ FPS
    override void OnUpdate(float timeslice)
    {
        super.OnUpdate(timeslice);

        #ifndef DZ_SERVER
        if (m_AnimLoopActive)
        {
            // Увеличиваем фазу анимации пропорционально времени кадра (timeslice)
            // Скорость вращения: 0.15 за секунду (можешь менять для ускорения/замедления)
            m_ShaftPhase += 0.15 * timeslice;

            // Поскольку в твоему model.cfg прописан sourceAddress="loop", 
            // при достижении значения maxValue (0.08) анимация должна сбрасываться в 0.0
            if (m_ShaftPhase > 0.08) // Твое maxValue из конфига
            {
                m_ShaftPhase = 0.0;
            }

            // Насильно принуждаем движок повернуть кость "shaft" на текущий шаг фазы
            // "shaft_rotation" — имя класса анимации из твоего CfgModels
            SetAnimationPhase("shaft_rotation", m_ShaftPhase);
        }
        #endif
    }

    // Подстраховка: если игрок подошел к работающей помпе, которая уже качает далеко от него,
    // метод инициализации на клиенте проверит стейт и сразу запустит вращение маховика
    override void EOnInit(IEntity other, int extra)
    {
        super.EOnInit(other, extra);

        #ifndef DZ_SERVER
        if (IsProcessing())
        {
            m_AnimLoopActive = true;
        }
        #endif
    }
}
