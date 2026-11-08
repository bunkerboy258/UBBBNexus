#pragma once
#include "Animation/AnimSingleNodeInstanceProxy.h"
/** 只在现有身体姿势上附加头部旋转 不访问工作线程外的对象 */
class FBBBMaodieTestAnimInstanceProxy final : public FAnimSingleNodeInstanceProxy
{
public:
    /** @param Instance 所属动画实例 @return 构造代理 */
    explicit FBBBMaodieTestAnimInstanceProxy(UAnimInstance *Instance);
    /** @param Instance 所属实例 @param DeltaSeconds 帧间隔 @return 无 */
    virtual void PreUpdate(UAnimInstance *Instance, float DeltaSeconds) override;
    /** @param Output 身体姿势与头部附加结果 @return 是否完成姿势求值 */
    virtual bool Evaluate(FPoseContext &Output) override;
private:
    /** 本次动画求值使用的机械角度副本 */
    FRotator HeadRotation = FRotator::ZeroRotator;
};
