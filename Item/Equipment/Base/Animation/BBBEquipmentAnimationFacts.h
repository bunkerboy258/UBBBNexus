#pragma once

#include "CoreMinimal.h"
#include "BBBEquipmentAnimationFacts.generated.h"

/** 装备动画图读取的只读表现事实 */
USTRUCT(BlueprintType)
struct ABBB_EVAC_API FBBBEquipmentAnimationFacts
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly, meta = (DisplayName = "右手空间左手目标"))
    FVector LeftHandTargetHandRSpace = FVector::ZeroVector;

    /** 左手握持目标相对角色 hand_r 骨骼的旋转 */
    UPROPERTY(BlueprintReadOnly, meta = (DisplayName = "右手空间左手目标旋转"))
    FRotator LeftHandTargetHandRSpaceRotation = FRotator::ZeroRotator;

    UPROPERTY(BlueprintReadOnly, meta = (DisplayName = "存在左手目标"))
    bool bHasLeftHandTarget = false;

};
