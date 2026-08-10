
#pragma once
#include "CoreMinimal.h"
#include "BBBCharacterAnimationStates.generated.h"

class UAnimSequence;

USTRUCT(BlueprintType)
//角色动画状态封装
struct FBBBCharacterAnimationState
{
    GENERATED_BODY()

    /** 是否存在实际水平移动 */
    UPROPERTY(BlueprintReadOnly)
    bool bIsMoving = false;

    /** 是否接触地面 */
    UPROPERTY(BlueprintReadOnly)
    bool bIsGrounded = true;

    /** 是否实际处于蹲伏状态 */
    UPROPERTY(BlueprintReadOnly)
    bool bIsCrouching = false;

    /** 主手是否持有装备 */
    UPROPERTY(BlueprintReadOnly)
    bool bHasMainHandEquipment = false;

    /** 当前装备提供的普通握持上半身动画 */
    UPROPERTY(BlueprintReadOnly)
    TObjectPtr<UAnimSequence> HoldingUpperBodyAnimation = nullptr;

    /** 当前装备提供的瞄准上半身动画 */
    UPROPERTY(BlueprintReadOnly)
    TObjectPtr<UAnimSequence> AimingUpperBodyAnimation = nullptr;

    /** 普通握持与瞄准姿势之间的连续混合权重 */
    UPROPERTY(BlueprintReadOnly)
    float AimPresentationAlpha = 0.0f;

    /** 身体是否正在原地转向 */
    /** 实际水平移动速度 */
    UPROPERTY(BlueprintReadOnly)
    float GroundSpeed = 0.0f;

    /** 角色局部前后速度 */
    UPROPERTY(BlueprintReadOnly)
    float LocalForwardSpeed = 0.0f;

    /** 角色局部左右速度 */
    UPROPERTY(BlueprintReadOnly)
    float LocalRightSpeed = 0.0f;

    /** 角色实际水平转向速度，单位为度每秒 */
    UPROPERTY(BlueprintReadOnly)
    float TurnRate = 0.0f;

    /** 实际垂直移动速度 */
    UPROPERTY(BlueprintReadOnly)
    float VerticalSpeed = 0.0f;

    /** 相对角色前轴的水平瞄准角 */
    UPROPERTY(BlueprintReadOnly)
    float AimOffsetYaw = 0.0f;

    /** 瞄准IK权重 */
    UPROPERTY(BlueprintReadOnly)
    float AimIKAlpha = 0.0f;

    /** 左手IK权重 */
    UPROPERTY(BlueprintReadOnly)
    float LeftHandIKAlpha = 0.0f;

    /** 组件空间的瞄准目标点 */
    UPROPERTY(BlueprintReadOnly)
    FVector AimTargetComponentSpace = FVector::ZeroVector;

    /** 瞄准来源相对瞄准骨骼的本地变换 */
    UPROPERTY(BlueprintReadOnly)
    FTransform AimSourceLocalTransform = FTransform::Identity;

    /** 瞄准目标点是否有效 */
    UPROPERTY(BlueprintReadOnly)
    bool bHasValidAimTarget = false;

    /** 瞄准来源是否有效 */
    UPROPERTY(BlueprintReadOnly)
    bool bHasValidAimSource = false;

    /** 按距离衰减的瞄准IK权重 */
    UPROPERTY(BlueprintReadOnly)
    float AimIKDistanceAlpha = 1.0f;

    /** 左手IK目标在右手骨骼空间的完整变换 */
    UPROPERTY(BlueprintReadOnly)
    FTransform LeftHandTargetRightHandBoneSpace = FTransform::Identity;

    UPROPERTY(BlueprintReadOnly)
    //关闭boolHasValidLeftHand目标
    bool bHasValidLeftHandTarget = false;
};

/**
 * 保存实际转向速度计算所需的跨帧朝向
 */
struct FBBBCharacterTurnTrackingState
{
    /** 上一次采样的角色水平朝向 */
    float PreviousActorYaw = 0.0f;

    /** 上一轮角色移动更新使用的帧时间 */
    float PreviousMovementDeltaSeconds = 0.0f;

    /** 是否已有可用于计算的上一帧朝向 */
    bool bHasPreviousActorYaw = false;
};

//保存瞄准表现处理器的跨帧平滑状态
struct FBBBAimPresentationRuntimeState
{
    /** 平滑后的组件空间瞄准目标点 */
    FVector SmoothedAimTargetComponentSpace = FVector::ZeroVector;

    /** 瞄准目标平滑算法的速度状态 */
    FVector AimTargetSmoothVelocity = FVector::ZeroVector;

    /** 平滑后的瞄准IK权重 */
    float SmoothedAimPresentationAlpha = 0.0f;

    /** 平滑后的动画水平瞄准偏角 */
    float SmoothedAimOffsetYaw = 0.0f;

    /** 是否已有平滑瞄准目标 */
    bool bHasSmoothedAimTarget = false;
};
