
#include "BBBWork/UBBBNexus/Character/System/ItemSystem/Processors/BBBCharacterEquipmentTransitionProcessor.h"
#include "BBBWork/UBBBNexus/Character/System/ItemSystem/Definition/BBBCharacterItemState.h"

void FBBBCharacterEquipmentTransitionProcessor::Update(
    float WorldTimeSeconds,
    FBBBCharacterEquipmentState &Equipment) const
{

    if (!Equipment.bIsEquipping)
    {
        return;
    }

    if (WorldTimeSeconds < Equipment.EquipEndTime)
    {
        return;
    }

    Equipment.bIsEquipping = false;
    Equipment.EquipEndTime = 0.0f;
}
