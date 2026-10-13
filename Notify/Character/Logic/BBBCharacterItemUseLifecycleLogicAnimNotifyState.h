#pragma once
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "BBBCharacterItemUseLifecycleLogicAnimNotifyState.generated.h"

/** 直接向角色提交通用物品通知 不解释物品内部逻辑 */
UCLASS(meta = (DisplayName = "角色物品UseLifecycle"))
class ABBB_EVAC_API UBBBCharacterItemUseLifecycleLogicAnimNotifyState final : public UAnimNotifyState
{
    GENERATED_BODY()
public:
    virtual void NotifyBegin(USkeletalMeshComponent *Mesh, UAnimSequenceBase *Animation, float Duration, const FAnimNotifyEventReference &Reference) override;
    virtual void NotifyEnd(USkeletalMeshComponent *Mesh, UAnimSequenceBase *Animation, const FAnimNotifyEventReference &Reference) override;
private:
    TMap<TWeakObjectPtr<USkeletalMeshComponent>, TWeakObjectPtr<AActor>> Recipients;
    TMap<TWeakObjectPtr<USkeletalMeshComponent>, uint64> Generations;
    TMap<TWeakObjectPtr<USkeletalMeshComponent>, int32> Tokens;
    TMap<TWeakObjectPtr<USkeletalMeshComponent>, FGuid> Instances;
};
