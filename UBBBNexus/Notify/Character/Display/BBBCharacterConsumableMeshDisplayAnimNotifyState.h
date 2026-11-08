#pragma once
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "BBBCharacterConsumableMeshDisplayAnimNotifyState.generated.h"
class UStaticMeshComponent;
/** 只在通知区间生成消耗品临时网格 */
UCLASS(meta = (DisplayName = "角色消耗品临时网格"))
class ABBB_EVAC_API UBBBCharacterConsumableMeshDisplayAnimNotifyState final : public UAnimNotifyState
{
    GENERATED_BODY()
  public:
    virtual void NotifyBegin(USkeletalMeshComponent *Mesh, UAnimSequenceBase *Animation, float Duration,
                             const FAnimNotifyEventReference &Reference) override;
    virtual void NotifyTick(USkeletalMeshComponent *Mesh, UAnimSequenceBase *Animation, float DeltaTime,
                            const FAnimNotifyEventReference &Reference) override;
    virtual void NotifyEnd(USkeletalMeshComponent *Mesh, UAnimSequenceBase *Animation,
                           const FAnimNotifyEventReference &Reference) override;

  private:
    /** 每个显示实例的临时组件 结束时销毁 */
    TMap<TWeakObjectPtr<USkeletalMeshComponent>, TWeakObjectPtr<UStaticMeshComponent>> Displays;
};
