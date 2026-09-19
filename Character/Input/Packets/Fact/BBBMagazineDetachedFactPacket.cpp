#include "BBBWork/UBBBNexus/Character/Input/Packets/Fact/BBBMagazineDetachedFactPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/Packets/Fact/BBBEquipmentActionFact.h"

bool FBBBMagazineDetachedFactPacket::IsValid() const
{
    return Sequence > 0;
}

bool FBBBMagazineDetachedFactPacket::CanApply(const FBBBCharacterPacketContext &Context) const
{
    return true;
}

void FBBBMagazineDetachedFactPacket::Apply(FBBBCharacterPacketContext &Context) const
{
    if (Context.Operation.IsRestoreMode())
    {
        Context.Commands.SubmitRestoredAction(ToFact());
    }

    Context.Events.AddAction(ToFact());
    Context.Operation.TrackMagazineDetached(Sequence, EquipmentId, Context.Equipment.GetActiveEquipmentId());
}

FBBBEquipmentActionFact FBBBMagazineDetachedFactPacket::ToFact() const
{
    return FBBBEquipmentActionFact{PacketId, EquipmentId, Sequence, LoadedAmmo};
}
