#pragma once
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "BBBCharacterItemReleaseUseLogicAnimNotifyState.generated.h"

/** 直接向角色提交通用物品通知 不解释物品内部逻辑 */
UCLASS(meta = (DisplayName = "角色物品ReleaseUse"))
class ABBB_EVAC_API UBBBCharacterItemReleaseUseLogicAnimNotifyState final : public UAnimNotifyState
{
    GENERATED_BODY()
public:
    virtual void NotifyBegin(USkeletalMeshComponent *Mesh, UAnimSequenceBase *Animation, float Duration, const FAnimNotifyEventReference &Reference) override;

private:
};
