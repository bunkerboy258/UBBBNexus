#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/System/AnimationSystem/State/BBBEquipmentAnimationFacts.h"
#include "BBBEquipmentAnimationState.generated.h"

class FBBBEquipmentAnimationFactProcessor;

/** 动画系统持有的单帧事实 */
USTRUCT()
struct FBBBEquipmentAnimationState
{
    GENERATED_BODY()

private:
    friend class FBBBEquipmentAnimationFactProcessor;

    UPROPERTY()
    FBBBEquipmentAnimationFacts Facts;
};
