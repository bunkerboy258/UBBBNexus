#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "BBBWork/UBBBNexus/Character/Pipeline/Request/Definition/BBBDecisionRuntimeData.h"
#include "BBBWork/UBBBNexus/Equipment/Presentation/Animation/States/BBBEquipmentAnimationStates.h"
#include "BBBEquipmentAnimInstance.generated.h"

struct FBBBEquipmentActionEvent;
class USkeletalMeshComponent;

/** 装备表现动画实例的线程安全状态入口 */
UCLASS(BlueprintType)
class ABBB_EVAC_API UBBBEquipmentAnimInstance : public UAnimInstance
{
    GENERATED_BODY()

public:
    /** @return 瞄准来源相对右手骨骼的本地变换 */
    UFUNCTION(BlueprintPure, Category = "BBB|Aim", meta = (BlueprintThreadSafe))
    FTransform GetAimSourceLocalTransform() const
    {
        return AimSourceLocalTransform;
    }

    /** @return 瞄准来源是否有效 */
    UFUNCTION(BlueprintPure, Category = "BBB|Aim", meta = (BlueprintThreadSafe))
    bool HasValidAimSource() const
    {
        return bHasValidAimSource;
    }

    /** @return 最近一次通过角色仲裁的装备动作类型 */
    UFUNCTION(BlueprintPure, Category = "BBB|Equipment", meta = (BlueprintThreadSafe))
    EBBBCharacterActionType GetEquipmentActionType() const
    {
        return EquipmentActionType;
    }

    /** @return 最近一次通过角色仲裁的装备动作序号 */
    UFUNCTION(BlueprintPure, Category = "BBB|Equipment", meta = (BlueprintThreadSafe))
    int32 GetEquipmentActionSequence() const
    {
        return EquipmentActionSequence;
    }

    /** @return 最近一次通过角色仲裁的装备动作时长 */
    UFUNCTION(BlueprintPure, Category = "BBB|Equipment", meta = (BlueprintThreadSafe))
    float GetEquipmentActionDuration() const
    {
        return EquipmentActionDuration;
    }

    /** @return 最近一次通过角色仲裁的人物动作蒙太奇 */
    UFUNCTION(BlueprintPure, Category = "BBB|Equipment", meta = (BlueprintThreadSafe))
    UAnimMontage *GetEquipmentActionMontage() const
    {
        return EquipmentActionMontage;
    }

    /** @return 最近一次通过角色仲裁的人物动作播放倍率 */
    UFUNCTION(BlueprintPure, Category = "BBB|Equipment", meta = (BlueprintThreadSafe))
    float GetEquipmentActionPlayRate() const
    {
        return EquipmentActionPlayRate;
    }

    /** @return 左手握持目标在角色 hand_r 骨骼空间中的位置 */
    UFUNCTION(BlueprintPure, Category = "BBB|Equipment|Animation", meta = (BlueprintThreadSafe))
    FVector GetLeftHandTargetHandRSpace() const;

    /** @return 左手握持所需的角色网格、武器网格和插槽是否有效 */
    UFUNCTION(BlueprintPure, Category = "BBB|Equipment|Animation", meta = (BlueprintThreadSafe))
    bool HasLeftHandTarget() const;

    /** @return 武器是否已绑定角色 */
    UFUNCTION(BlueprintPure, Category = "BBB|Equipment|Animation", meta = (BlueprintThreadSafe))
    bool HasMainHandEquipment() const;

    /**
     * 设置武器所属角色网格
     * @param InCharacterMesh	所属角色网格，卸下时传入空
     * @return 绑定是否有效
     */
    bool BindCharacterMesh(USkeletalMeshComponent *InCharacterMesh);

    /**
     * 装备时绑定武器瞄准来源
     * @param InAimSourceLocalTransform	瞄准来源相对角色右手骨骼的变换
     * @param bInHasValidAimSource		武器瞄准来源是否有效
     * @return 无
     */
    void BindAimSource(const FTransform &InAimSourceLocalTransform, bool bInHasValidAimSource);

    /**
     * 发布人物装备动作的动画数据
     * @param Event	已经选定的装备动作事件
     * @return 无
     */
    void PublishEquipmentAction(const FBBBEquipmentActionEvent &Event);

    /** 武器网格中的左手握持插槽 */
    UPROPERTY(EditDefaultsOnly, Category = "BBB|Equipment|Animation")
    FName LeftHandSocketName = TEXT("LeftHand");

    /** 左手插槽在武器组件空间中的附加位置偏移 */
    UPROPERTY(EditDefaultsOnly, Category = "BBB|Equipment|Animation")
    FVector LeftHandSocketOffset = FVector::ZeroVector;

    /** @return 最近一次开火序号 */
    UFUNCTION(BlueprintPure, Category = "BBB|Equipment|Animation", meta = (BlueprintThreadSafe))
    int32 GetFireSequence() const
    {
        return AnimationState.FireSequence;
    }

    /** @return 最近一次开火距当前的世界时间 */
    UFUNCTION(BlueprintPure, Category = "BBB|Equipment|Animation", meta = (BlueprintThreadSafe))
    float GetLastFireTimeSeconds() const
    {
        return AnimationState.LastFireTimeSeconds;
    }

    /** @return 最近一次换弹序号 */
    UFUNCTION(BlueprintPure, Category = "BBB|Equipment|Animation", meta = (BlueprintThreadSafe))
    int32 GetReloadSequence() const
    {
        return AnimationState.ReloadSequence;
    }

    /** @return 当前是否正在换弹 */
    UFUNCTION(BlueprintPure, Category = "BBB|Equipment|Animation", meta = (BlueprintThreadSafe))
    bool IsReloading() const
    {
        return AnimationState.bIsReloading;
    }

    /** @return 换弹开始世界时间 */
    UFUNCTION(BlueprintPure, Category = "BBB|Equipment|Animation", meta = (BlueprintThreadSafe))
    float GetReloadStartTimeSeconds() const
    {
        return AnimationState.ReloadStartTimeSeconds;
    }

    /** @return 换弹动作持续时间 */
    UFUNCTION(BlueprintPure, Category = "BBB|Equipment|Animation", meta = (BlueprintThreadSafe))
    float GetReloadDurationSeconds() const
    {
        return AnimationState.ReloadDurationSeconds;
    }

    /** @return 最近一次开火经过的时间 */
    UFUNCTION(BlueprintPure, Category = "BBB|Equipment|Animation", meta = (BlueprintThreadSafe))
    float GetTimeSinceLastFireSeconds() const
    {
        return AnimationState.FireSequence > 0
            ? FMath::Max(AnimationState.CurrentWorldTimeSeconds - AnimationState.LastFireTimeSeconds, 0.0f)
            : BIG_NUMBER;
    }

    /** @return 当前换弹动作经过的时间 */
    UFUNCTION(BlueprintPure, Category = "BBB|Equipment|Animation", meta = (BlueprintThreadSafe))
    float GetReloadElapsedSeconds() const
    {
        return FMath::Max(AnimationState.CurrentWorldTimeSeconds - AnimationState.ReloadStartTimeSeconds, 0.0f);
    }

    /**
     * 判断最近一次开火是否仍在指定时长内
     * @param Duration	动作保持激活的时长
     * @return 最近一次开火是否仍在指定时长内
     */
    UFUNCTION(BlueprintPure, Category = "BBB|Equipment|Animation", meta = (BlueprintThreadSafe))
    bool IsFireActiveForDuration(float Duration) const
    {
        return AnimationState.FireSequence > 0 && GetTimeSinceLastFireSeconds() < FMath::Max(Duration, 0.0f);
    }

    /** @return 当前开火动画是否仍在激活窗口 */
    UFUNCTION(BlueprintPure, Category = "BBB|Equipment|Animation", meta = (BlueprintThreadSafe))
    bool IsFireActive() const
    {
        return AnimationState.bFireActive;
    }

    /** @return 当前装备动画状态快照 */
    UFUNCTION(BlueprintPure, Category = "BBB|Equipment|Animation", meta = (BlueprintThreadSafe))
    FBBBEquipmentAnimationState GetAnimationState() const
    {
        return AnimationState;
    }

    /** 更新开火状态 */
    void NotifyFire(float WorldTimeSeconds);

    /** 更新换弹开始状态 */
    void NotifyReloadStarted(float WorldTimeSeconds, float DurationSeconds);

    /** 更新换弹完成状态 */
    void NotifyReloadCompleted(float WorldTimeSeconds);

protected:
    virtual void NativeUpdateAnimation(float DeltaSeconds) override;

private:
    /** 握持位置转换使用的所属角色网格 */
    UPROPERTY(Transient)
    TWeakObjectPtr<USkeletalMeshComponent> CharacterMesh;

    /** 装备时确定的武器瞄准来源变换 */
    FTransform AimSourceLocalTransform = FTransform::Identity;

    /** 武器瞄准来源是否有效 */
    bool bHasValidAimSource = false;

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

    /** 装备动画状态快照 */
    UPROPERTY(Transient, BlueprintReadOnly, Category = "BBB|Equipment|Animation", meta = (AllowPrivateAccess = "true"))
    FBBBEquipmentAnimationState AnimationState;
};
