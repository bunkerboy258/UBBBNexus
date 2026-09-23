#include "BBBWork/UBBBNexus/Character/Input/Packets/Event/Equipment/BBBFireFactPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/Packets/Event/Equipment/BBBEquipmentActionFact.h"

bool FBBBFireFactPacket::IsValid() const
{
    return Sequence > 0;
}

bool FBBBFireFactPacket::CanApply(const FBBBCharacterInputContext &Context) const
{
    return true;
}

void FBBBFireFactPacket::Apply(FBBBCharacterInputContext &Context) const
{
    Context.Commands.PendingFacts.Add(ToFact());
    Context.Events.ActionEvents.Add(ToFact());
}

FBBBEquipmentActionFact FBBBFireFactPacket::ToFact() const
{
    return FBBBEquipmentActionFact{
        EBBBEquipmentActionType::Fire,
        EquipmentId,
        Sequence,
        LoadedAmmo};
}
