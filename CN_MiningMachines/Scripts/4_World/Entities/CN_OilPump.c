class CN_OilPump : CN_MiningMachineBase
{
    // Оставляем только то, что нужно для анимации вала
    protected bool m_AnimLoopActive = false;
    protected float m_ShaftPhase = 0.0;

    // Твой кастомный саундсет двигателя
    protected const string PUMP_SOUND_SET = "powerGenerator_low_Fuel_Loop_SoundSet"; //Bunker_Lamp_Hum_SoundSet

    void CN_OilPump()
    {
        // Включаем обновление кадров (FRAME) только на клиенте для вращения вала
        #ifndef SERVER
        SetEventMask(EntityEvent.FRAME);
        #endif
    }

    override void SetActions()
    {
        super.SetActions();
        AddAction(ActionStartRecycler);
        AddAction(ActionStopRecycler);
        AddAction(ActionPlugIn);
        AddAction(ActionUnplugThisByCord);
		AddAction(ActionPlaceObject);
		AddAction(ActionTogglePlaceObject);
    }

    override bool IsElectricAppliance()
    {
        return true;
    }

    override bool CanPutInCargo(EntityAI parent)
    {
        if (!super.CanPutInCargo(parent)) return false;
        return !GetCompEM().IsPlugged();
    }

    override bool CanPutIntoHands(EntityAI parent) 
    {
        if (!super.CanPutIntoHands(parent)) return false;
        return true;
    }

    // =================================================================
    // ИДЕАЛЬНОЕ ПЕРЕОПРЕДЕЛЕНИЕ ЭФФЕКТОВ БАЗОВОГО КЛАССА
    // =================================================================

    override protected void StartProcessingEffects()
    {
        if (m_EffectsActive || !GetGame().IsClient()) return;
        m_EffectsActive = true;

        // ИСПРАВЛЕНО: Вместо звука генератора принудительно запускаем ТВОЙ саундсет!
        m_ProcessingSound = SEffectManager.PlaySound("powerGenerator_low_Fuel_Loop_SoundSet", GetPosition());
        if (m_ProcessingSound) 
            m_ProcessingSound.SetSoundAutodestroy(false);

        // Дым тоже запускается как положено
        m_ProcessingParticle = Particle.PlayOnObject(ParticleList.POWER_GENERATOR_SMOKE, this);

        // Активируем вращение вала в EOnFrame
        m_AnimLoopActive = true;
    }

    override protected void StopProcessingEffects()
    {
        if (!m_EffectsActive || !GetGame().IsClient()) return;
        m_EffectsActive = false;

        // Чисто и без багов останавливаем кастомный звук
        if (m_ProcessingSound) 
        { 
            m_ProcessingSound.Stop(); 
            m_ProcessingSound = null; 
        }

        // Гасим партикл дыма
        if (m_ProcessingParticle) 
        { 
            m_ProcessingParticle.Stop(); 
            m_ProcessingParticle = null; 
        }

        // Останавливаем вращение вала
        m_AnimLoopActive = false;

        // Проигрываем ванильный звук остановки (по желанию можно убрать или заменить)
        SEffectManager.PlaySound("powerGeneratorStop_SoundSet", GetPosition());
    }

    // =================================================================
    // ВРАЩЕНИЕ ВАЛА (ОБНОВЛЕНИЕ КАДРОВ)
    // =================================================================

    override void EOnFrame(IEntity other, float timeSlice)
    {
        super.EOnFrame(other, timeSlice);

        #ifndef SERVER
        if (m_AnimLoopActive)
        {
            // Плавное смещение фазы под ограничения твоего model.cfg (max 0.08)
            m_ShaftPhase += 0.04 * timeSlice;

            if (m_ShaftPhase > 0.08)
            {
                m_ShaftPhase = 0.0;
            }

            // Передаем фазу вращения кости "shaft"
            SetAnimationPhase("shaft_rotation", m_ShaftPhase);
        }
        #endif
    }
	override bool IsDeployable()
	{
		return true;
	}
	override string GetDeploySoundset()
	{
		return "placeBatteryCharger_SoundSet";
	}

}
