#include "BBBWork/UBBBNexus/Character/System/ItemSystem/Definition/BBBCharacterItemState.h"

#include "BBBWork/UBBBNexus/Item/Base/Equipment/BBBEquipmentInstance.h"

bool FBBBCharacterEquipmentState::IsEquipping() const
{
    return ActiveMainHandInstance && ActiveMainHandInstance->IsEquipping();
}
