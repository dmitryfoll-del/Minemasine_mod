class CN_MiningMachineBase extends ItemBase
{
    protected bool m_IsProcessing;
    protected bool m_EffectsActive;
    protected EffectSound m_ProcessingSound;
    protected Particle m_ProcessingParticle;

    void CN_MiningMachineBase()
    {
        m_IsProcessing = false;
        m_EffectsActive = false;

        RegisterNetSyncVariableBool("m_IsProcessing");
    }

    bool IsProcessing()
    {
        return m_IsProcessing;
    }

    bool IsPowered()
    {
        ComponentEnergyManager energyManager = GetCompEM();
        if (!energyManager)
            return false;

        return energyManager.IsPlugged() && energyManager.IsWorking();
    }

    override bool CanReceiveAttachment(EntityAI attachment, int slotId)
    {
        if (!super.CanReceiveAttachment(attachment, slotId))
            return false;

        if (!m_IsProcessing)
            return true;

        string slotName = InventorySlots.GetSlotName(slotId);

        if (slotName == "RecycleInput" || slotName == "RecycleOutput" || slotName == "RecycleRare")
            return false;

        return true;
    }

    override bool CanReleaseAttachment(EntityAI attachment)
    {
        if (!super.CanReleaseAttachment(attachment))
            return false;

        if (!m_IsProcessing || !attachment)
            return true;

        InventoryLocation location = new InventoryLocation();
        attachment.GetInventory().GetCurrentInventoryLocation(location);

        if (!location || !location.IsValid())
            return true;

        string slotName = InventorySlots.GetSlotName(location.GetSlot());

        if (slotName == "RecycleInput" || slotName == "RecycleOutput" || slotName == "RecycleRare")
            return false;

        return true;
    }

    override void OnVariablesSynchronized()
    {
        super.OnVariablesSynchronized();

        if (m_IsProcessing && !m_EffectsActive)
            StartProcessingEffects();
        else if (!m_IsProcessing && m_EffectsActive)
            StopProcessingEffects();
    }

    protected void StartProcessingEffects()
    {
        if (m_EffectsActive)
            return;

        m_EffectsActive = true;

        if (!GetGame().IsClient())
            return;

        m_ProcessingSound = SEffectManager.PlaySound("powerGeneratorLoop_SoundSet", GetPosition());
        if (m_ProcessingSound)
            m_ProcessingSound.SetSoundAutodestroy(false);

        m_ProcessingParticle = ParticleManager.GetInstance().PlayOnObject(ParticleList.BARREL_SMOKE, this);
    }

    protected void StopProcessingEffects()
    {
        if (!m_EffectsActive)
            return;

        m_EffectsActive = false;

        if (!GetGame().IsClient())
            return;

        if (m_ProcessingSound)
        {
            m_ProcessingSound.Stop();
            m_ProcessingSound = null;
        }

        if (m_ProcessingParticle)
        {
            m_ProcessingParticle.Stop();
            m_ProcessingParticle = null;
        }

        SEffectManager.PlaySound("powerGeneratorStop_SoundSet", GetPosition());
    }
};
