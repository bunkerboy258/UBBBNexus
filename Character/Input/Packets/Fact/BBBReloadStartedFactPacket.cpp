#include "BBBWork/UBBBNexus/Character/Input/Packets/Fact/BBBReloadStartedFactPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/Packets/Fact/BBBEquipmentActionFact.h"

bool FBBBReloadStartedFactPacket::IsValid() const
{
    return Sequence > 0;
}

bool FBBBReloadStartedFactPacket::CanExecute(const FBBBCharacterPacketContext &Context) const
{
    return true;
}

void FBBBReloadStartedFactPacket::Execute(FBBBCharacterPacketContext &Context) const
{
    if (Context.Operation.IsRestoreMode())
    {
        Context.Commands.SubmitRestoredAction(ToFact());
    }

    Context.Events.AddAction(ToFact());
    Context.Operation.TrackReloadStarted(
        Sequence,
        EquipmentId,
        Context.Equipment.GetActiveMainHandInstance(),
        Context.Equipment.GetActiveEquipmentId());
}

FBBBEquipmentActionFact FBBBReloadStartedFactPacket::ToFact() const
{
    return FBBBEquipmentActionFact{PacketId, EquipmentId, Sequence, LoadedAmmo};
}
