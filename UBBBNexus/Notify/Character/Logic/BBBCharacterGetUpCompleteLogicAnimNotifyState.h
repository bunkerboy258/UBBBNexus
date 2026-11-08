#pragma once
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "BBBCharacterGetUpCompleteLogicAnimNotifyState.generated.h"

/** 起身自然结束后向直接角色提交完成轮次 */
UCLASS(meta = (DisplayName = "角色起身完成"))
class ABBB_EVAC_API UBBBCharacterGetUpCompleteLogicAnimNotifyState final : public UAnimNotifyState
{
    GENERATED_BODY()
public:
    virtual void NotifyBegin(USkeletalMeshComponent *Mesh, UAnimSequenceBase *Animation, float Duration,
        const FAnimNotifyEventReference &Reference) override;
    virtual void NotifyEnd(USkeletalMeshComponent *Mesh, UAnimSequenceBase *Animation,
        const FAnimNotifyEventReference &Reference) override;
private:
    /** 按网格及蒙太奇实例绑定进入物理轮次 */
    TMap<TWeakObjectPtr<USkeletalMeshComponent>, TMap<int32, uint64>> Episodes;
};
