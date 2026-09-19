#include "BBBWork/UBBBNexus/Character/Input/Packets/Fact/BBBMagazineLoadedFactPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/Packets/Fact/BBBEquipmentActionFact.h"

bool FBBBMagazineLoadedFactPacket::IsValid() const
{
    return Sequence > 0;
}

bool FBBBMagazineLoadedFactPacket::CanExecute(const FBBBCharacterPacketContext &Context) const
{
    return true;
}

void FBBBMagazineLoadedFactPacket::Execute(FBBBCharacterPacketContext &Context) const
{
    if (Context.Operation.IsRestoreMode())
    {
        Context.Commands.SubmitRestoredAction(ToFact());
    }

    Context.Events.AddAction(ToFact());
    Context.Operation.TrackReloadFinished(Sequence, EquipmentId, Context.Equipment.GetActiveEquipmentId(), false);
}

FBBBEquipmentActionFact FBBBMagazineLoadedFactPacket::ToFact() const
{
    return FBBBEquipmentActionFact{PacketId, EquipmentId, Sequence, LoadedAmmo};
}
