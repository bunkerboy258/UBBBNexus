#pragma once

#include "Animation/AnimInstance.h"
#include "Animation/PoseSnapshot.h"
#include "BBBZombieGibPoseAnimInstance.generated.h"

class USkeletalMeshComponent;

/** 掉落部件只捕获一次当前姿态 后续运动交给受限物理 */
UCLASS(Transient)
class ABBB_EVAC_API UBBBZombieGibPoseAnimInstance final : public UAnimInstance
{
    GENERATED_BODY()
public:
    /** 当前静止驱动姿态 不是命中历史 */
    FPoseSnapshot Pose;

    /** @param Source	断开瞬间的身体 @return 捕获当前局部骨骼姿态 无返回值 */
    void Capture(USkeletalMeshComponent& Source);

    /** @return 创建只求值固定姿态的动画代理 */
    virtual FAnimInstanceProxy* CreateAnimInstanceProxy() override;

    /** @param Proxy	当前代理 @return 释放代理 无返回值 */
    virtual void DestroyAnimInstanceProxy(FAnimInstanceProxy* Proxy) override;
};
