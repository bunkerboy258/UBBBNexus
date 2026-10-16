#include "BBBWork/UBBBNexus/Item/Consumable/Recovery/HealthPack/Logic/Core/Shutdown/BBBHealthPackShutdown.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Recovery/HealthPack/BBBHealthPackConsumable.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Recovery/HealthPack/Config/BBBHealthPackDefinition.h"
void FBBBHealthPackShutdown::ShutdownInstance(UBBBConsumable &Item) const
{
    auto &Concrete = *CastChecked<UBBBHealthPackConsumable>(&Item);
    Concrete.ParseSystem.Shutdown();
    Concrete.UseSystem.Shutdown();
}
