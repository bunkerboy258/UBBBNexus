#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Runtime/Controller/LocomotionController/DomainData/States/BBBCharacterLocomotionState.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "BBBCharacterAnimationFacts.generated.h"

/** 游戏线程提交给动画实例的只读角色事实 */
USTRUCT(BlueprintType)
struct FBBBCharacterAnimationFacts
{
    GENERATED_BODY()

    /** 角色世界位置 */
    UPROPERTY(BlueprintReadOnly)
    FVector ActorLocation = FVector::ZeroVector;

    /** 角色世界旋转 */
    UPROPERTY(BlueprintReadOnly)
    FRotator ActorRotation = FRotator::ZeroRotator;

    /** 角色当前速度 */
    UPROPERTY(BlueprintReadOnly)
    FVector Velocity = FVector::ZeroVector;

    /** 移动组件上次更新速度 */
    UPROPERTY(BlueprintReadOnly)
    FVector LastUpdateVelocity = FVector::ZeroVector;

    /** 移动组件当前加速度 */
    UPROPERTY(BlueprintReadOnly)
    FVector Acceleration = FVector::ZeroVector;

    /** 角色当前步态 */
    UPROPERTY(BlueprintReadOnly)
    EBBBCharacterGait Gait = EBBBCharacterGait::Run;

    /** 移动组件当前移动模式 */
    UPROPERTY(BlueprintReadOnly)
    TEnumAsByte<EMovementMode> MovementMode = MOVE_None;

    /** 地面摩擦力 */
    UPROPERTY(BlueprintReadOnly)
    float GroundFriction = 0.0f;

    /** 制动摩擦力 */
    UPROPERTY(BlueprintReadOnly)
    float BrakingFriction = 0.0f;

    /** 制动摩擦力倍率 */
    UPROPERTY(BlueprintReadOnly)
    float BrakingFrictionFactor = 0.0f;

    /** 行走制动减速度 */
    UPROPERTY(BlueprintReadOnly)
    float BrakingDecelerationWalking = 0.0f;

    /** 当前重力加速度 */
    UPROPERTY(BlueprintReadOnly)
    float GravityZ = 0.0f;

    /** 角色到地面的距离 */
    UPROPERTY(BlueprintReadOnly)
    float GroundDistance = -1.0f;

    /** 是否使用独立制动摩擦力 */
    UPROPERTY(BlueprintReadOnly)
    bool bUseSeparateBrakingFriction = false;

    /** 是否正在地面移动 */
    UPROPERTY(BlueprintReadOnly)
    bool bIsMovingOnGround = false;

    /** 是否正在蹲伏 */
    UPROPERTY(BlueprintReadOnly)
    bool bIsCrouching = false;

    /** 当前是否具有瞄准意图 */
    UPROPERTY(BlueprintReadOnly)
    bool bIsAiming = false;

    /** 平滑后的角色瞄准意图权重 */
    UPROPERTY(BlueprintReadOnly)
    float AimIntentAlpha = 0.0f;

    /** 角色计算的瞄准 IK 最终权重 */
    UPROPERTY(BlueprintReadOnly)
    float AimIKAlpha = 0.0f;

    /** 角色组件空间中的瞄准目标 */
    UPROPERTY(BlueprintReadOnly)
    FVector AimTargetComponentSpace = FVector::ZeroVector;

};
