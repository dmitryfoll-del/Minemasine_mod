#ifdef DZ_SERVER

modded class CN_MiningMachineBase
{
    protected ref CN_MachineConfig m_ServerConfig;
    protected Timer m_ProcessTimer;

    override void EEInit()
    {
        super.EEInit();
        m_ServerConfig = CN_MachineConfigManager.GetInstance().LoadConfig(GetType());
        if (!m_ProcessTimer) m_ProcessTimer = new Timer(CALL_CATEGORY_SYSTEM);
    }

    CN_MachineConfig GetMachineConfig() { return m_ServerConfig; }

    bool Server_CanStartProcess()
    {
        if (m_IsProcessing || !m_ServerConfig || !IsPowered()) return false;
        ItemBase input = GetInputItem();
        if (!input || input.GetType() != m_ServerConfig.InputClass || input.GetQuantity() <= 0) return false;
        if (!CanAcceptOutput(m_ServerConfig.OutputClass, "RecycleOutput")) return false;
        if (m_ServerConfig.RareChance > 0.0 && !CanAcceptOutput(m_ServerConfig.RareClass, "RecycleRare")) return false;
        return true;
    }

    void Server_StartProcess()
    {
        if (!Server_CanStartProcess()) return;
        float processTime = m_ServerConfig.ProcessTimeSeconds;
        if (processTime < 0.1) processTime = 0.1;
        m_IsProcessing = true;
        SetSynchDirty();
        m_ProcessTimer.Run(processTime, this, "Server_ExecuteCycleTick", NULL, true);
    }

    void Server_ExecuteCycleTick()
    {
        if (!m_IsProcessing) return;
        if (!IsPowered()) { Server_StopProcess(); return; }
        ItemBase input = GetInputItem();
        if (!input || input.GetType() != m_ServerConfig.InputClass || input.GetQuantity() <= 0) { Server_StopProcess(); return; }
        if (input.GetQuantity() <= 1.0) input.Delete();
        else input.SetQuantity(input.GetQuantity() - 1.0);

        float wasteChance = Math.Clamp(m_ServerConfig.WasteChance, 0.0, 100.0);
        float rareChance = Math.Clamp(m_ServerConfig.RareChance, 0.0, 100.0 - wasteChance);
        float roll = Math.RandomFloat(0.0, 100.0);

        if (roll < wasteChance)
        {
            if (!CreateWaste()) { Server_StopProcess(); return; }
        }
        else if (roll < wasteChance + rareChance)
        {
            if (!AddOneToAttachment(m_ServerConfig.RareClass, "RecycleRare")) { Server_StopProcess(); return; }
        }
        else if (!AddOneToAttachment(m_ServerConfig.OutputClass, "RecycleOutput"))
        {
            Server_StopProcess();
            return;
        }

        if (!GetInputItem()) Server_StopProcess();
    }

    void Server_StopProcess()
    {
        if (m_ProcessTimer) m_ProcessTimer.Stop();
        if (!m_IsProcessing) return;
        m_IsProcessing = false;
        SetSynchDirty();
    }

    protected ItemBase GetInputItem() { return ItemBase.Cast(GetInventory().FindAttachment(InventorySlots.GetSlotIdFromString("RecycleInput"))); }
    protected ItemBase GetAttachmentItem(string slotName) { return ItemBase.Cast(GetInventory().FindAttachment(InventorySlots.GetSlotIdFromString(slotName))); }

    protected bool CanAcceptOutput(string className, string slotName)
    {
        if (className == "") return false;
        ItemBase existing = GetAttachmentItem(slotName);
        if (!existing) return true;
        return existing.GetType() == className && existing.GetQuantity() < existing.GetQuantityMax();
    }

    protected bool AddOneToAttachment(string className, string slotName)
    {
        if (className == "") return false;
        int slotId = InventorySlots.GetSlotIdFromString(slotName);
        ItemBase existing = ItemBase.Cast(GetInventory().FindAttachment(slotId));
        if (existing)
        {
            if (existing.GetType() != className || existing.GetQuantity() >= existing.GetQuantityMax()) return false;
            existing.SetQuantity(existing.GetQuantity() + 1.0);
            return true;
        }
        ItemBase createdItem = ItemBase.Cast(GetInventory().CreateAttachmentEx(className, slotId));
        if (!createdItem) return false;
        createdItem.SetQuantity(1.0);
        return true;
    }

    protected bool CreateWaste()
    {
        if (m_ServerConfig.WasteClass == "") return true;
        return GetInventory().CreateInInventory(m_ServerConfig.WasteClass) != null;
    }

    override void EEDelete(EntityAI parent)
    {
        Server_StopProcess();
        super.EEDelete(parent);
    }
};

#endif