#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "BBBWork/UBBBNexus/Equipment/Presentation/Animation/States/BBBEquipmentAnimationStates.h"
#include "BBBEquipmentAnimInstance.generated.h"

/** 装备表现动画实例的线程安全状态入口 */
UCLASS(BlueprintType)
class ABBB_EVAC_API UBBBEquipmentAnimInstance : public UAnimInstance
{
    GENERATED_BODY()

public:
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
        return FMath::Max(AnimationState.CurrentWorldTimeSeconds - AnimationState.LastFireTimeSeconds, 0.0f);
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
    /** 装备动画状态快照 */
    UPROPERTY(Transient, BlueprintReadOnly, Category = "BBB|Equipment|Animation", meta = (AllowPrivateAccess = "true"))
    FBBBEquipmentAnimationState AnimationState;
};
