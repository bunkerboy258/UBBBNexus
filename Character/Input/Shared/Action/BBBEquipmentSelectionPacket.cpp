#include "BBBWork/UBBBNexus/Character/Input/Shared/Action/BBBEquipmentSelectionPacket.h"

#include "BBBWork/UBBBNexus/Character/Logic/System/EquipmentSystem/DomainData/States/BBBCharacterEquipmentSelectionState.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/ParseSystem/DomainData/Context/BBBCharacterInputContext.h"

bool FBBBEquipmentSelectionPacket::IsValid() const
{
    return true;
}

bool FBBBEquipmentSelectionPacket::CanApply(const FBBBCharacterInputContext &Context) const
{
    return true;
}

void FBBBEquipmentSelectionPacket::Apply(FBBBCharacterInputContext &Context) const
{
    Context.EquipmentSelection.PendingEquipmentId = EquipmentId;
    Context.EquipmentSelection.bHasEquipmentRequest = true;
}
