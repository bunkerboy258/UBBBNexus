#include "BBBWork/UBBBNexus/Character/Input/Packets/Event/Equipment/BBBReloadStartedFactPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/BBBCharacterOperation.h"
#include "BBBWork/UBBBNexus/Character/Input/Packets/Event/Equipment/BBBEquipmentActionFact.h"

bool FBBBReloadStartedFactPacket::IsValid() const
{
    return Sequence > 0;
}

bool FBBBReloadStartedFactPacket::CanApply(const FBBBCharacterInputContext &Context) const
{
    return true;
}

void FBBBReloadStartedFactPacket::Apply(FBBBCharacterInputContext &Context) const
{
    Context.Commands.PendingFacts.Add(ToFact());
    Context.Events.ActionEvents.Add(ToFact());
    BBBCharacterOperation::TrackReloadStarted(
        Context.Operation,
        Sequence,
        EquipmentId,
        Context.Equipment.ActiveMainHandInstance,
        Context.Equipment.ActiveEquipmentId);
}

FBBBEquipmentActionFact FBBBReloadStartedFactPacket::ToFact() const
{
    return FBBBEquipmentActionFact{
        EBBBEquipmentActionType::ReloadStarted,
        EquipmentId,
        Sequence,
        LoadedAmmo};
}
