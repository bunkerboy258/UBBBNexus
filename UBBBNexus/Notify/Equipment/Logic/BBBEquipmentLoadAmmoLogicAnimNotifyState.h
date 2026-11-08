#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "BBBEquipmentLoadAmmoLogicAnimNotifyState.generated.h"

/** 向直接所属装备提交装入弹药请求 */
UCLASS(meta = (DisplayName = "BBB 装备装入弹药"))
class ABBB_EVAC_API UBBBEquipmentLoadAmmoLogicAnimNotifyState final : public UAnimNotifyState
{
    GENERATED_BODY()

public:
    /** @param MeshComp 直接所属装备网格 @param Animation 通知所属动画 @param TotalDuration 通知区间秒数 @param EventReference 通知事件上下文 @return 无 */
    virtual void NotifyBegin(USkeletalMeshComponent *MeshComp, UAnimSequenceBase *Animation,
        float TotalDuration, const FAnimNotifyEventReference &EventReference) override;
};
