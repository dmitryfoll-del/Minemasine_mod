class CN_OilPump : CN_MiningMachineBase
{
    // Сетевая переменная синхронизации стейта работы помпы
    protected bool m_IsPumpWorking;

    // Внутренние клиентские переменные для анимации
    protected float m_ShaftPhase = 0.0;
    protected bool m_AnimLoopActive = false;

    // --- ПЕРЕМЕННЫЕ ДЛЯ ЗВУКА ---
    protected ref EffectSound m_PumpLoopSound;
    protected const string PUMP_SOUND_SET = "CN_RAM_TRX_Engine_Ext_Rpm0_SoundSet";

    void CN_OilPump()
    {
        // Регистрируем логический флаг в сетевой системе DayZ
        RegisterNetSyncVariableBool("m_IsPumpWorking");

        // Включаем обновление кадров (FRAME) только на клиенте
        #ifndef DZ_SERVER
        SetEventMask(EntityEvent.FRAME);
        #endif
    }

    // Деструктор: чистим звуковые эффекты во избежание утечки памяти или зависшего звука в воздухе
    void ~CN_OilPump()
    {
        #ifndef DZ_SERVER
        if (m_PumpLoopSound)
        {
            SEffectManager.DestroyEffect(m_PumpLoopSound);
        }
        #endif
    }

    override void SetActions()
    {
        super.SetActions();
        AddAction(ActionStartRecycler);
        AddAction(ActionStopRecycler); // Наш новый экшен выключения теперь тоже тут!
        AddAction(ActionPlugIn);
        AddAction(ActionUnplugThisByCord);
    }

    // ОБЯЗАТЕЛЬНО: Сигнализируем движку DayZ, что это электрический прибор
    override bool IsElectricAppliance()
    {
        return true;
    }

    override bool CanPutInCargo(EntityAI parent)
	{
		if (!super.CanPutInCargo(parent)) 
			return false;
		
		return !GetCompEM().IsPlugged();
	}

	override bool CanPutIntoHands(EntityAI parent) 
	{
		if (!super.CanPutIntoHands(parent))
		{
			return false;
		}
		// Commented out so Reposition action is possible to execute
		return true;
	}

    // Вызывается у клиентов, когда сервер прислал обновленный статус m_IsPumpWorking
    override void OnVariablesSynchronized()
    {
        super.OnVariablesSynchronized();

        if (m_IsPumpWorking)
        {
            m_AnimLoopActive = true;
            PlayPumpSound();
        }
        else
        {
            m_AnimLoopActive = false;
            StopPumpSound();
        }
    }

    // Включение зацикленного звука работы на клиенте
    protected void PlayPumpSound()
    {
        #ifndef SERVER
        if (!m_PumpLoopSound || !m_PumpLoopSound.IsSoundPlaying())
        {
            // Спавним зацикленный звук на позиции нашей модели
            m_PumpLoopSound = SEffectManager.PlaySoundOnObject(PUMP_SOUND_SET, this, 0, 0, true);
            
            if (m_PumpLoopSound)
            {
                m_PumpLoopSound.SetSoundFadeIn(1.0); // Плавный старт звука (1 секунда)
            }
        }
        #endif
    }

    // Отключение звука работы
    protected void StopPumpSound()
    {
        #ifndef SERVER
        if (m_PumpLoopSound && m_PumpLoopSound.IsSoundPlaying())
        {
            m_PumpLoopSound.SetSoundFadeOut(0.5); // Плавное затухание при остановке (0.5 секунды)
            m_PumpLoopSound.SoundStop();
        }
        #endif
    }

    // Точное ванильное имя аргумента с большой буквы 'timeSlice'
    override void EOnFrame(IEntity other, float timeSlice)
    {
        super.EOnFrame(other, timeSlice);

        #ifndef DZ_SERVER
        if (m_AnimLoopActive)
        {
            // Смещение фазы. Используем точную переменную timeSlice из аргументов
            m_ShaftPhase += 0.15 * timeSlice;

            // Сброс в 0 при достижении maxValue (0.08) из твоего model.cfg
            if (m_ShaftPhase > 0.08)
            {
                m_ShaftPhase = 0.0;
            }

            // Вращаем кость "shaft" на клиенте по классу анимации "shaft_rotation"
            SetAnimationPhase("shaft_rotation", m_ShaftPhase);
        }
        #endif
    }

    // Запуск анимации и звука, если игрок прибежал к уже работающему станку
    override void EOnInit(IEntity other, int extra)
    {
        super.EOnInit(other, extra);

        #ifndef DZ_SERVER
        if (m_IsPumpWorking)
        {
            m_AnimLoopActive = true;
            PlayPumpSound();
        }
        #endif
    }
}
