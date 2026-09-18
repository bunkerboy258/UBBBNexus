#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "BBBWork/UBBBNexus/Character/Input/Events/BBBEquipmentActionEvent.h"
#include "BBBWork/UBBBNexus/Character/Runtime/System/AnimationSystem/Definition/States/BBBCharacterAnimationStates.h"
#include "BBBAnimInstance.generated.h"

class FBBBCharacterAnimationActionProcessor;
class FBBBCharacterAnimationSystem;
class UBBBEquipmentAnimInstance;
class UAnimMontage;

/** 角色动画事实快照、蓝图读取与瞬时动作转发入口 */
UCLASS()
class ABBB_EVAC_API UBBBAnimInstance : public UAnimInstance
{
    GENERATED_BODY()

    friend class FBBBCharacterAnimationSystem;

public:
    /** @return 当前骨骼网格体上的 BBB 主动画实例；主实例自身调用时返回自身 */
    UFUNCTION(BlueprintPure, Category = "BBB|Animation Facts", meta = (BlueprintThreadSafe))
    UBBBAnimInstance *GetBBBMainAnimInstanceThreadSafe() const;

    /** 本帧角色世界位置 */
    UPROPERTY(BlueprintReadOnly, Transient, Category = "BBB|Animation Facts")
    FVector SourceActorLocation = FVector::ZeroVector;

    /** 本帧角色世界旋转 */
    UPROPERTY(BlueprintReadOnly, Transient, Category = "BBB|Animation Facts")
    FRotator SourceActorRotation = FRotator::ZeroRotator;

    /** 本帧角色世界速度 */
    UPROPERTY(BlueprintReadOnly, Transient, Category = "BBB|Animation Facts")
    FVector SourceVelocity = FVector::ZeroVector;

    /** 移动组件上一次更新后的世界速度 */
    UPROPERTY(BlueprintReadOnly, Transient, Category = "BBB|Animation Facts")
    FVector SourceLastUpdateVelocity = FVector::ZeroVector;

    /** 本帧角色世界加速度 */
    UPROPERTY(BlueprintReadOnly, Transient, Category = "BBB|Animation Facts")
    FVector SourceAcceleration = FVector::ZeroVector;

    /** 本帧引擎移动模式 */
    UPROPERTY(BlueprintReadOnly, Transient, Category = "BBB|Animation Facts")
    TEnumAsByte<EMovementMode> SourceMovementMode = MOVE_None;

    /** 本帧地面摩擦力 */
    UPROPERTY(BlueprintReadOnly, Transient, Category = "BBB|Animation Facts")
    float SourceGroundFriction = 0.0f;

    /** 本帧制动摩擦力 */
    UPROPERTY(BlueprintReadOnly, Transient, Category = "BBB|Animation Facts")
    float SourceBrakingFriction = 0.0f;

    /** 本帧制动摩擦系数 */
    UPROPERTY(BlueprintReadOnly, Transient, Category = "BBB|Animation Facts")
    float SourceBrakingFrictionFactor = 0.0f;

    /** 本帧行走制动减速度 */
    UPROPERTY(BlueprintReadOnly, Transient, Category = "BBB|Animation Facts")
    float SourceBrakingDecelerationWalking = 0.0f;

    /** 是否使用独立制动摩擦力 */
    UPROPERTY(BlueprintReadOnly, Transient, Category = "BBB|Animation Facts")
    bool bSourceUseSeparateBrakingFriction = false;

    /** 本帧重力加速度 */
    UPROPERTY(BlueprintReadOnly, Transient, Category = "BBB|Animation Facts")
    float SourceGravityZ = 0.0f;

    /** 本帧是否在地面移动 */
    UPROPERTY(BlueprintReadOnly, Transient, Category = "BBB|Animation Facts")
    bool bSourceMovingOnGround = false;

    /** 本帧是否处于蹲伏 */
    UPROPERTY(BlueprintReadOnly, Transient, Category = "BBB|Animation Facts")
    bool bSourceCrouching = false;

    /** @return BBBNexus 本地计算或网络恢复后的精确步态 */
    UFUNCTION(BlueprintPure, Category = "BBB|Locomotion", meta = (BlueprintThreadSafe))
    EBBBCharacterGait GetGait() const
    {
        return AnimationFacts.Gait;
    }

    /** @return 精确步态是否为行走 */
    UFUNCTION(BlueprintPure, Category = "BBB|Locomotion", meta = (BlueprintThreadSafe))
    bool IsWalking() const
    {
        return AnimationFacts.Gait == EBBBCharacterGait::Walk;
    }

    /** @return 精确步态是否为奔跑 */
    UFUNCTION(BlueprintPure, Category = "BBB|Locomotion", meta = (BlueprintThreadSafe))
    bool IsRunning() const
    {
        return AnimationFacts.Gait == EBBBCharacterGait::Run;
    }

    /** @return 精确步态是否为冲刺 */
    UFUNCTION(BlueprintPure, Category = "BBB|Locomotion", meta = (BlueprintThreadSafe))
    bool IsSprinting() const
    {
        return AnimationFacts.Gait == EBBBCharacterGait::Sprint;
    }

    /** @return 玩家是否具有瞄准意图 */
    UFUNCTION(BlueprintPure, Category = "BBB|Aim", meta = (BlueprintThreadSafe))
    bool IsAiming() const
    {
        return GetBBBMainAnimInstanceThreadSafe()->AnimationFacts.bIsAiming;
    }

    /** @return 玩家瞄准意图的连续强度 */
    UFUNCTION(BlueprintPure, Category = "BBB|Aim", meta = (BlueprintThreadSafe))
    float GetAimIntentAlpha() const
    {
        return GetBBBMainAnimInstanceThreadSafe()->AnimationFacts.AimIntentAlpha;
    }

    /** @return 瞄准 IK 权重 */
    UFUNCTION(BlueprintPure, Category = "BBB|Aim", meta = (BlueprintThreadSafe))
    float GetAimIKAlpha() const
    {
        return GetBBBMainAnimInstanceThreadSafe()->AnimationFacts.AimIKAlpha;
    }

    /** @return 组件空间的瞄准目标点 */
    UFUNCTION(BlueprintPure, Category = "BBB|Aim", meta = (BlueprintThreadSafe))
    FVector GetAimTargetComponentSpace() const
    {
        return GetBBBMainAnimInstanceThreadSafe()->AnimationFacts.AimTargetComponentSpace;
    }

    /** @return 当前装备的实际武器动画实例，链接层自动读取主实例绑定 */
    UFUNCTION(BlueprintPure, Category = "BBB|Equipment", meta = (BlueprintThreadSafe))
    UBBBEquipmentAnimInstance *TryGetWeaponAnimInstance() const;

    /** @return 武器瞄准来源本地变换，未装备时返回单位变换 */
    UFUNCTION(BlueprintPure, Category = "BBB|Equipment", meta = (BlueprintThreadSafe))
    FTransform TryGetWeaponAimSourceLocalTransform() const;

    /** @return 左手目标在右手骨骼空间中的位置，未装备时返回零向量 */
    UFUNCTION(BlueprintPure, Category = "BBB|Equipment", meta = (BlueprintThreadSafe))
    FVector TryGetWeaponLeftHandTargetHandRSpace() const;

    /** @return 武器左手目标是否有效，未装备时返回 false */
    UFUNCTION(BlueprintPure, Category = "BBB|Equipment", meta = (BlueprintThreadSafe))
    bool TryHasWeaponLeftHandTarget() const;

    /** @return 当前武器是否正在换弹 未装备时返回 false */
    UFUNCTION(BlueprintPure, Category = "BBB|Equipment", meta = (BlueprintThreadSafe))
    bool TryGetWeaponReloading() const;

    /** @return 距离当前武器上次开火的秒数 未装备时返回极大值 */
    UFUNCTION(BlueprintPure, Category = "BBB|Equipment", meta = (BlueprintThreadSafe))
    float TryGetWeaponTimeSinceLastFireSeconds() const;

    /**
     * 绑定装备实际使用的武器动画实例
     * @param InWeaponAnimInstance	武器动画实例，卸下时传入空
     * @return 无
     */
    void BindWeaponAnimInstance(UBBBEquipmentAnimInstance *InWeaponAnimInstance);

    /** 角色胶囊体底部到地面的距离，供动画属性存取节点直接读取 */
    UPROPERTY(BlueprintReadOnly, Transient, Category = "BBB|Animation Facts")
    float GroundDistance = 0.0f;

private:
    /**
     * 提交移动完成后的角色事实快照
     * @param Facts 新事实快照
     */
    void PublishAnimationFacts(const FBBBCharacterAnimationFacts &Facts);

    UPROPERTY(Transient)
    FBBBCharacterAnimationFacts AnimationFacts;

    /** 当前主手武器实际使用的动画实例 */
    UPROPERTY(Transient)
    TWeakObjectPtr<UBBBEquipmentAnimInstance> WeaponAnimInstance;
};
