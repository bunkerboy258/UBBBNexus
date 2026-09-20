#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "BBBReloadLoadAnimNotify.generated.h"

/**
 *
 * 换弹动画到达装入弹匣关键帧时提交领域输入
 */
UCLASS()
class ABBB_EVAC_API UBBBReloadLoadAnimNotify final : public UAnimNotify
{
    GENERATED_BODY()

public:
    /**
     *
     * 从触发通知的角色网格提交装入弹匣输入
     * @param MeshComp          触发通知的骨骼网格
     * @param Animation         所属动画资产
     * @param EventReference    本次通知事件引用
     */
    virtual void Notify(
        USkeletalMeshComponent *MeshComp,
        UAnimSequenceBase *Animation,
        const FAnimNotifyEventReference &EventReference) override;
};
