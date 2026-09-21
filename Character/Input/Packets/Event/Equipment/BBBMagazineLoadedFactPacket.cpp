#include "BBBWork/UBBBNexus/Character/Input/Packets/Event/Equipment/BBBMagazineLoadedFactPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/Packets/Event/Equipment/BBBEquipmentActionFact.h"

bool FBBBMagazineLoadedFactPacket::IsValid() const
{
    return Sequence > 0;
}

bool FBBBMagazineLoadedFactPacket::CanApply(const FBBBCharacterInputContext &Context) const
{
    return true;
}

void FBBBMagazineLoadedFactPacket::Apply(FBBBCharacterInputContext &Context) const
{
    Context.Commands.SubmitFact(ToFact());
    Context.Events.AddAction(ToFact());
    Context.Operation.TrackReloadFinished(Sequence, EquipmentId, Context.Equipment.ActiveEquipmentId, false);
}

FBBBEquipmentActionFact FBBBMagazineLoadedFactPacket::ToFact() const
{
    return FBBBEquipmentActionFact{PacketId, EquipmentId, Sequence, LoadedAmmo};
}
