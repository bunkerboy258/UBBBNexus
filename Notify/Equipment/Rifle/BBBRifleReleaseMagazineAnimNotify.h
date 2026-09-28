#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "BBBRifleReleaseMagazineAnimNotify.generated.h"

/** 向当前步枪提交甩出旧弹匣输入的动画通知 */
UCLASS(meta = (DisplayName = "BBB Rifle Release Magazine"))
class ABBB_EVAC_API UBBBRifleReleaseMagazineAnimNotify final : public UAnimNotify
{
    GENERATED_BODY()

public:
    /**
     * 向角色当前步枪提交单次甩匣输入
     * @param MeshComp		触发通知的角色网格
     * @param Animation	动画资产
     * @param EventReference	通知事件引用
     * @return 无
     */
    virtual void Notify(USkeletalMeshComponent *MeshComp, UAnimSequenceBase *Animation, const FAnimNotifyEventReference &EventReference) override;
};
