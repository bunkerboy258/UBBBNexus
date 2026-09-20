#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "BBBReloadLifecycleAnimNotifyState.generated.h"

/**
 *
 * 覆盖换弹全过程并在未完成装填时提交中断输入
 */
UCLASS()
class ABBB_EVAC_API UBBBReloadLifecycleAnimNotifyState final : public UAnimNotifyState
{
    GENERATED_BODY()

public:
    /**
     *
     * 在换弹通知状态离开活动集合时提交中断输入
     * @param MeshComp          触发通知的骨骼网格
     * @param Animation         所属动画资产
     * @param EventReference    本次通知事件引用
     */
    virtual void NotifyEnd(
        USkeletalMeshComponent *MeshComp,
        UAnimSequenceBase *Animation,
        const FAnimNotifyEventReference &EventReference) override;
};
