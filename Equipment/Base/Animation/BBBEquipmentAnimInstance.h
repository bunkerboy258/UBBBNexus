#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "BBBWork/UBBBNexus/Equipment/Base/Definition/BBBEquipmentAnimationFacts.h"
#include "BBBEquipmentAnimInstance.generated.h"

class UAnimMontage;

/** 只读装备动画事实快照的动画实例 */
UCLASS(BlueprintType)
class ABBB_EVAC_API UBBBEquipmentAnimInstance : public UAnimInstance
{
    GENERATED_BODY()

public:
    /**
     * 在装备动画实例上播放已经批准的蒙太奇
     * @param Montage 待播放的装备蒙太奇
     * @return 是否成功开始播放
     */
    bool PlayEquipmentMontage(UAnimMontage &Montage);

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
