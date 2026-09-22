#include "BBBWork/UBBBNexus/Customization/UI/BBBCharacterCustomizationWidget.h"
#include "BBBWork/UBBBNexus/Customization/BBBCharacterCustomizationController.h"
#include "InputCoreTypes.h"

FReply UBBBCharacterCustomizationWidget::NativeOnKeyDown(const FGeometry &Geometry, const FKeyEvent &KeyEvent)
{
    if (Customization && (KeyEvent.GetKey() == EKeys::Escape || KeyEvent.GetKey() == EKeys::F6))
    {
        Customization->Close();
        return FReply::Handled();
    }
    return Super::NativeOnKeyDown(Geometry, KeyEvent);
}
