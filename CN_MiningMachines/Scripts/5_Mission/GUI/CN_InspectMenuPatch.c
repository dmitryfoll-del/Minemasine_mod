modded class InspectMenuNew
{
    override static void UpdateItemInfoLiquidType(Widget root_widget, EntityAI item)
    {
        // Вызываем базовое обновление ванильного интерфейса
        super.UpdateItemInfoLiquidType(root_widget, item);

        if (item)
        {
            int liquid_type = item.GetLiquidType(); 
            Widget liquid_widget = root_widget.FindAnyWidget("ItemLiquidTypeWidget");
            
            if (liquid_widget)
            {
                TextWidget text_widget = TextWidget.Cast(liquid_widget);
                if (text_widget)
                {
                    // Проверяем по жестким числовым ID жидкостей
                    if (liquid_type == 8388608) // Керосин
                    {
                        // Устанавливаем текстовый токен и переводим его средствами движка
                        text_widget.SetText("#STR_CN_LIQUID_KEROSENE");
                        root_widget.TranslateString("#STR_CN_LIQUID_KEROSENE"); 
                    }
                    else if (liquid_type == 16777216) // Сырая нефть
                    {
                        text_widget.SetText("#STR_CN_LIQUID_CRUDE_OIL");
                        root_widget.TranslateString("#STR_CN_LIQUID_CRUDE_OIL");
                    }
                }
            }
        }
    }
}
