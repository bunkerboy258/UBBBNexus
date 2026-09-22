#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "BBBRifleLoadMagazineAnimNotify.generated.h"

/** 向当前步枪提交弹匣装入输入的动画通知 */
UCLASS(meta = (DisplayName = "BBB Rifle Load Magazine"))
class ABBB_EVAC_API UBBBRifleLoadMagazineAnimNotify final : public UAnimNotify
{
    GENERATED_BODY()

public:
    virtual void Notify(USkeletalMeshComponent *MeshComp, UAnimSequenceBase *Animation, const FAnimNotifyEventReference &EventReference) override;
};
