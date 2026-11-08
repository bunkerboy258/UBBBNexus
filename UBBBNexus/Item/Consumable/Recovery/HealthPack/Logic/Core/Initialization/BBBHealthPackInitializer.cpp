#include "BBBWork/UBBBNexus/Item/Consumable/Recovery/HealthPack/Logic/Core/Initialization/BBBHealthPackInitializer.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Recovery/HealthPack/BBBHealthPackConsumable.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Recovery/HealthPack/Config/BBBHealthPackDefinition.h"
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
bool FBBBHealthPackInitializer::InitializeInstance(UBBBConsumable &Item) const
{
    auto &Concrete = *CastChecked<UBBBHealthPackConsumable>(&Item);
    const auto *Definition = Cast<UBBBHealthPackDefinition>(Item.GetDefinition());
    if (!Definition || !Definition->UseMontage || Definition->RecoveryFraction <= 0.0f ||
        Item.GetCharacter()->GetHealth() >= Item.GetCharacter()->GetCharacterConfig().MaximumHealth)
    {
        return false;
    }
    Concrete.ParseSystem.Initialize(Concrete.RuntimeData);
    Concrete.UseSystem.Initialize(Concrete.RuntimeData, *Definition, *Item.GetCharacter());
    return true;
}
