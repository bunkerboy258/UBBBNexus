#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "BBBEquipmentReloadLifecycleLogicAnimNotifyState.generated.h"

/** 向直接所属装备提交换弹生命周期请求 */
UCLASS(meta = (DisplayName = "BBB 装备换弹生命周期"))
class ABBB_EVAC_API UBBBEquipmentReloadLifecycleLogicAnimNotifyState final : public UAnimNotifyState
{
    GENERATED_BODY()

public:
    /** @param MeshComp 直接所属装备网格 @param Animation 通知所属动画 @param EventReference 通知事件上下文 @return 无 */
    virtual void NotifyEnd(USkeletalMeshComponent *MeshComp, UAnimSequenceBase *Animation,
        const FAnimNotifyEventReference &EventReference) override;
};
