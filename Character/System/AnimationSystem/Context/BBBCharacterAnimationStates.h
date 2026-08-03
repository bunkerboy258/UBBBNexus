
#pragma once
#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/System/LocomotionSystem/Context/BBBLocomotionStates.h"
#include "BBBCharacterAnimationStates.generated.h"

USTRUCT(BlueprintType)
//角色动画状态封装
struct FBBBCharacterAnimationState
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly)
    EBBBLocomotionState LocomotionState = EBBBLocomotionState::Idle;

    UPROPERTY(BlueprintReadOnly)
    FVector2D MoveInput = FVector2D::ZeroVector;

    UPROPERTY(BlueprintReadOnly)
    FVector2D SmoothedMoveInput = FVector2D::ZeroVector;

    UPROPERTY(BlueprintReadOnly)
    bool bWantsFire = false;

    UPROPERTY(BlueprintReadOnly)
    bool bWantsAim = false;

    UPROPERTY(BlueprintReadOnly)
    float AimIKAlpha = 0.0f;

    UPROPERTY(BlueprintReadOnly)
    float LeftHandIKAlpha = 0.0f;

    UPROPERTY(BlueprintReadOnly)
    bool bIsTurningInPlaceLeft = false;

    UPROPERTY(BlueprintReadOnly)
    bool bIsTurningInPlaceRight = false;

    UPROPERTY(BlueprintReadOnly)
    FVector AimTargetComponentSpace = FVector::ZeroVector;

    UPROPERTY(BlueprintReadOnly)
    FTransform AimSourceLocalTransform = FTransform::Identity;

    UPROPERTY(BlueprintReadOnly)
    bool bHasValidAimTarget = false;

    UPROPERTY(BlueprintReadOnly)
    bool bHasValidAimSource = false;

    UPROPERTY(BlueprintReadOnly)
    float AimIKDistanceAlpha = 1.0f;

    UPROPERTY(BlueprintReadOnly)
    FTransform LeftHandTargetRightHandSocketSpace = FTransform::Identity;

    UPROPERTY(BlueprintReadOnly)
    //关闭boolHasValidLeftHand目标
    bool bHasValidLeftHandTarget = false;
};

//保存瞄准表现处理器的跨帧平滑状态
struct FBBBAimPresentationRuntimeState
{
    FVector SmoothedAimTargetComponentSpace = FVector::ZeroVector;

    FVector AimTargetSmoothVelocity = FVector::ZeroVector;

    float SmoothedAimIKAlpha = 0.0f;

    bool bHasSmoothedAimTarget = false;
};
