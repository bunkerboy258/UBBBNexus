#include "BBBWork/UBBBNexus/Item/Consumable/Instance/Grenade/Logic/System/ParseSystem/Processors/BBBGrenadeParseProcessor.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Instance/Grenade/Logic/System/ParseSystem/DomainData/Context/BBBGrenadeParseUpdateContext.h"

void FBBBGrenadeParseProcessor::Update(FBBBGrenadeParseUpdateContext &Context) const
{
    const auto Process = [&Context](auto &Slot)
    {
        if (!Slot.bActive)
        {
            return;
        }
        if (Slot.Data.IsValid() && Slot.Data.CanApply(Context))
        {
            Slot.Data.Apply(Context);
        }
        Slot.bActive = false;
        Slot.Data = {};
    };
    Process(Context.Slots.PrimaryLocalControl);
    Process(Context.Slots.EquipLocalControl);
    Process(Context.Slots.ReleaseLocalControl);
    Process(Context.Slots.FinishLocalControl);
    Process(Context.Slots.PermissionLocalControl);
    Process(Context.Slots.StateRemoteMessage);
    Process(Context.Slots.StateAuthorityFact);
}
