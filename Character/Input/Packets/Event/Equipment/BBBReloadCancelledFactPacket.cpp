#include "BBBWork/UBBBNexus/Character/Input/Packets/Event/Equipment/BBBReloadCancelledFactPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/Packets/Event/Equipment/BBBEquipmentActionFact.h"

bool FBBBReloadCancelledFactPacket::IsValid() const
{
    return Sequence > 0;
}

bool FBBBReloadCancelledFactPacket::CanApply(const FBBBCharacterInputContext &Context) const
{
    return true;
}

void FBBBReloadCancelledFactPacket::Apply(FBBBCharacterInputContext &Context) const
{
    Context.Commands.PendingFacts.Add(ToFact());
    Context.Events.ActionEvents.Add(ToFact());
}

FBBBEquipmentActionFact FBBBReloadCancelledFactPacket::ToFact() const
{
    return FBBBEquipmentActionFact{
        EBBBEquipmentActionType::ReloadCancelled,
        EquipmentId,
        Sequence,
        LoadedAmmo};
}
