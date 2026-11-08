#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "BBBShotgunReloadCycleLogicAnimNotifyState.generated.h"

/** 区分逐发装填动作自然结束与被外部打断 */
UCLASS(meta = (DisplayName = "BBB 霰弹枪一发装填周期"))
class ABBB_EVAC_API UBBBShotgunReloadCycleLogicAnimNotifyState final : public UAnimNotifyState
{
    GENERATED_BODY()

public:
    /** 仅收手蒙太奇启用 防止旧装填段结束提前允许射击 */
    UPROPERTY(EditAnywhere, Category = "Reload", meta = (DisplayName = "收手阶段", ToolTip = "仅收手动作启用 结束后允许排队射击 装入一发的动作保持关闭"))
    bool bFinishing = false;

    /**
     * 仅向直接所属霰弹枪提交结束输入
     * @param MeshComp	通知直接所属网格
     * @param Animation	通知所属动画
     * @param EventReference	自然结束或取消的事件上下文
     * @return 无
     */
    virtual void NotifyEnd(USkeletalMeshComponent *MeshComp, UAnimSequenceBase *Animation,
        const FAnimNotifyEventReference &EventReference) override;
};
