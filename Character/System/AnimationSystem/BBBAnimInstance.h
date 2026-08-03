
#pragma once
#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "BBBWork/UBBBNexus/Character/System/AnimationSystem/Context/BBBCharacterAnimationStates.h"
#include "BBBAnimInstance.generated.h"

UCLASS()
class ABBB_EVAC_API UBBBAnimInstance : public UAnimInstance
{
    GENERATED_BODY()
public:

    /**
     * 动画实例初始化时缓存角色动画状态引用
     */
    virtual void NativeInitializeAnimation() override;

    /**
     * 动画实例每帧更新 状态引用失效时重新缓存
     * @param DeltaSeconds	距上一帧的时间间隔
     */
    virtual void NativeUpdateAnimation(float DeltaSeconds) override;

    /**
     * 读取当前移动状态枚举
     * @return Locomotion状态枚举
     */
    UFUNCTION(BlueprintPure, Category = "BBB|Locomotion", meta = (BlueprintThreadSafe))
    EBBBLocomotionState GetLocomotionState() const
    {
        return GetAnimationState().LocomotionState;
    }

    /**
     * 读取归一化左右移动输入
     * @return 归一化左右移动输入
     */
    UFUNCTION(BlueprintPure, Category = "BBB|Locomotion", meta = (BlueprintThreadSafe))
    float GetNormalizedMoveRight() const
    {
        return GetAnimationState().MoveInput.X;
    }

    /**
     * 读取归一化前后移动输入
     * @return 归一化前后移动输入
     */
    UFUNCTION(BlueprintPure, Category = "BBB|Locomotion", meta = (BlueprintThreadSafe))
    float GetNormalizedMoveForward() const
    {
        return GetAnimationState().MoveInput.Y;
    }

    /**
     * 读取平滑后的左右移动输入
     * @return 平滑后的左右移动输入
     */
    UFUNCTION(BlueprintPure, Category = "BBB|Locomotion", meta = (BlueprintThreadSafe))
    float GetSmoothedMoveRight() const
    {
        return GetAnimationState().SmoothedMoveInput.X;
    }

    /**
     * 读取平滑后的前后移动输入
     * @return 平滑后的前后移动输入
     */
    UFUNCTION(BlueprintPure, Category = "BBB|Locomotion", meta = (BlueprintThreadSafe))
    float GetSmoothedMoveForward() const
    {
        return GetAnimationState().SmoothedMoveInput.Y;
    }

    /**
     * 是否正在向左原地转身
     * @return 正在向左原地转身时返回true
     */
    UFUNCTION(BlueprintPure, Category = "BBB|Locomotion", meta = (BlueprintThreadSafe))
    bool IsTurningInPlaceLeft() const
    {
        return GetAnimationState().bIsTurningInPlaceLeft;
    }

    /**
     * 是否正在向右原地转身
     * @return 正在向右原地转身时返回true
     */
    UFUNCTION(BlueprintPure, Category = "BBB|Locomotion", meta = (BlueprintThreadSafe))
    bool IsTurningInPlaceRight() const
    {
        return GetAnimationState().bIsTurningInPlaceRight;
    }

    /**
     * 是否有开火意图
     * @return 有开火意图时返回true
     */
    UFUNCTION(BlueprintPure, Category = "BBB|Weapon", meta = (BlueprintThreadSafe))
    bool WantsToFire() const
    {
        return GetAnimationState().bWantsFire;
    }

    /**
     * 是否有瞄准意图
     * @return 有瞄准意图时返回true
     */
    UFUNCTION(BlueprintPure, Category = "BBB|Weapon", meta = (BlueprintThreadSafe))
    bool WantsToAim() const
    {
        return GetAnimationState().bWantsAim;
    }

    /**
     * 读取组件空间的瞄准目标点
     * @return 组件空间瞄准目标点
     */
    UFUNCTION(BlueprintPure, Category = "BBB|Aim", meta = (BlueprintThreadSafe))
    FVector GetAimTargetComponentSpace() const
    {
        return GetAnimationState().AimTargetComponentSpace;
    }

    /**
     * 读取瞄准来源相对瞄准骨骼的本地变换
     * @return 瞄准来源本地变换
     */
    UFUNCTION(BlueprintPure, Category = "BBB|Aim", meta = (BlueprintThreadSafe))
    FTransform GetAimSourceLocalTransform() const
    {
        return GetAnimationState().AimSourceLocalTransform;
    }

    /**
     * 瞄准目标点是否有效
     * @return 瞄准目标点有效时返回true
     */
    UFUNCTION(BlueprintPure, Category = "BBB|Aim", meta = (BlueprintThreadSafe))
    bool HasValidAimTarget() const
    {
        return GetAnimationState().bHasValidAimTarget;
    }

    /**
     * 瞄准来源是否有效
     * @return 瞄准来源有效时返回true
     */
    UFUNCTION(BlueprintPure, Category = "BBB|Aim", meta = (BlueprintThreadSafe))
    bool HasValidAimSource() const
    {
        return GetAnimationState().bHasValidAimSource;
    }


    /**
     * 读取左手IK目标在右手插槽空间的位置
     * @return 左手IK目标位置
     */
    UFUNCTION(BlueprintPure, Category = "BBB|HandIK", meta = (BlueprintThreadSafe))
    FVector GetLeftHandIKTargetLocation() const
    {
        return GetAnimationState().LeftHandTargetRightHandSocketSpace.GetLocation();
    }

    /**
     * 左手IK目标是否有效
     * @return 左手IK目标有效时返回true
     */
    UFUNCTION(BlueprintPure, Category = "BBB|HandIK", meta = (BlueprintThreadSafe))
    bool HasValidLeftHandTarget() const
    {
        return GetAnimationState().bHasValidLeftHandTarget;
    }

    /**
     * 读取瞄准IK权重
     * @return 瞄准IK权重
     */
    UFUNCTION(BlueprintPure, Category = "BBB|IKAlpha", meta = (BlueprintThreadSafe))
    float GetAimIKAlpha() const
    {
        return GetAnimationState().AimIKAlpha;
    }

    /**
     * 读取左手IK权重
     * @return 左手IK权重
     */
    UFUNCTION(BlueprintPure, Category = "BBB|IKAlpha", meta = (BlueprintThreadSafe))
    float GetLeftHandIKAlpha() const
    {
        return GetAnimationState().LeftHandIKAlpha;
    }
protected:

    /**
     * 从所属角色刷新缓存的动画状态引用
     */
    void RefreshCachedReferences();

    /**
     * 读取动画状态 引用无效时返回静态空状态
     * @return 角色动画状态
     */
    const FBBBCharacterAnimationState &GetAnimationState() const;

    const FBBBCharacterAnimationState *AnimationState = nullptr;
};
