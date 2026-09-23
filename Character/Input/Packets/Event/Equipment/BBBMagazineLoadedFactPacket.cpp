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
    Context.Commands.PendingFacts.Add(ToFact());
    Context.Events.ActionEvents.Add(ToFact());
}

FBBBEquipmentActionFact FBBBMagazineLoadedFactPacket::ToFact() const
{
    return FBBBEquipmentActionFact{
        EBBBEquipmentActionType::MagazineLoaded,
        EquipmentId,
        Sequence,
        LoadedAmmo};
}
