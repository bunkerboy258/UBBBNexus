#pragma once

#include "CoreMinimal.h"
#include "BBBEquipmentAnimationFacts.generated.h"

/** 装备动画图读取的只读表现事实 */
USTRUCT(BlueprintType)
struct ABBB_EVAC_API FBBBEquipmentAnimationFacts
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly)
    FTransform AimSourceLocalTransform = FTransform::Identity;

    UPROPERTY(BlueprintReadOnly)
    bool bHasValidAimSource = false;

    UPROPERTY(BlueprintReadOnly)
    FVector LeftHandTargetHandRSpace = FVector::ZeroVector;

    UPROPERTY(BlueprintReadOnly)
    bool bHasLeftHandTarget = false;

};
