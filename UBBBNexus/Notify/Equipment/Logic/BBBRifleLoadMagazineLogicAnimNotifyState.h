#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "BBBRifleLoadMagazineLogicAnimNotifyState.generated.h"

/** 在武器换弹动画的装入帧提交步枪装填输入 */
UCLASS(meta = (DisplayName = "BBB 步枪装入弹匣逻辑"))
class ABBB_EVAC_API UBBBRifleLoadMagazineLogicAnimNotifyState final : public UAnimNotifyState
{
    GENERATED_BODY()

public:
    /**
     * 向具有事实生成权限的步枪提交装填输入
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
};
