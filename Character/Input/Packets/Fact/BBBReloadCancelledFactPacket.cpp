#include "BBBWork/UBBBNexus/Character/Input/Packets/Fact/BBBReloadCancelledFactPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/Packets/Fact/BBBEquipmentActionFact.h"

bool FBBBReloadCancelledFactPacket::IsValid() const
{
    return Sequence > 0;
}

bool FBBBReloadCancelledFactPacket::CanExecute(const FBBBCharacterPacketContext &Context) const
{
    return true;
}

void FBBBReloadCancelledFactPacket::Execute(FBBBCharacterPacketContext &Context) const
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
