
class CN_OilDistiller : CN_MiningMachineBase
{
    protected bool m_IsFireBurning = false;
    protected float m_FireFuelEnergy = 0;

    void CN_OilDistiller()
    {
        // Включаем обновление FRAME для симуляции горения костра
        SetEventMask(EntityEvent.FRAME);
        #ifdef DZ_SERVER
        SetTemperature(20.0); 
        #endif
    }

    override bool CanIgniteItem(EntityAI ignite_source = null)
    {
        #ifdef DZ_SERVER
        return (!m_IsFireBurning && HasFuelInToopka());
        #else
        return true;
        #endif
    }

    override void OnIgniteItem(EntityAI ignite_source)
    {
        super.OnIgniteItem(ignite_source);
        #ifdef DZ_SERVER
        m_IsFireBurning = true;
        #endif
    }

    bool HasFuelInToopka()
    {
        return (FindAttachmentBySlotName("Firewood") || FindAttachmentBySlotName("WoodenStick"));
    }

    override void OnUpdate(float timeslice)
    {
        super.OnUpdate(timeslice);

        #ifdef DZ_SERVER
        if (m_IsFireBurning)
        {
            if (m_FireFuelEnergy <= 0)
            {
                if (!ConsumeWoodFuel())
                    m_IsFireBurning = false; 
            }
            else
            {
                m_FireFuelEnergy -= timeslice;
            }

            if (GetTemperature() < 300.0)
                SetTemperature(GetTemperature() + (4.0 * timeslice));
        }
        else
        {
            if (GetTemperature() > 20.0)
                SetTemperature(GetTemperature() - (1.0 * timeslice));
        }
        #endif
    }

    #ifdef DZ_SERVER
    protected bool ConsumeWoodFuel()
    {
        ItemBase fuel = ItemBase.Cast(FindAttachmentBySlotName("Firewood"));
        if (fuel && fuel.GetQuantity() > 0)
        {
            fuel.AddQuantity(-1);
            m_FireFuelEnergy += 90.0; 
            return true;
        }

        fuel = ItemBase.Cast(FindAttachmentBySlotName("WoodenStick"));
        if (fuel && fuel.GetQuantity() > 0)
        {
            fuel.AddQuantity(-1);
            m_FireFuelEnergy += 30.0;
            return true;
        }
        return false;
    }

    // Проверка условий запуска (Правила мода)
    override bool Server_CanStartProcess()
    {
        if (!super.Server_CanStartProcess())
            return false;

        // Требуем нагрев корпуса выше 100°C
        if (GetTemperature() < 100.0)
            return false; 

        ItemBase inputItem = ItemBase.Cast(FindAttachmentBySlotName("RecycleInput"));
        if (!inputItem || inputItem.GetQuantity() <= 0 || inputItem.GetLiquidType() != CN_LiquidTypes.CRUDE_OIL)
            return false;

        return true;
    }

    // Главный цикл обработки вашего мода по тику таймера
    override void Server_ExecuteCycleTick()
    {
        if (!IsPowered() || GetTemperature() < 100.0)
        {
            Server_StopProcess();
            return;
        }

        ItemBase inputCanister = ItemBase.Cast(FindAttachmentBySlotName("RecycleInput"));
        ItemBase outGasoline = ItemBase.Cast(FindAttachmentBySlotName("RecycleOutput")); 
        ItemBase outKerosene = ItemBase.Cast(FindAttachmentBySlotName("RecycleRare"));   

        if (!inputCanister || inputCanister.GetQuantity() <= 0 || inputCanister.GetLiquidType() != CN_LiquidTypes.CRUDE_OIL)
        {
            Server_StopProcess();
            return;
        }

        // Проверяем, свободны ли выходы (Бензин 8192)
        bool canOutputGasoline = (outGasoline && !outGasoline.IsFullQuantity() && (outGasoline.GetQuantity() == 0 || outGasoline.GetLiquidType() == 8192));
        bool canOutputKerosene = (outKerosene && !outKerosene.IsFullQuantity() && (outKerosene.GetQuantity() == 0 || outKerosene.GetLiquidType() == CN_LiquidTypes.KEROSENE));

        if (!canOutputGasoline && !canOutputKerosene)
        {
            Server_StopProcess();
            return;
        }

        float oilVolumeConsumed = 200.0;
        if (inputCanister.GetQuantity() < oilVolumeConsumed)
            oilVolumeConsumed = inputCanister.GetQuantity();

        bool bSuccess = false;

        if (canOutputGasoline)
        {
            if (outGasoline.GetQuantity() == 0)
                outGasoline.SetLiquidType(8192); 
            
            outGasoline.AddQuantity(oilVolumeConsumed * 0.4);
            bSuccess = true;
        }

        if (canOutputKerosene)
        {
            if (outKerosene.GetQuantity() == 0)
                outKerosene.SetLiquidType(CN_LiquidTypes.KEROSENE); 
            
            outKerosene.AddQuantity(oilVolumeConsumed * 0.3);
            bSuccess = true;
        }

        // Списываем сырье (Правило №6)
        if (bSuccess)
        {
            inputCanister.AddQuantity(-oilVolumeConsumed);
        }

        if (inputCanister.GetQuantity() <= 0)
        {
            Server_StopProcess();
        }
    }
    #endif
}
