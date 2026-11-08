#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "BBBEquipmentBoneVisibilityDisplayAnimNotifyState.generated.h"

/** 在独立表现物体接替动画网格时隐藏指定骨骼 区间结束或中断后恢复 */
UCLASS(meta = (DisplayName = "BBB 装备骨骼显隐表现"))
class ABBB_EVAC_API UBBBEquipmentBoneVisibilityDisplayAnimNotifyState final : public UAnimNotifyState
{
    GENERATED_BODY()

public:
    /** 直接播放装备动画的网格骨骼 隐藏其所属顶点及子骨骼 不修改玩法状态 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BBB|装备|骨骼", meta = (DisplayName = "隐藏骨骼", AnimNotifyBoneName = "true", ToolTip = "独立弹壳等视觉物体接替原动画网格的区间 不允许同一骨骼的显隐区间重叠"))
    FName BoneName = NAME_None;

    /**
     * 隐藏直接播放此动画的装备网格骨骼
     * @param MeshComp		直接播放动画的装备网格
     * @param Animation		当前动画
     * @param TotalDuration	通知区间秒数
     * @param EventReference	当前通知上下文
     * @return 无
     */
    virtual void NotifyBegin(USkeletalMeshComponent *MeshComp, UAnimSequenceBase *Animation,
        float TotalDuration, const FAnimNotifyEventReference &EventReference) override;

    /**
     * 恢复直接播放此动画的装备网格骨骼
     * @param MeshComp		直接播放动画的装备网格
     * @param Animation		当前动画
     * @param EventReference	当前通知上下文
     * @return 无
     */
    virtual void NotifyEnd(USkeletalMeshComponent *MeshComp, UAnimSequenceBase *Animation,
        const FAnimNotifyEventReference &EventReference) override;
};
