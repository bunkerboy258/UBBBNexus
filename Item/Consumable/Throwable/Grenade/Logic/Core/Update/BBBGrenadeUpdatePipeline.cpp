#include "BBBWork/UBBBNexus/Item/Consumable/Throwable/Grenade/Logic/Core/Update/BBBGrenadeUpdatePipeline.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Throwable/Grenade/BBBGrenadeConsumable.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Throwable/Grenade/Config/BBBGrenadeDefinition.h"
void FBBBGrenadeUpdatePipeline::UpdateInstance(UBBBConsumable &Item) const
{
    auto &Concrete = *CastChecked<UBBBGrenadeConsumable>(&Item);
    Concrete.ParseSystem.Update();
    Concrete.UseSystem.Update();
}
