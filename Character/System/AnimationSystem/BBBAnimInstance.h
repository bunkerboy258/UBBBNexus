#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "BBBWork/UBBBNexus/Character/Pipeline/Request/Definition/BBBDecisionRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/System/AnimationSystem/Definition/States/BBBCharacterAnimationStates.h"
#include "BBBWork/UBBBNexus/Character/System/AnimationSystem/Diagnostics/BBBLocomotionRuntimeProbe.h"
#include "BBBAnimInstance.generated.h"

class FBBBCharacterAnimationActionProcessor;
class FBBBCharacterAnimationSystem;
class UAnimMontage;

/** 角色动画事实快照、蓝图读取与瞬时动作转发入口 */
UCLASS()
class ABBB_EVAC_API UBBBAnimInstance : public UAnimInstance
{
    GENERATED_BODY()

    friend class FBBBCharacterAnimationActionProcessor;
    friend class FBBBCharacterAnimationSystem;

public:
    /** @return 无 */
    virtual void NativeInitializeAnimation() override;

    /** @return 无 */
    virtual void NativePostEvaluateAnimation() override;

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
        return AnimationFacts.bIsAiming;
    }

    /** @return 玩家瞄准意图的连续强度 */
    UFUNCTION(BlueprintPure, Category = "BBB|Aim", meta = (BlueprintThreadSafe))
    float GetAimIntentAlpha() const
    {
        return AnimationFacts.AimIntentAlpha;
    }

    /** @return 瞄准 IK 权重 */
    UFUNCTION(BlueprintPure, Category = "BBB|Aim", meta = (BlueprintThreadSafe))
    float GetAimIKAlpha() const
    {
        return AnimationFacts.AimIKAlpha;
    }

    /** @return 组件空间的瞄准目标点 */
    UFUNCTION(BlueprintPure, Category = "BBB|Aim", meta = (BlueprintThreadSafe))
    FVector GetAimTargetComponentSpace() const
    {
        return AnimationFacts.AimTargetComponentSpace;
    }

    /** @return 瞄准来源相对右手骨骼的本地变换 */
    UFUNCTION(BlueprintPure, Category = "BBB|Aim", meta = (BlueprintThreadSafe))
    FTransform GetAimSourceLocalTransform() const
    {
        return AnimationFacts.AimSourceLocalTransform;
    }

    /** @return 瞄准目标点是否有效 */
    UFUNCTION(BlueprintPure, Category = "BBB|Aim", meta = (BlueprintThreadSafe))
    bool HasValidAimTarget() const
    {
        return AnimationFacts.bHasValidAimTarget;
    }

    /** @return 瞄准来源是否有效 */
    UFUNCTION(BlueprintPure, Category = "BBB|Aim", meta = (BlueprintThreadSafe))
    bool HasValidAimSource() const
    {
        return AnimationFacts.bHasValidAimSource;
    }

    /** @return 主手是否持有装备 */
    UFUNCTION(BlueprintPure, Category = "BBB|Equipment", meta = (BlueprintThreadSafe))
    bool HasMainHandEquipment() const
    {
        return AnimationFacts.bHasMainHandEquipment;
    }

    /** @return 当前本地或远端恢复的换弹是否仍在进行 */
    UFUNCTION(BlueprintPure, Category = "BBB|Equipment", meta = (BlueprintThreadSafe))
    bool IsReloading() const
    {
        return AnimationFacts.bIsReloading;
    }

    /** @return 距离上次本地或远端开火经过的秒数 */
    UFUNCTION(BlueprintPure, Category = "BBB|Equipment", meta = (BlueprintThreadSafe))
    float GetTimeSinceLastFire() const
    {
        return AnimationFacts.TimeSinceLastFire;
    }

    /**
     * 判断最近一次开火是否仍应维持上半身持枪姿势
     * @param Duration	持枪姿势维持时长
     * @return 是否仍处于指定维持时间内
     */
    UFUNCTION(BlueprintPure, Category = "BBB|Equipment", meta = (BlueprintThreadSafe))
    bool ShouldRaiseWeaponAfterFiring(float Duration) const
    {
        return AnimationFacts.TimeSinceLastFire < FMath::Max(Duration, 0.0f);
    }

    /** @return 最近一次通过角色仲裁的装备动作类型 */
    UFUNCTION(BlueprintPure, Category = "BBB|Equipment")
    EBBBCharacterActionType GetEquipmentActionType() const
    {
        return EquipmentActionType;
    }

    /** @return 最近一次通过角色仲裁的装备动作序号 */
    UFUNCTION(BlueprintPure, Category = "BBB|Equipment")
    int32 GetEquipmentActionSequence() const
    {
        return EquipmentActionSequence;
    }

    /** @return 最近一次通过角色仲裁的装备动作时长 */
    UFUNCTION(BlueprintPure, Category = "BBB|Equipment")
    float GetEquipmentActionDuration() const
    {
        return EquipmentActionDuration;
    }

    /** @return 最近一次通过角色仲裁的人物动作蒙太奇 */
    UFUNCTION(BlueprintPure, Category = "BBB|Equipment")
    UAnimMontage *GetEquipmentActionMontage() const
    {
        return EquipmentActionMontage;
    }

    /** @return 最近一次通过角色仲裁的人物动作播放倍率 */
    UFUNCTION(BlueprintPure, Category = "BBB|Equipment")
    float GetEquipmentActionPlayRate() const
    {
        return EquipmentActionPlayRate;
    }

    /**
     * 执行角色代码已经选定的人物动作蒙太奇
     * @param ActionType	动作类型
     * @param Montage		人物动作蒙太奇
     * @param PlayRate		播放倍率
     * @return 无
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "BBB|Equipment")
    void ExecuteEquipmentActionMontage(
        EBBBCharacterActionType ActionType,
        UAnimMontage *Montage,
        float PlayRate);

    /** 角色胶囊体底部到地面的距离，供动画属性存取节点直接读取 */
    UPROPERTY(BlueprintReadOnly, Transient, Category = "BBB|Animation Facts")
    float GroundDistance = 0.0f;

private:
    /**
     * 提交移动完成后的角色事实快照
     * @param Facts 新事实快照
     */
    void PublishAnimationFacts(const FBBBCharacterAnimationFacts &Facts);

    /**
     * 提交已经通过角色仲裁的动作信号并通知动画蓝图执行
     * @param ActionType	动作类型
     * @param Sequence		动作序号
     * @param Duration		动作时长
     * @param Montage		人物动作蒙太奇
     * @param PlayRate		播放倍率
     */
    void PublishEquipmentAction(
        EBBBCharacterActionType ActionType,
        int32 Sequence,
        float Duration,
        UAnimMontage &Montage,
        float PlayRate);

    UPROPERTY(Transient)
    FBBBCharacterAnimationFacts AnimationFacts;

    UPROPERTY(BlueprintReadOnly, Transient, Category = "BBB|Equipment", meta = (AllowPrivateAccess = "true"))
    EBBBCharacterActionType EquipmentActionType = EBBBCharacterActionType::None;

    UPROPERTY(BlueprintReadOnly, Transient, Category = "BBB|Equipment", meta = (AllowPrivateAccess = "true"))
    int32 EquipmentActionSequence = INDEX_NONE;

    UPROPERTY(BlueprintReadOnly, Transient, Category = "BBB|Equipment", meta = (AllowPrivateAccess = "true"))
    float EquipmentActionDuration = 0.0f;

    UPROPERTY(BlueprintReadOnly, Transient, Category = "BBB|Equipment", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UAnimMontage> EquipmentActionMontage = nullptr;

    UPROPERTY(BlueprintReadOnly, Transient, Category = "BBB|Equipment", meta = (AllowPrivateAccess = "true"))
    float EquipmentActionPlayRate = 1.0f;

    FBBBLocomotionRuntimeProbe LocomotionRuntimeProbe;
};
