
#pragma once

#include "CoreMinimal.h"

struct FBBBCameraCommands;
struct FBBBCharacterAnimationCommands;
struct FBBBCharacterAnimationRequest;

class FBBBCharacterExternalAPI
{
public:

    /**
     * 注入动画与相机命令目标
     * @param InAnimationCommands	动画命令
     * @param InCameraCommands	相机命令
     */
    void Initialize(
        FBBBCharacterAnimationCommands &InAnimationCommands,
        FBBBCameraCommands &InCameraCommands);

    /**
     * 压入动画蒙太奇请求
     * @param Request	动画播放请求
     */
    void QueueMontage(const FBBBCharacterAnimationRequest &Request);
    /**
     * 提交相机后坐力冲量
     * @param Impulse	后坐力二维冲量
     * @param RecoverySpeed	后坐力恢复速度
     */
    void SubmitCameraRecoil(const FVector2D &Impulse, float RecoverySpeed);
    /**
     * 压入锁定物品 IK 请求
     * @param bBlocked	是否锁定物品 IK
     */
    void SubmitLeftHandIKBlockRequest(bool bBlocked);

    /**
     * 提交瞄准IK关闭请求
     * @param bBlocked	是否关闭瞄准IK
     */
    void SubmitAimIKBlockRequest(bool bBlocked);

private:

    FBBBCharacterAnimationCommands *AnimationCommands = nullptr;
    FBBBCameraCommands *CameraCommands = nullptr;
};
