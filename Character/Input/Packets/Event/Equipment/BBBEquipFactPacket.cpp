#include "BBBWork/UBBBNexus/Character/Input/Packets/Event/Equipment/BBBEquipFactPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/Packets/Event/Equipment/BBBEquipmentActionFact.h"

bool FBBBEquipFactPacket::IsValid() const
{
    return Sequence > 0;
}

bool FBBBEquipFactPacket::CanApply(const FBBBCharacterInputContext &Context) const
{
    return true;
}

void FBBBEquipFactPacket::Apply(FBBBCharacterInputContext &Context) const
{
    Context.Commands.PendingFacts.Add(ToFact());
    Context.Events.ActionEvents.Add(ToFact());
}

FBBBEquipmentActionFact FBBBEquipFactPacket::ToFact() const
{
    return FBBBEquipmentActionFact{
        EBBBEquipmentActionType::Equip,
        EquipmentId,
        Sequence,
        LoadedAmmo};
}
