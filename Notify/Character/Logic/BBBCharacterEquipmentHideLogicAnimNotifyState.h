#pragma once
#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "BBBCharacterEquipmentHideLogicAnimNotifyState.generated.h"
/** 在通知开始时提交一次装备隐藏请求 */
UCLASS(meta = (DisplayName = "BBB 隐藏装备"))
class ABBB_EVAC_API UBBBCharacterEquipmentHideLogicAnimNotifyState final : public UAnimNotifyState
{
    GENERATED_BODY()
public:
    /**
     * @param MeshComp	播放人物动画的网格
     * @param Animation	当前动画
     * @param TotalDuration	通知区间秒数 不控制恢复
     * @param Reference	通知执行上下文
     * @return 无
     */
    virtual void NotifyBegin(USkeletalMeshComponent *MeshComp, UAnimSequenceBase *Animation,
        float TotalDuration, const FAnimNotifyEventReference &Reference) override;
};
