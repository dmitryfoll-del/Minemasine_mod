modded class CN_OilDistiller
{
    protected bool          m_IsFireBurning = false;
    protected float         m_FuelTankCapacity = 1000.0; 
    protected float         m_FuelToEnergyRatio;        

    void CN_OilDistiller()
    {
        SetEventMask(EntityEvent.FRAME);
        SetTemperature(20.0);
    }

    override void OnInitEnergy()
    {
        super.OnInitEnergy();
        if (GetCompEM())
        {
            m_FuelToEnergyRatio = GetCompEM().GetEnergyMax() / m_FuelTankCapacity;
        }
    }

    override bool CanIgniteItem(EntityAI ignite_target = null)
    {
        return (!m_IsFireBurning && GetCompEM() && GetCompEM().GetEnergy() > 0);
    }

    override void OnIgnitedThis(EntityAI fire_source)
    {
        super.OnIgnitedThis(fire_source);
        m_IsFireBurning = true;
        if (GetCompEM()) GetCompEM().SwitchOn();
    }

    float AddToopkaFuel(float fuel_amount)
    {
        if (!GetCompEM() || fuel_amount <= 0.0) return 0.0;

        float currentFuel = GetCompEM().GetEnergy() / m_FuelToEnergyRatio;
        float neededFuel = m_FuelTankCapacity - currentFuel;

        if (neededFuel <= 0) return 0.0;

        float acceptedFuel = fuel_amount;
        if (acceptedFuel > neededFuel) acceptedFuel = neededFuel;

        GetCompEM().SetEnergy((currentFuel + acceptedFuel) * m_FuelToEnergyRatio);
        return acceptedFuel;
    }

    override void OnWork(float consumed_energy)
    {
        super.OnWork(consumed_energy);
        if (GetCompEM() && GetCompEM().GetEnergy() <= 0)
        {
            GetCompEM().SwitchOff(); 
        }
    }

    override void OnWorkStop()
    {
        super.OnWorkStop();
        m_IsFireBurning = false;
    }

    override void EOnFrame(IEntity other, float timeSlice)
    {
        super.EOnFrame(other, timeSlice);

        if (m_IsFireBurning && GetCompEM() && GetCompEM().IsWorking())
        {
            if (GetTemperature() < 300.0) SetTemperature(GetTemperature() + (5.0 * timeSlice));
        }
        else
        {
            if (GetTemperature() > 20.0) SetTemperature(GetTemperature() - (1.5 * timeSlice));
            if (m_IsFireBurning) m_IsFireBurning = false;
        }
    }

    override bool Server_CanStartProcess()
    {
        if (!super.Server_CanStartProcess()) return false;
        if (GetTemperature() < 100.0) return false; 

        // ИСПРАВЛЕНО: Bottle_Base
        Bottle_Base inputItem = Bottle_Base.Cast(FindAttachmentBySlotName("RecycleInput"));
        // ИСПРАВЛЕНО: Проверка нефти по ID маски (16777216)
        if (!inputItem || inputItem.GetQuantity() <= 0 || inputItem.GetLiquidType() != 16777216) return false;

        return true;
    }

    override void Server_ExecuteCycleTick()
    {
        if (GetTemperature() < 100.0) { Server_StopProcess(); return; }

        // ИСПРАВЛЕНО: Bottle_Base для всех трех канистр станка
        Bottle_Base inputCanister = Bottle_Base.Cast(FindAttachmentBySlotName("RecycleInput"));
        Bottle_Base outGasoline = Bottle_Base.Cast(FindAttachmentBySlotName("RecycleOutput")); 
        Bottle_Base outKerosene = Bottle_Base.Cast(FindAttachmentBySlotName("RecycleRare"));   

        // ИСПРАВЛЕНО: Проверка Нефти на входе по ID маски (16777216)
        if (!inputCanister || inputCanister.GetQuantity() <= 0 || inputCanister.GetLiquidType() != 16777216)
        { 
            Server_StopProcess(); 
            return; 
        }

        // Проверка выходов: ванильный бензин (8192) остается прежним
        bool canOutputGasoline = (outGasoline && !outGasoline.IsFullQuantity() && (outGasoline.GetQuantity() == 0 || outGasoline.GetLiquidType() == 8192));
        // ИСПРАВЛЕНО: Проверка кастомного Керосина по ID маски (8388608)
        bool canOutputKerosene = (outKerosene && !outKerosene.IsFullQuantity() && (outKerosene.GetQuantity() == 0 || outKerosene.GetLiquidType() == 8388608));

        if (!canOutputGasoline && !canOutputKerosene) { Server_StopProcess(); return; }

        float oilVolumeConsumed = 200.0;
        if (inputCanister.GetQuantity() < oilVolumeConsumed) oilVolumeConsumed = inputCanister.GetQuantity();

        bool bSuccess = false;

        if (canOutputGasoline)
        {
            if (outGasoline.GetQuantity() == 0) outGasoline.SetLiquidType(8192); 
            outGasoline.AddQuantity(oilVolumeConsumed * 0.4);
            bSuccess = true;
        }

        if (canOutputKerosene)
        {
            // ИСПРАВЛЕНО: Заливаем керосин по ID маски (8388608)
            if (outKerosene.GetQuantity() == 0) outKerosene.SetLiquidType(8388608); 
            outKerosene.AddQuantity(oilVolumeConsumed * 0.3);
            bSuccess = true;
        }

        if (bSuccess) inputCanister.AddQuantity(-oilVolumeConsumed);
        if (inputCanister.GetQuantity() <= 0) Server_StopProcess();
    }
}
