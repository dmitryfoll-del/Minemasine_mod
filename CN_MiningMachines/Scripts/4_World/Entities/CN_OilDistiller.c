class CN_OilDistiller : CN_MiningMachine_Base
{
    // Переменная для синхронизации состояния работы между сервером и клиентами
    protected bool m_IsProcessing;

    // Ссылки на активные эффекты (клиентская часть)
    protected EffectSound m_BoilingSound;
    protected Particle m_ExhaustParticle;

    void CN_OilDistiller()
    {
        // Регистрируем переменную в сетевой маске для автоматической синхронизации
        RegisterNetSyncVariableBool("m_IsProcessing");
    }

    // Метод вызывается на клиенте всякий раз, когда сервер обновляет синхронизированные переменные
    override void OnVariablesSynchronized()
    {
        super.OnVariablesSynchronized();

        if (m_IsProcessing)
        {
            Client_StartEffects();
        }
        else
        {
            Client_StopEffects();
        }
    }

    // --- СЕРВЕРНЫЕ ИНТЕРФЕЙСНЫЕ МЕТОДЫ (Переопределяются в серверном моде) ---
    bool Server_CanStartProcess()
    {
        return false;
    }

    void Server_StartProcess() {}
    void Server_StopProcess() {}
    void Server_ExecuteCycleTick() {}

    // --- КЛИЕНТСКАЯ АУДИОВИЗУАЛЬНАЯ ЛОГИКА ---
    protected void Client_StartEffects()
    {
        // Защита: эффекты запускаются только на стороне клиента (или в синглплеере)
        if (GetGame().IsServer() && !GetGame().IsMultiplayer()) return;

        // 1. ЗАПУСК ЗВУКА КИПЕНИЯ / ПЕРЕГОНКИ
        if (!m_BoilingSound)
        {
            // Используется кастомный саундсет, который должен быть прописан в твоем config.cpp
            // Если кастомного звука нет, можно временно поставить ванильный "SND_Boiling_Loop"
            m_BoilingSound = SEffectManager.PlaySoundOnObject("CN_OilDistiller_Boiling_SoundSet", this, 0, 0, true);
        }

        // 2. ЗАПУСК ПАРТИКЛА ДЫМА / ВЫХЛОПА
        if (!m_ExhaustParticle)
        {
            // Воспроизводим партикл (например, ванильный дым печки ParticleList.CAMP_STOVE_CHIMNEY)
            // И привязываем его к точке "Proxy_Exhaust" или "smoke" в памяти твоей 3D-модели (p3d)
            m_ExhaustParticle = ParticleManager.GetInstance().PlayOnObject(ParticleList.CAMP_STOVE_CHIMNEY, this, GetMemoryPointPos("smoke"));
        }
    }

    protected void Client_StopEffects()
    {
        if (GetGame().IsServer() && !GetGame().IsMultiplayer()) return;

        // Останавливаем и очищаем звук
        if (m_BoilingSound)
        {
            m_BoilingSound.SoundStop();
            m_BoilingSound = null;
        }

        // Останавливаем партикл выхлопа
        if (m_ExhaustParticle)
        {
            m_ExhaustParticle.Stop();
            m_ExhaustParticle = null;
        }
    }

    // Корректно подчищаем эффекты, если станок удаляется из мира (например, удален админом или скрылся из зоны видимости)
    override void EEDelete(EntityAI parent)
    {
        super.EEDelete(parent);
        Client_StopEffects();
    }
}
