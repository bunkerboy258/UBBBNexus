#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/LifeSystem/DomainData/Definitions/BBBCharacterLifePhase.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "BBBWork/UBBBNexus/Equipment/Base/Config/BBBEquipmentType.h"
#include "BBBCharacterAnimationFactState.generated.h"

/** 游戏线程维护并提交给动画实例的角色动画事实状态 */
USTRUCT()
struct FBBBCharacterAnimationFactState final
{
    GENERATED_BODY()
    /** 当前生命阶段 */
    UPROPERTY(Transient)
    EBBBCharacterLifePhase LifePhase = EBBBCharacterLifePhase::Alive;

    /** 本次倒地入场的经过秒数 负值表示无需播放入场 */
    UPROPERTY(Transient)
    float DownedEntryElapsed = -1.0f;

    /** 实际持有装备的配置类别 空手时为无 */
    UPROPERTY(Transient)
    EBBBEquipmentType EquipmentType = EBBBEquipmentType::None;

    /** 本帧是否正在执行翻越 */
    UPROPERTY(Transient)
    bool bTraversing = false;

    /** 上次动画采集时全身槽位是否仍在播放 */
    UPROPERTY(Transient)
    bool bFullBodyPlaying = false;

    /** 角色世界位置 */
    UPROPERTY(Transient)
    FVector ActorLocation = FVector::ZeroVector;

    /** 角色世界旋转 */
    UPROPERTY(Transient)
    FRotator ActorRotation = FRotator::ZeroRotator;

    /** 角色当前速度 */
    UPROPERTY(Transient)
    FVector Velocity = FVector::ZeroVector;

    /** 移动组件上次更新速度 */
    UPROPERTY(Transient)
    FVector LastUpdateVelocity = FVector::ZeroVector;

    /** 移动组件当前加速度 */
    UPROPERTY(Transient)
    FVector Acceleration = FVector::ZeroVector;

    /** 角色当前是否跑步 */
    UPROPERTY(Transient)
    bool bIsRunning = false;

    /** 移动组件当前移动模式 */
    UPROPERTY(Transient)
    TEnumAsByte<EMovementMode> MovementMode = MOVE_None;

    /** 地面摩擦力 */
    UPROPERTY(Transient)
    float GroundFriction = 0.0f;

    /** 制动摩擦力 */
    UPROPERTY(Transient)
    float BrakingFriction = 0.0f;

    /** 制动摩擦力倍率 */
    UPROPERTY(Transient)
    float BrakingFrictionFactor = 0.0f;

    /** 行走制动减速度 */
    UPROPERTY(Transient)
    float BrakingDecelerationWalking = 0.0f;

    /** 当前重力加速度 */
    UPROPERTY(Transient)
    float GravityZ = 0.0f;

    /** 角色到地面的距离 */
    UPROPERTY(Transient)
    float GroundDistance = -1.0f;

    /** 是否使用独立制动摩擦力 */
    UPROPERTY(Transient)
    bool bUseSeparateBrakingFriction = false;

    /** 是否正在地面移动 */
    UPROPERTY(Transient)
    bool bIsMovingOnGround = false;

    /** 是否正在蹲伏 */
    UPROPERTY(Transient)
    bool bIsCrouching = false;

    /** 当前是否具有瞄准意图 */
    UPROPERTY(Transient)
    bool bIsAiming = false;

    /** 角色当前额外瞄准角度偏移 */
    UPROPERTY(Transient)
    FVector2D AimOffsetDegrees = FVector2D::ZeroVector;

    /** 角色组件空间中的瞄准目标 */
    UPROPERTY(Transient)
    FVector AimTargetComponentSpace = FVector::ZeroVector;

    /** 当前枪口在角色 hand_r 骨骼空间中的变换 获取失败时为单位变换 */
    UPROPERTY(Transient)
    FTransform MuzzleTransformHandRSpace = FTransform::Identity;

    /** 本帧是否成功采集有效枪口 获取失败时禁用瞄准求解 */
    UPROPERTY(Transient)
    bool bHasMuzzle = false;

};
