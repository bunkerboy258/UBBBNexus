#pragma once
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "BBBCharacterEquipmentActionLifecycleLogicAnimNotifyState.generated.h"
class ABBBEquipment;
/** 装备攻击生命周期通知 只向直接持有角色提交事实 */
UCLASS(meta = (DisplayName = "装备攻击生命周期"))
class ABBB_EVAC_API UBBBCharacterEquipmentActionLifecycleLogicAnimNotifyState final : public UAnimNotifyState
{
    GENERATED_BODY()
public:
    virtual void NotifyBegin(USkeletalMeshComponent *MeshComp, UAnimSequenceBase *Animation,
        float TotalDuration, const FAnimNotifyEventReference &Reference) override;
    virtual void NotifyEnd(USkeletalMeshComponent *MeshComp, UAnimSequenceBase *Animation,
        const FAnimNotifyEventReference &Reference) override;
private:
    /** 每个播放实例在通知开始时绑定的装备目标 */
    TMap<TWeakObjectPtr<USkeletalMeshComponent>, TMap<int32, TWeakObjectPtr<ABBBEquipment>>> Recipients;
};
