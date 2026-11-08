#pragma once

#include "CoreMinimal.h"
#include "BBBEquipmentAnimationFacts.generated.h"

/** 装备动画图读取的只读表现事实 */
USTRUCT(BlueprintType)
struct ABBB_EVAC_API FBBBEquipmentAnimationFacts
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly)
    FVector LeftHandTargetHandRSpace = FVector::ZeroVector;

    /** 左手握持目标相对角色 hand_r 骨骼的旋转 */
    UPROPERTY(BlueprintReadOnly)
    FRotator LeftHandTargetHandRSpaceRotation = FRotator::ZeroRotator;

    UPROPERTY(BlueprintReadOnly)
    bool bHasLeftHandTarget = false;

};
