#include "BBBWork/UBBBNexus/Character/Input/Packets/Event/Equipment/BBBMagazineDetachedFactPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/Packets/Event/Equipment/BBBEquipmentActionFact.h"

bool FBBBMagazineDetachedFactPacket::IsValid() const
{
    return Sequence > 0;
}

bool FBBBMagazineDetachedFactPacket::CanApply(const FBBBCharacterInputContext &Context) const
{
    return true;
}

void FBBBMagazineDetachedFactPacket::Apply(FBBBCharacterInputContext &Context) const
{
    Context.Commands.PendingFacts.Add(ToFact());
    Context.Events.ActionEvents.Add(ToFact());
}

FBBBEquipmentActionFact FBBBMagazineDetachedFactPacket::ToFact() const
{
    return FBBBEquipmentActionFact{
        EBBBEquipmentActionType::MagazineDetached,
        EquipmentId,
        Sequence,
        LoadedAmmo};
}
