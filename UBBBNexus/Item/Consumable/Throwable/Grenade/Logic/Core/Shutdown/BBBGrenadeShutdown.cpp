#include "BBBWork/UBBBNexus/Item/Consumable/Throwable/Grenade/Logic/Core/Shutdown/BBBGrenadeShutdown.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Throwable/Grenade/BBBGrenadeConsumable.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Throwable/Grenade/Config/BBBGrenadeDefinition.h"
void FBBBGrenadeShutdown::ShutdownInstance(UBBBConsumable &Item) const
{
    auto &Concrete = *CastChecked<UBBBGrenadeConsumable>(&Item);
    Concrete.ParseSystem.Shutdown();
    Concrete.UseSystem.Shutdown();
}
