
#include "BBBWork/UBBBNexus/Character/System/ItemSystem/Processors/BBBCharacterWeaponSwitchProcessor.h"
#include "BBBWork/UBBBNexus/Character/System/ItemSystem/Definition/BBBCharacterItemState.h"
#include "BBBWork/UBBBNexus/Item/Equipment/BBBEquipmentActor.h"

void FBBBCharacterWeaponSwitchProcessor::Update(FBBBCharacterEquipmentState &Equipment) const
{

    if (Equipment.EquippedItemActor)
    {
        Equipment.EquippedItemActor->Destroy();
    }

    Equipment.EquippedItemActor = nullptr;

    Equipment.ActiveMainHandItem = FBBBItemInstance();
    Equipment.ActiveMainHandDefinition = nullptr;
    Equipment.ActiveTargetMode = EBBBEquipmentTargetMode::None;
    Equipment.ActiveMirrorHandle = NAME_None;

    Equipment.bIsEquipping = false;
    Equipment.EquipEndTime = 0.0f;
}
