#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "BBBRifleMagazineVisibilityDisplayAnimNotifyState.generated.h"

/** 在武器换弹动画中管理枪上弹匣骨骼的可见性 */
UCLASS(meta = (DisplayName = "BBB 步枪弹匣可见性表现"))
class ABBB_EVAC_API UBBBRifleMagazineVisibilityDisplayAnimNotifyState final : public UAnimNotifyState
{
    GENERATED_BODY()

public:
    /** 枪上需要隐藏的弹匣骨骼 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BBB|步枪|弹匣", meta = (DisplayName = "武器弹匣骨骼名"))
    FName WeaponMagazineBoneName = TEXT("Magazine_joint");

    /**
     * 在拔出动作开始时隐藏枪上弹匣
     * @param MeshComp		播放武器动画的网格
     * @param Animation	动画资产
     * @param TotalDuration	通知区间长度
     * @param EventReference	通知事件引用
     * @return 无
     */
    virtual void NotifyBegin(
        USkeletalMeshComponent *MeshComp,
        UAnimSequenceBase *Animation,
        float TotalDuration,
        const FAnimNotifyEventReference &EventReference) override;

    /**
     * 在动画结束或中断时恢复枪上弹匣
     * @param MeshComp		播放武器动画的网格
     * @param Animation	动画资产
     * @param EventReference	通知事件引用
     * @return 无
     */
    virtual void NotifyEnd(
        USkeletalMeshComponent *MeshComp,
        UAnimSequenceBase *Animation,
        const FAnimNotifyEventReference &EventReference) override;
};
