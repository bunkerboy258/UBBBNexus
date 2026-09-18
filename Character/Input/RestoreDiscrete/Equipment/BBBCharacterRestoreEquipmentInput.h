#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Input/Events/BBBEquipmentActionEvent.h"
#include "BBBCharacterRestoreEquipmentInput.generated.h"

/** 网络还原装备选择和装备事实 */
USTRUCT()
struct FBBBCharacterRestoreEquipmentInput
{
    GENERATED_BODY()

    UPROPERTY()
    FName EquipmentHandle = NAME_None;

    UPROPERTY()
    FBBBEquipmentActionEvent ActionEvent;
};
