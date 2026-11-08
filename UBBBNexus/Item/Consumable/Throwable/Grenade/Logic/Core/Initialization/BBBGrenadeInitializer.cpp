#include "BBBWork/UBBBNexus/Item/Consumable/Throwable/Grenade/Logic/Core/Initialization/BBBGrenadeInitializer.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Throwable/Grenade/BBBGrenadeConsumable.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Throwable/Grenade/Config/BBBGrenadeDefinition.h"
bool FBBBGrenadeInitializer::InitializeInstance(UBBBConsumable &Item) const
{
    auto &Concrete = *CastChecked<UBBBGrenadeConsumable>(&Item);
    const auto *Definition = Cast<UBBBGrenadeDefinition>(Item.GetDefinition());
    if (!Definition)
    {
        return false;
    }
    Concrete.ParseSystem.Initialize(Concrete.RuntimeData);
    Concrete.UseSystem.Initialize(Concrete.RuntimeData, *Definition, *Item.GetCharacter());
    return true;
}
