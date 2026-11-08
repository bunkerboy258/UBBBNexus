#pragma once
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "BBBCharacterQuickMeleeContactLogicAnimNotifyState.generated.h"
/** 快速近战有效命中窗口 */
UCLASS(meta = (DisplayName = "角色快速近战命中窗口"))
class ABBB_EVAC_API UBBBCharacterQuickMeleeContactLogicAnimNotifyState final : public UAnimNotifyState
{
    GENERATED_BODY()
  public:
    virtual void NotifyBegin(USkeletalMeshComponent *Mesh, UAnimSequenceBase *Animation, float Duration,
                             const FAnimNotifyEventReference &Reference) override;
    virtual void NotifyEnd(USkeletalMeshComponent *Mesh, UAnimSequenceBase *Animation,
                           const FAnimNotifyEventReference &Reference) override;

  private:
    TMap<TWeakObjectPtr<USkeletalMeshComponent>, int32> Tokens;
};
