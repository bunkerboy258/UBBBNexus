#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "BBBWork/UBBBNexus/Equipment/System/AnimationSystem/Definition/BBBEquipmentAnimationFacts.h"
#include "BBBEquipmentAnimInstance.generated.h"

/** 只读装备动画事实快照的动画实例 */
UCLASS(BlueprintType)
class ABBB_EVAC_API UBBBEquipmentAnimInstance : public UAnimInstance
{
    GENERATED_BODY()

public:
    /** @return 本帧装备动画事实 */
    UFUNCTION(BlueprintPure, Category = "BBB|Equipment|Animation", meta = (BlueprintThreadSafe))
    FBBBEquipmentAnimationFacts GetAnimationFacts() const
    {
        return AnimationFacts;
    }

    /** @return 瞄准来源相对右手骨骼的本地变换 */
    UFUNCTION(BlueprintPure, Category = "BBB|Aim", meta = (BlueprintThreadSafe))
    FTransform GetAimSourceLocalTransform() const
    {
        return AnimationFacts.AimSourceLocalTransform;
    }

    /** @return 瞄准来源是否有效 */
    UFUNCTION(BlueprintPure, Category = "BBB|Aim", meta = (BlueprintThreadSafe))
    bool HasValidAimSource() const
    {
        return AnimationFacts.bHasValidAimSource;
    }

    /** @return 左手握持目标在角色 hand_r 骨骼空间中的位置 */
    UFUNCTION(BlueprintPure, Category = "BBB|Equipment|Animation", meta = (BlueprintThreadSafe))
    FVector GetLeftHandTargetHandRSpace() const
    {
        return AnimationFacts.LeftHandTargetHandRSpace;
    }

    /** @return 本帧左手握持目标是否有效 */
    UFUNCTION(BlueprintPure, Category = "BBB|Equipment|Animation", meta = (BlueprintThreadSafe))
    bool HasLeftHandTarget() const
    {
        return AnimationFacts.bHasLeftHandTarget;
    }

    /** @return 本帧是否持有主手装备 */
    UFUNCTION(BlueprintPure, Category = "BBB|Equipment|Animation", meta = (BlueprintThreadSafe))
    bool HasMainHandEquipment() const
    {
        return AnimationFacts.bHasMainHandEquipment;
    }

    /** @return 最近一次开火序号 */
    UFUNCTION(BlueprintPure, Category = "BBB|Equipment|Animation", meta = (BlueprintThreadSafe))
    int32 GetFireSequence() const
    {
        return AnimationFacts.FireSequence;
    }

    /** @return 最近一次开火世界时间 */
    UFUNCTION(BlueprintPure, Category = "BBB|Equipment|Animation", meta = (BlueprintThreadSafe))
    float GetLastFireTimeSeconds() const
    {
        return AnimationFacts.LastFireTimeSeconds;
    }

    /** @return 最近一次换弹序号 */
    UFUNCTION(BlueprintPure, Category = "BBB|Equipment|Animation", meta = (BlueprintThreadSafe))
    int32 GetReloadSequence() const
    {
        return AnimationFacts.ReloadSequence;
    }

    /** @return 本帧是否正在换弹 */
    UFUNCTION(BlueprintPure, Category = "BBB|Equipment|Animation", meta = (BlueprintThreadSafe))
    bool IsReloading() const
    {
        return AnimationFacts.bIsReloading;
    }

    /** @return 换弹开始世界时间 */
    UFUNCTION(BlueprintPure, Category = "BBB|Equipment|Animation", meta = (BlueprintThreadSafe))
    float GetReloadStartTimeSeconds() const
    {
        return AnimationFacts.ReloadStartTimeSeconds;
    }

    /** @return 换弹动作持续时间 */
    UFUNCTION(BlueprintPure, Category = "BBB|Equipment|Animation", meta = (BlueprintThreadSafe))
    float GetReloadDurationSeconds() const
    {
        return AnimationFacts.ReloadDurationSeconds;
    }

    /** @return 本帧世界时间 */
    UFUNCTION(BlueprintPure, Category = "BBB|Equipment|Animation", meta = (BlueprintThreadSafe))
    float GetCurrentWorldTimeSeconds() const
    {
        return AnimationFacts.CurrentWorldTimeSeconds;
    }

    /**
     * 一次性发布本帧装备动画事实
     * @param Facts	装备动画系统计算完成的事实
     * @return 无
     */
    void PublishAnimationFacts(const FBBBEquipmentAnimationFacts &Facts);

private:
    UPROPERTY(Transient, BlueprintReadOnly, Category = "BBB|Equipment|Animation", meta = (AllowPrivateAccess = "true"))
    FBBBEquipmentAnimationFacts AnimationFacts;
};
