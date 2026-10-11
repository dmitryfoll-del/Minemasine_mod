
modded class CN_OilPump
{
    protected bool IsEnergySourceRunning()
    {
        ComponentEnergyManager energyManager = GetCompEM();
        if (!energyManager || !energyManager.IsSwitchedOn()) return false;

        EntityAI powerSource = energyManager.GetEnergySource();
        if (!powerSource) return false;

        ComponentEnergyManager sourceEM = powerSource.GetCompEM();
        if (sourceEM && (!sourceEM.IsSwitchedOn() || sourceEM.GetEnergy() <= 0)) return false;

        return true;
    }

    protected ItemBase GetOutputCanister()
    {
        return ItemBase.Cast(FindAttachmentBySlotName("RecycleOutput"));
    }

    protected bool CanPumpIntoCanister(ItemBase canister)
    {
        if (!canister || canister.IsFullQuantity()) return false;

        if (canister.GetQuantity() > 0 && canister.GetLiquidType() != 16777216) return false;

        return true;
    }

    override bool Server_CanStartProcess()
    {
        if (m_IsProcessing) return false;
        if (!IsEnergySourceRunning()) return false;

        ItemBase outputCanister = GetOutputCanister();
        if (!CanPumpIntoCanister(outputCanister)) return false;

        if (!IsNearOilDerrick()) return false;

        return true;
    }

    override void Server_StartProcess()
    {
        if (!Server_CanStartProcess()) return;

        if (!m_ProcessTimer) m_ProcessTimer = new Timer(CALL_CATEGORY_SYSTEM);

        float processTime = 2.0;
        CN_OilPumpConfig config = CN_OilPumpConfig.Cast(CN_MachineConfigManager.GetInstance().LoadConfig("CN_OilPump"));
        if (config && config.ProcessTimeSeconds >= 0.1) processTime = config.ProcessTimeSeconds;

        m_IsProcessing = true;
        m_IsPumpWorking = true;
        SetSynchDirty();

        m_ProcessTimer.Run(processTime, this, "Server_ExecuteCycleTick", NULL, true);
    }

    override void Server_StopProcess()
    {
        if (m_ProcessTimer) m_ProcessTimer.Stop();

        m_IsProcessing = false;
        m_IsPumpWorking = false;
        SetSynchDirty();
    }

    protected bool IsNearOilDerrick()
    {
        CN_OilPumpConfig config = CN_OilPumpConfig.Cast(CN_MachineConfigManager.GetInstance().LoadConfig("CN_OilPump"));

        float checkRadius = 15.0;
        if (config) checkRadius = config.OilDerrickCheckRadius;

        array<Object> nearbyObjects = new array<Object>;
        GetGame().GetObjectsAtPosition(GetPosition(), checkRadius, nearbyObjects, null);

        for (int i = 0; i < nearbyObjects.Count(); i++)
        {
            Object obj = nearbyObjects.Get(i);
            if (!obj) continue;

            if (obj.IsKindOf("cn_LAND_OilPump")) return true;

            if (config && config.OilDerrickClassnames)
            {
                for (int j = 0; j < config.OilDerrickClassnames.Count(); j++)
                {
                    string derrickClass = config.OilDerrickClassnames.Get(j);
                    if (derrickClass != "" && obj.IsKindOf(derrickClass)) return true;
                }
            }
        }

        return false;
    }

    override void Server_ExecuteCycleTick()
    {
        if (!m_IsProcessing) return;

        if (!IsEnergySourceRunning() || !IsNearOilDerrick())
        {
            Server_StopProcess();
            return;
        }

        ItemBase outputCanister = GetOutputCanister();
        if (!CanPumpIntoCanister(outputCanister))
        {
            Server_StopProcess();
            return;
        }

        float fluidToPump = 300.0;
        float freeSpace = outputCanister.GetQuantityMax() - outputCanister.GetQuantity();

        if (freeSpace <= 0)
        {
            Server_StopProcess();
            return;
        }

        if (fluidToPump > freeSpace) fluidToPump = freeSpace;

        if (outputCanister.GetQuantity() <= 0)
            outputCanister.SetLiquidType(16777216);

        outputCanister.AddQuantity(fluidToPump);

        if (outputCanister.IsFullQuantity())
            Server_StopProcess();
    }
}
