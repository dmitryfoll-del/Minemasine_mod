modded class CN_OilPump
{
    protected bool IsEnergySourceRunning()
    {
        ComponentEnergyManager energy_manager = GetCompEM();
        if (!energy_manager) return false;
        if (!energy_manager.IsSwitchedOn()) return false;

        EntityAI power_source = energy_manager.GetEnergySource();
        if (!power_source) return false; 

        ComponentEnergyManager source_em = power_source.GetCompEM();
        if (source_em)
        {
            if (!source_em.IsSwitchedOn() || source_em.GetEnergy() <= 0) return false;
        }
        return true;
    }

    override bool Server_CanStartProcess()
    {
        if (!super.Server_CanStartProcess()) return false;
        if (!IsEnergySourceRunning()) return false;

        ItemBase outCanister = ItemBase.Cast(FindAttachmentBySlotName("RecycleOutput"));
        if (!outCanister) return false;
        if (outCanister.IsFullQuantity()) return false;
        
        if (outCanister.GetQuantity() > 0 && outCanister.GetLiquidType() != 16777216) return false;

        return IsNearOilDerrick();
    }

    override void Server_StartProcess()
    {
        super.Server_StartProcess();
        m_IsPumpWorking = true;
        SetSynchDirty(); 
    }

    override void Server_StopProcess()
    {
        super.Server_StopProcess();
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
                    string allowedDerrickClass = config.OilDerrickClassnames.Get(j);
                    if (allowedDerrickClass != "" && obj.IsKindOf(allowedDerrickClass)) return true;
                }
            }
        }
        return false; 
    }

    override void Server_ExecuteCycleTick()
    {
        if (!IsEnergySourceRunning() || !IsNearOilDerrick())
        {
            Server_StopProcess();
            return;
        }

        ItemBase outCanister = ItemBase.Cast(FindAttachmentBySlotName("RecycleOutput"));
        if (!outCanister || outCanister.IsFullQuantity())
        {
            Server_StopProcess();
            return;
        }

        if (outCanister.GetQuantity() > 0 && outCanister.GetLiquidType() != 16777216)
        {
            Server_StopProcess();
            return;
        }

        float fluidToPump = 300.0;
        
        // ИСПРАВЛЕНО: Заменили GetLiquidCapacity() на ванильный GetQuantityMax()
        float itemFreeSpace = outCanister.GetQuantityMax() - outCanister.GetQuantity(); 
        if (itemFreeSpace <= 0)
        {
            Server_StopProcess();
            return;
        }

        if (fluidToPump > itemFreeSpace) fluidToPump = itemFreeSpace;

        if (outCanister.GetQuantity() == 0)
        {
            outCanister.SetLiquidType(16777216); 
        }

        outCanister.AddQuantity(fluidToPump);

        if (outCanister.IsFullQuantity()) Server_StopProcess();
    }
}
