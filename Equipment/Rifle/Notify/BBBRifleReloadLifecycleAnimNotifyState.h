#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "BBBRifleReloadLifecycleAnimNotifyState.generated.h"

/** 换弹蒙太奇结束时向当前步枪提交打断输入的通知状态 */
UCLASS(meta = (DisplayName = "BBB Rifle Reload Lifecycle"))
class ABBB_EVAC_API UBBBRifleReloadLifecycleAnimNotifyState final : public UAnimNotifyState
{
    GENERATED_BODY()

public:
    virtual void NotifyEnd(USkeletalMeshComponent *MeshComp, UAnimSequenceBase *Animation, const FAnimNotifyEventReference &EventReference) override;
};
