#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "BBBWork/UBBBNexus/Equipment/Template/Definition/BBBEquipmentAnimationFacts.h"
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

    /** @return 装备当前是否正在换弹 */
    UFUNCTION(BlueprintPure, Category = "BBB|Equipment|Animation", meta = (BlueprintThreadSafe))
    bool IsReloading() const
    {
        return AnimationFacts.bIsReloading;
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

    /** @return 本帧已装填弹量 */
    UFUNCTION(BlueprintPure, Category = "BBB|Equipment|Animation", meta = (BlueprintThreadSafe))
    int32 GetLoadedAmmo() const
    {
        return AnimationFacts.LoadedAmmo;
    }

    /** @return 配置弹药容量 */
    UFUNCTION(BlueprintPure, Category = "BBB|Equipment|Animation", meta = (BlueprintThreadSafe))
    int32 GetAmmoCapacity() const
    {
        return AnimationFacts.AmmoCapacity;
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
