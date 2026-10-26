#pragma once

#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "BBBCharacterRescueCompleteLogicAnimNotifyState.generated.h"

/** 接受救援动作自然结束时向直接角色贡献完成输入 */
UCLASS(meta = (DisplayName = "角色接受救援完成"))
class ABBB_EVAC_API UBBBCharacterRescueCompleteLogicAnimNotifyState final : public UAnimNotifyState
{
    GENERATED_BODY()

public:
    virtual void NotifyBegin(USkeletalMeshComponent *Mesh, UAnimSequenceBase *Animation, float Duration,
        const FAnimNotifyEventReference &Reference) override;

    virtual void NotifyEnd(USkeletalMeshComponent *Mesh, UAnimSequenceBase *Animation,
        const FAnimNotifyEventReference &Reference) override;

private:
    /** 按网格及蒙太奇实例绑定操作 防止旧动作通知作用于新救援 */
    TMap<TWeakObjectPtr<USkeletalMeshComponent>, TMap<int32, uint64>> Operations;

    /** 与已绑定操作对应的倒地轮次 */
    TMap<TWeakObjectPtr<USkeletalMeshComponent>, TMap<int32, uint64>> Rounds;
};
