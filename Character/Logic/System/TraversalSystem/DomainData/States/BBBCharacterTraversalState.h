#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Config/Locomotion/BBBTraversalAction.h"
#include "BBBCharacterTraversalState.generated.h"

class UPrimitiveComponent;

/** 翻越动作的几何目标与独立生命周期 */
USTRUCT()
struct FBBBCharacterTraversalState final
{
    GENERATED_BODY()

    /** 当前已经确定的翻越动作 */
    UPROPERTY(Transient)
    EBBBTraversalAction Action = EBBBTraversalAction::None;

    /** 当前动作标识 用于拒绝过期播放反馈 */
    UPROPERTY(Transient)
    uint32 ActionId = 0;

    /** 障碍前侧顶部的世界空间基准 */
    UPROPERTY(Transient)
    FTransform ContactTarget = FTransform::Identity;

    /** 动作完成时脚底的世界空间目标 */
    UPROPERTY(Transient)
    FTransform EndTarget = FTransform::Identity;

    /** 命中障碍 用于检查动作期间目标是否仍然存在 */
    UPROPERTY(Transient)
    TWeakObjectPtr<UPrimitiveComponent> Obstacle;

    /** 动作申请的世界时间 */
    double StartTime = 0.0;

    /** 动画层是否已经提交有效的播放请求 */
    bool bPlaybackRequested = false;

    /** 是否已经观察到实际蒙太奇播放 */
    bool bPlaybackObserved = false;

    /** 本次动作是否需要结束并恢复移动 */
    bool bEndRequested = false;

    /** 本次动作需要的动画退出已经完成 */
    bool bAnimationReleased = false;

    /** 控制端已经提交的播放进度用于恢复当前表现 */
    float PlaybackPosition = 0.0f;

    /** 本动作已经安装的命名目标需要在退出后清理 */
    bool bTargetsInstalled = false;
};
