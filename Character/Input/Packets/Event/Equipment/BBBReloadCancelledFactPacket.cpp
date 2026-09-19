#include "BBBWork/UBBBNexus/Character/Input/Packets/Event/Equipment/BBBReloadCancelledFactPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/Packets/Event/Equipment/BBBEquipmentActionFact.h"

bool FBBBReloadCancelledFactPacket::IsValid() const
{
    return Sequence > 0;
}

bool FBBBReloadCancelledFactPacket::CanApply(const FBBBCharacterPacketContext &Context) const
{
    return true;
}

void FBBBReloadCancelledFactPacket::Apply(FBBBCharacterPacketContext &Context) const
{
    if (Context.Operation.IsRestoreMode())
    {
        Context.Commands.SubmitRestoredAction(ToFact());
    }

    Context.Events.AddAction(ToFact());
    Context.Operation.TrackReloadFinished(Sequence, EquipmentId, Context.Equipment.GetActiveEquipmentId(), true);
}

FBBBEquipmentActionFact FBBBReloadCancelledFactPacket::ToFact() const
{
    return FBBBEquipmentActionFact{PacketId, EquipmentId, Sequence, LoadedAmmo};
}
