#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "BBBRifleReloadLifecycleLogicAnimNotifyState.generated.h"

/** 在武器换弹动画结束时提交步枪换弹收束输入 */
UCLASS(meta = (DisplayName = "BBB 步枪换弹生命周期逻辑"))
class ABBB_EVAC_API UBBBRifleReloadLifecycleLogicAnimNotifyState final : public UAnimNotifyState
{
    GENERATED_BODY()

public:
    /**
     * 动画正常结束或中断时提交换弹收束输入
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
