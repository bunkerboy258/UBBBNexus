
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

    /** 主手是否持有装备 */
    UPROPERTY(BlueprintReadOnly)
    bool bHasMainHandEquipment = false;

    /** 当前装备提供的持续上半身动画 */
    UPROPERTY(BlueprintReadOnly)
    TObjectPtr<UAnimSequence> EquippedUpperBodyAnimation = nullptr;

    /** 身体是否正在原地转向 */
    UPROPERTY(BlueprintReadOnly)
    bool bIsTurningInPlace = false;

    /** 实际水平移动速度 */
    UPROPERTY(BlueprintReadOnly)
    float GroundSpeed = 0.0f;

    /** 角色局部前后速度 */
    UPROPERTY(BlueprintReadOnly)
    float LocalForwardSpeed = 0.0f;

    /** 角色局部左右速度 */
    UPROPERTY(BlueprintReadOnly)
    float LocalRightSpeed = 0.0f;

    /** 实际垂直移动速度 */
    UPROPERTY(BlueprintReadOnly)
    float VerticalSpeed = 0.0f;

    /** 相对角色前轴的水平瞄准角 */
    UPROPERTY(BlueprintReadOnly)
    float AimYaw = 0.0f;

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

    /** 平滑后的动画水平瞄准偏角 */
    float SmoothedAimYaw = 0.0f;

    /** 是否已有平滑瞄准目标 */
    bool bHasSmoothedAimTarget = false;
};
