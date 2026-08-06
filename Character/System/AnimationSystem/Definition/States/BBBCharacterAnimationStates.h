
#pragma once
#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/System/AnimationSystem/Definition/States/BBBCharacterLocomotionStates.h"
#include "BBBCharacterAnimationStates.generated.h"

USTRUCT(BlueprintType)
//角色动画状态封装
struct FBBBCharacterAnimationState
{
    GENERATED_BODY()

    /** 当前移动动画表现状态 */
    UPROPERTY(BlueprintReadOnly)
    EBBBLocomotionState LocomotionState = EBBBLocomotionState::Idle;

    /** 归一化移动输入 X为左右 Y为前后 */
    UPROPERTY(BlueprintReadOnly)
    FVector2D MoveInput = FVector2D::ZeroVector;

    /** 平滑后的归一化移动输入 */
    UPROPERTY(BlueprintReadOnly)
    FVector2D SmoothedMoveInput = FVector2D::ZeroVector;

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

    /** 左手IK目标在右手插槽空间的变换 */
    UPROPERTY(BlueprintReadOnly)
    FTransform LeftHandTargetRightHandSocketSpace = FTransform::Identity;

    UPROPERTY(BlueprintReadOnly)
    //关闭boolHasValidLeftHand目标
    bool bHasValidLeftHandTarget = false;
};

//保存瞄准表现处理器的跨帧平滑状态
struct FBBBAimPresentationRuntimeState
{
    /** 平滑后的组件空间瞄准目标点 */
    FVector SmoothedAimTargetComponentSpace = FVector::ZeroVector;

    /** 瞄准目标平滑算法的速度状态 */
    FVector AimTargetSmoothVelocity = FVector::ZeroVector;

    /** 平滑后的瞄准IK权重 */
    float SmoothedAimIKAlpha = 0.0f;

    /** 是否已有平滑瞄准目标 */
    bool bHasSmoothedAimTarget = false;
};

//保存移动表现处理器的原地转身运行时状态
struct FBBBLocomotionPresentationRuntimeState
{
    /** 正在向左原地转身 */
    bool bIsTurningInPlaceLeft = false;

    /** 正在向右原地转身 */
    bool bIsTurningInPlaceRight = false;
};
