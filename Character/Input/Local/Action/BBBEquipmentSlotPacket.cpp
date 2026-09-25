#include "BBBWork/UBBBNexus/Character/Input/Local/Action/BBBEquipmentSlotPacket.h"

#include "BBBWork/UBBBNexus/Character/Logic/System/EquipmentSystem/DomainData/States/BBBCharacterEquipmentInventoryState.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/EquipmentSystem/DomainData/States/BBBCharacterEquipmentSelectionState.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/ParseSystem/DomainData/Context/BBBCharacterInputContext.h"

bool FBBBEquipmentSlotPacket::IsValid() const
{
    return Slot >= INDEX_NONE;
}

bool FBBBEquipmentSlotPacket::CanApply(const FBBBCharacterInputContext &Context) const
{
    return Slot == INDEX_NONE || Context.EquipmentInventory.ItemBarSlots.IsValidIndex(Slot);
}

void FBBBEquipmentSlotPacket::Apply(FBBBCharacterInputContext &Context) const
{
    Context.EquipmentSelection.PendingSlot = Slot;
}
