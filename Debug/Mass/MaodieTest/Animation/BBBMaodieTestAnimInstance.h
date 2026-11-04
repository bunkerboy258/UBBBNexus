#pragma once
#include "Animation/AnimSingleNodeInstance.h"
#include "BBBMaodieTestAnimInstance.generated.h"
/** 在单动作身体动画后接入独立头部注视的动画实例 */
UCLASS(Transient, NotBlueprintable)
class ABBB_EVAC_API UBBBMaodieTestAnimInstance final : public UAnimSingleNodeInstance
{
    GENERATED_BODY()
protected:
    /** @return 身体动画与头部附加旋转的线程代理 */
    virtual FAnimInstanceProxy *CreateAnimInstanceProxy() override;
private:
    friend class FBBBMaodieTestAnimationProcessor;
    friend class FBBBMaodieTestAnimInstanceProxy;
    /** AnimationSystem 发布的只读头部角度快照 */
    UPROPERTY(Transient, VisibleAnywhere, BlueprintReadOnly, Category = "Look", meta = (AllowPrivateAccess = "true"))
    FRotator HeadRotation = FRotator::ZeroRotator;
};
