#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/System/LocomotionSystem/Definition/BBBCharacterLocomotionRuntimeData.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "BBBCharacterAnimationStates.generated.h"

/** 游戏线程提交给动画实例的只读角色事实 */
USTRUCT(BlueprintType)
struct FBBBCharacterAnimationFacts
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly)
    FVector ActorLocation = FVector::ZeroVector;

    UPROPERTY(BlueprintReadOnly)
    FRotator ActorRotation = FRotator::ZeroRotator;

    UPROPERTY(BlueprintReadOnly)
    FVector Velocity = FVector::ZeroVector;

    UPROPERTY(BlueprintReadOnly)
    FVector LastUpdateVelocity = FVector::ZeroVector;

    UPROPERTY(BlueprintReadOnly)
    FVector Acceleration = FVector::ZeroVector;

    UPROPERTY(BlueprintReadOnly)
    FVector AimTargetComponentSpace = FVector::ZeroVector;

    UPROPERTY(BlueprintReadOnly)
    FTransform AimSourceLocalTransform = FTransform::Identity;

    /** 左手 IK 目标相对右手骨骼的变换 */
    UPROPERTY(BlueprintReadOnly)
    FTransform LeftHandIKTargetRightHandBoneSpace = FTransform::Identity;

    UPROPERTY(BlueprintReadOnly)
    EBBBCharacterGait Gait = EBBBCharacterGait::Run;

    UPROPERTY(BlueprintReadOnly)
    TEnumAsByte<EMovementMode> MovementMode = MOVE_None;

    UPROPERTY(BlueprintReadOnly)
    float GroundFriction = 0.0f;

    UPROPERTY(BlueprintReadOnly)
    float BrakingFriction = 0.0f;

    UPROPERTY(BlueprintReadOnly)
    float BrakingFrictionFactor = 0.0f;

    UPROPERTY(BlueprintReadOnly)
    float BrakingDecelerationWalking = 0.0f;

    UPROPERTY(BlueprintReadOnly)
    float GravityZ = 0.0f;

    UPROPERTY(BlueprintReadOnly)
    float GroundDistance = -1.0f;

    UPROPERTY(BlueprintReadOnly)
    float AimIntentAlpha = 0.0f;

    UPROPERTY(BlueprintReadOnly)
    float AimIKAlpha = 0.0f;

    UPROPERTY(BlueprintReadOnly)
    float TimeSinceLastFire = BIG_NUMBER;

    UPROPERTY(BlueprintReadOnly)
    bool bUseSeparateBrakingFriction = false;

    UPROPERTY(BlueprintReadOnly)
    bool bIsMovingOnGround = false;

    UPROPERTY(BlueprintReadOnly)
    bool bIsCrouching = false;

    UPROPERTY(BlueprintReadOnly)
    bool bIsAiming = false;

    UPROPERTY(BlueprintReadOnly)
    bool bHasMainHandEquipment = false;

    UPROPERTY(BlueprintReadOnly)
    bool bIsReloading = false;

    UPROPERTY(BlueprintReadOnly)
    bool bHasValidAimTarget = false;

    UPROPERTY(BlueprintReadOnly)
    bool bHasValidAimSource = false;

    /** 左手 IK 目标是否有效 */
    UPROPERTY(BlueprintReadOnly)
    bool bHasValidLeftHandIKTarget = false;

};
