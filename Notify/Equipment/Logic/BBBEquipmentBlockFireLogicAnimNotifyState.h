#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "BBBEquipmentBlockFireLogicAnimNotifyState.generated.h"

class ABBBRifleEquipment;

/** 区间开始禁止开火 区间正常结束或中断时解除限制 */
UCLASS(meta = (DisplayName = "BBB 装备禁止开火"))
class ABBB_EVAC_API UBBBEquipmentBlockFireLogicAnimNotifyState final : public UAnimNotifyState
{
    GENERATED_BODY()
public:
    virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
        float TotalDuration, const FAnimNotifyEventReference& EventReference) override;
    virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
        const FAnimNotifyEventReference& EventReference) override;
private:
    /** 记录区间开始时的接收装备 结束回调不能解锁后来切换的新装备 */
    TMap<TWeakObjectPtr<USkeletalMeshComponent>, TWeakObjectPtr<ABBBRifleEquipment>> Recipients;
};
