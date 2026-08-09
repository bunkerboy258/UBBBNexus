
#pragma once
#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "BBBWork/UBBBNexus/Character/System/AimSystem/Definition/States/BBBAimStates.h"
#include "BBBWork/UBBBNexus/Character/System/AnimationSystem/Definition/States/BBBCharacterAnimationStates.h"
#include "BBBAnimInstance.generated.h"

class UAnimSequence;

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

    /** @return 是否保持瞄准姿态 */
    UFUNCTION(BlueprintPure, Category = "BBB|Aim", meta = (BlueprintThreadSafe))
    bool IsAiming() const
    {
        return AimState && AimState->bIsAiming;
    }

    /** @return 是否存在实际水平移动 */
    UFUNCTION(BlueprintPure, Category = "BBB|Locomotion", meta = (BlueprintThreadSafe))
    bool IsMoving() const
    {
        return GetAnimationState().bIsMoving;
    }

    /** @return 是否接触地面 */
    UFUNCTION(BlueprintPure, Category = "BBB|Locomotion", meta = (BlueprintThreadSafe))
    bool IsGrounded() const
    {
        return GetAnimationState().bIsGrounded;
    }

    /** @return 主手是否持有装备 */
    UFUNCTION(BlueprintPure, Category = "BBB|Locomotion", meta = (BlueprintThreadSafe))
    bool HasMainHandEquipment() const
    {
        return GetAnimationState().bHasMainHandEquipment;
    }

    /** @return 当前装备提供的持续上半身动画 */
    UFUNCTION(BlueprintPure, Category = "BBB|Equipment", meta = (BlueprintThreadSafe))
    UAnimSequence *GetEquippedUpperBodyAnimation() const
    {
        return GetAnimationState().EquippedUpperBodyAnimation;
    }

    /** @return 当前装备是否提供持续上半身动画 */
    UFUNCTION(BlueprintPure, Category = "BBB|Equipment", meta = (BlueprintThreadSafe))
    bool HasEquippedUpperBodyAnimation() const
    {
        return GetAnimationState().EquippedUpperBodyAnimation != nullptr;
    }

    /** @return 身体是否正在原地转向 */
    UFUNCTION(BlueprintPure, Category = "BBB|Locomotion", meta = (BlueprintThreadSafe))
    bool IsTurningInPlace() const
    {
        return GetAnimationState().bIsTurningInPlace;
    }

    /** @return 实际水平移动速度 */
    UFUNCTION(BlueprintPure, Category = "BBB|Locomotion", meta = (BlueprintThreadSafe))
    float GetGroundSpeed() const
    {
        return GetAnimationState().GroundSpeed;
    }

    /** @return 角色局部前后速度 */
    UFUNCTION(BlueprintPure, Category = "BBB|Locomotion", meta = (BlueprintThreadSafe))
    float GetLocalForwardSpeed() const
    {
        return GetAnimationState().LocalForwardSpeed;
    }

    /** @return 角色局部左右速度 */
    UFUNCTION(BlueprintPure, Category = "BBB|Locomotion", meta = (BlueprintThreadSafe))
    float GetLocalRightSpeed() const
    {
        return GetAnimationState().LocalRightSpeed;
    }

    /** @return 实际垂直移动速度 */
    UFUNCTION(BlueprintPure, Category = "BBB|Locomotion", meta = (BlueprintThreadSafe))
    float GetVerticalSpeed() const
    {
        return GetAnimationState().VerticalSpeed;
    }

    /** @return 相对角色前轴的水平瞄准角 */
    UFUNCTION(BlueprintPure, Category = "BBB|Locomotion", meta = (BlueprintThreadSafe))
    float GetAimYaw() const
    {
        return GetAnimationState().AimYaw;
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
     * 读取瞄准IK权重
     * @return 瞄准IK权重
     */
    UFUNCTION(BlueprintPure, Category = "BBB|Aim", meta = (BlueprintThreadSafe))
    float GetAimIKAlpha() const
    {
        return GetAnimationState().AimIKAlpha;
    }

    /**
     * 读取左手IK目标在右手插槽空间的位置
     * @return 左手IK目标位置
     */
    UFUNCTION(BlueprintPure, Category = "BBB|LeftHandIK", meta = (BlueprintThreadSafe))
    FVector GetLeftHandIKTargetLocation() const
    {
        return GetAnimationState().LeftHandTargetRightHandSocketSpace.GetLocation();
    }

    /**
     * 左手IK目标是否有效
     * @return 左手IK目标有效时返回true
     */
    UFUNCTION(BlueprintPure, Category = "BBB|LeftHandIK", meta = (BlueprintThreadSafe))
    bool HasValidLeftHandTarget() const
    {
        return GetAnimationState().bHasValidLeftHandTarget;
    }

    /**
     * 读取左手IK权重
     * @return 左手IK权重
     */
    UFUNCTION(BlueprintPure, Category = "BBB|LeftHandIK", meta = (BlueprintThreadSafe))
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

    /** 缓存的角色动画状态引用 由所属角色每帧刷新 */
    const FBBBCharacterAnimationState *AnimationState = nullptr;

    /** 缓存的瞄准只读状态 */
    const FBBBAimRuntimeState *AimState = nullptr;
};
