#include "BBBWork/UBBBNexus/Character/Input/Packets/Event/Equipment/BBBMagazineLoadedFactPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/Packets/Event/Equipment/BBBEquipmentActionFact.h"

bool FBBBMagazineLoadedFactPacket::IsValid() const
{
    return Sequence > 0;
}

bool FBBBMagazineLoadedFactPacket::CanApply(const FBBBCharacterPacketContext &Context) const
{
    return true;
}

void FBBBMagazineLoadedFactPacket::Apply(FBBBCharacterPacketContext &Context) const
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
