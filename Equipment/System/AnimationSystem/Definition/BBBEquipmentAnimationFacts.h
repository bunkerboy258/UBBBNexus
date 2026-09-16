#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Equipment/Definition/BBBEquipmentActionType.h"
#include "BBBEquipmentAnimationFacts.generated.h"

class UAnimMontage;

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
    bool bHasMainHandEquipment = false;

    UPROPERTY(BlueprintReadOnly)
    int32 FireSequence = 0;

    UPROPERTY(BlueprintReadOnly)
    float LastFireTimeSeconds = -1000.0f;

    UPROPERTY(BlueprintReadOnly)
    int32 ReloadSequence = 0;

    UPROPERTY(BlueprintReadOnly)
    bool bIsReloading = false;

    UPROPERTY(BlueprintReadOnly)
    float ReloadStartTimeSeconds = 0.0f;

    UPROPERTY(BlueprintReadOnly)
    float ReloadDurationSeconds = 0.0f;

    UPROPERTY(BlueprintReadOnly)
    float CurrentWorldTimeSeconds = 0.0f;

    UPROPERTY(BlueprintReadOnly)
    EBBBEquipmentActionType EquipmentActionType = EBBBEquipmentActionType::None;

    UPROPERTY(BlueprintReadOnly)
    int32 EquipmentActionSequence = INDEX_NONE;

    UPROPERTY(BlueprintReadOnly)
    float EquipmentActionDuration = 0.0f;

    UPROPERTY(BlueprintReadOnly)
    TObjectPtr<UAnimMontage> EquipmentActionMontage = nullptr;

    UPROPERTY(BlueprintReadOnly)
    float EquipmentActionPlayRate = 1.0f;
};
