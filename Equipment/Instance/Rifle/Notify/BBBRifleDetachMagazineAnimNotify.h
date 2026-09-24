#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "BBBRifleDetachMagazineAnimNotify.generated.h"

/** 向当前步枪提交弹匣脱离输入的动画通知 */
UCLASS(meta = (DisplayName = "BBB Rifle Detach Magazine"))
class ABBB_EVAC_API UBBBRifleDetachMagazineAnimNotify final : public UAnimNotify
{
    GENERATED_BODY()

public:
    virtual void Notify(USkeletalMeshComponent *MeshComp, UAnimSequenceBase *Animation, const FAnimNotifyEventReference &EventReference) override;
};
