#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Input/Events/BBBEquipmentActionEvent.h"
#include "BBBCharacterDiscreteEquipmentInput.generated.h"

/** 装备切换和装备侧已执行事实 */
USTRUCT(BlueprintType)
struct FBBBCharacterDiscreteEquipmentInput
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 EquipSlot = INDEX_NONE;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FBBBEquipmentActionEvent ActionEvent;
};
