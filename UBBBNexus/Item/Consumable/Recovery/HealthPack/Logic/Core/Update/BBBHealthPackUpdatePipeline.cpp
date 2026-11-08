#include "BBBWork/UBBBNexus/Item/Consumable/Recovery/HealthPack/Logic/Core/Update/BBBHealthPackUpdatePipeline.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Recovery/HealthPack/BBBHealthPackConsumable.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Recovery/HealthPack/Config/BBBHealthPackDefinition.h"
void FBBBHealthPackUpdatePipeline::UpdateInstance(UBBBConsumable &Item) const
{
    auto &Concrete = *CastChecked<UBBBHealthPackConsumable>(&Item);
    Concrete.ParseSystem.Update();
    Concrete.UseSystem.Update();
}
