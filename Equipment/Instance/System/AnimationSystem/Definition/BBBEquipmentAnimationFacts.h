#pragma once

#include "CoreMinimal.h"
#include "BBBEquipmentAnimationFacts.generated.h"


/** 装备动画单帧事实快照 */
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

    UPROPERTY(BlueprintReadOnly)
    int32 FireSequence = 0;

    UPROPERTY(BlueprintReadOnly)
    float LastFireTimeSeconds = -1000.0f;

    UPROPERTY(BlueprintReadOnly)
    int32 LoadedAmmo = 0;

    UPROPERTY(BlueprintReadOnly)
    int32 AmmoCapacity = 0;

    UPROPERTY(BlueprintReadOnly)
    float CurrentWorldTimeSeconds = 0.0f;

};
