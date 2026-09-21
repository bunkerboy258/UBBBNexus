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
    Context.Commands.SubmitFact(ToFact());
    Context.Events.AddAction(ToFact());
    Context.Operation.TrackReloadFinished(Sequence, EquipmentId, Context.Equipment.ActiveEquipmentId, true);
}

FBBBEquipmentActionFact FBBBReloadCancelledFactPacket::ToFact() const
{
    return FBBBEquipmentActionFact{PacketId, EquipmentId, Sequence, LoadedAmmo};
}
