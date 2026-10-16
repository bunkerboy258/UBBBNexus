#pragma once
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "BBBCharacterRecoveryEffectLogicAnimNotifyState.generated.h"
/** 恢复动作生效节点 只向直接角色提交输入 */
UCLASS(meta = (DisplayName = "角色恢复物生效"))
class ABBB_EVAC_API UBBBCharacterRecoveryEffectLogicAnimNotifyState final : public UAnimNotifyState
{
    GENERATED_BODY()
  public:
    virtual void NotifyBegin(USkeletalMeshComponent *Mesh, UAnimSequenceBase *Animation, float Duration,
                             const FAnimNotifyEventReference &Reference) override;
};
