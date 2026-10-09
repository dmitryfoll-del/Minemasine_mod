modded class InspectMenuNew
{
    override static void UpdateItemInfoLiquidType(Widget root_widget, EntityAI item)
    {
        super.UpdateItemInfoLiquidType(root_widget, item);

        if (item)
        {
            int liquid_type = item.GetLiquidType(); 
            
            if (liquid_type == 8388608) // Керосин
            {
                WidgetTrySetText(root_widget, "ItemLiquidTypeWidget", "Керосин", Colors.COLOR_LIQUID);
            }
            else if (liquid_type == 16777216) // Сырая нефть
            {
                WidgetTrySetText(root_widget, "ItemLiquidTypeWidget", "Сырая нефть", Colors.COLOR_LIQUID);
            }
        }
    }
}
