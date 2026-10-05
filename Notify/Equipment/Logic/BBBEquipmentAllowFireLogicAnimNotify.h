#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "BBBEquipmentAllowFireLogicAnimNotify.generated.h"

/** 在可射击帧解除装备动画限制 弹药与射速条件仍由装备检查 */
UCLASS(meta = (DisplayName = "BBB 装备允许开火"))
class ABBB_EVAC_API UBBBEquipmentAllowFireLogicAnimNotify final : public UAnimNotify
{
    GENERATED_BODY()
public:
    virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
        const FAnimNotifyEventReference& EventReference) override;
};
