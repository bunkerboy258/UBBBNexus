#pragma once

#include "CoreMinimal.h"

struct FAnimInstanceProxy;
class UAnimInstance;
class UBBBAnimInstance;

/** 记录移动、动画播放器与最终脚部姿势的只读运行时探针 */
class FBBBLocomotionRuntimeProbe final
{
public:
    /** 清空跨帧速度计算状态 */
    void Reset();

    /** @return 当前是否通过控制台变量启用了探针 */
    static bool IsEnabled();

    /**
     * 记录主动画实例的移动事实、蓝图派生值与最终骨骼姿势
     * @param AnimInstance 主动画实例
     * @param AnimationProxy 主动画代理
     */
    void CaptureMainInstance(
        const UBBBAnimInstance &AnimInstance,
        const FAnimInstanceProxy &AnimationProxy);

    /**
     * 记录链接动画层的蓝图派生值与同步播放器
     * @param AnimInstance 链接动画层实例
     * @param AnimationProxy 链接动画层代理
     */
    void CaptureLinkedInstance(
        const UAnimInstance &AnimInstance,
        const FAnimInstanceProxy &AnimationProxy) const;

private:
    FVector PreviousActorLocation = FVector::ZeroVector;

    FVector PreviousPelvisLocation = FVector::ZeroVector;

    FVector PreviousLeftFootLocation = FVector::ZeroVector;

    FVector PreviousRightFootLocation = FVector::ZeroVector;

    double PreviousSampleTime = 0.0;

    bool bHasPreviousSample = false;
};
