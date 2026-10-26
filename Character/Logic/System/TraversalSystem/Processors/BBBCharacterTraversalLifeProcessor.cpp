#include "BBBWork/UBBBNexus/Character/Logic/System/TraversalSystem/Processors/BBBCharacterTraversalLifeProcessor.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/TraversalSystem/DomainData/Context/BBBCharacterTraversalUpdateContext.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/TraversalSystem/DomainData/States/BBBCharacterTraversalState.h"
#include "BBBWork/UBBBNexus/Character/Config/Locomotion/BBBTraversalConfig.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/ParseSystem/DomainData/States/BBBCharacterControlState.h"
#include "BBBWork/UBBBNexus/Character/Logic/RuntimeData/ExternalDomain/States/BBBCharacterWorldState.h"
#include "BBBWork/UBBBNexus/Character/Logic/RuntimeData/ExternalDomain/States/BBBCharacterNetworkIdentityState.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/AnimationSystem/DomainData/States/BBBCharacterTraversalAnimationState.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/AnimationSystem/DomainData/States/BBBCharacterAnimationMontageState.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/LocomotionSystem/DomainData/States/BBBCharacterLocomotionState.h"
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/LifeSystem/DomainData/States/BBBCharacterLifeState.h"

void FBBBCharacterTraversalLifeProcessor::Update(FBBBCharacterTraversalUpdateContext &Context) const
{
    FBBBCharacterTraversalState &State = Context.Traversal;
    if (State.Action == EBBBTraversalAction::None)
    {
        return;
    }

    // 生命阶段改变即结束动作 不等待播放尾段再交还移动权
    if (Context.Life.Phase != EBBBCharacterLifePhase::Alive)
    {
        State.bEndRequested = true;
    }

    const bool bOwnPlayback = Context.Playback.LastActionId == State.ActionId;
    // 尚未形成播放贡献的动作可以直接释放 已启动的各端都必须等待根运动退出确认
    State.bAnimationReleased = bOwnPlayback
        ? Context.Playback.bRootMotionReleased
        : State.bEndRequested && !State.bPlaybackObserved;
    if (bOwnPlayback && Context.Playback.bPlaying)
    {
        State.PlaybackPosition = Context.Playback.Position;
        State.bPlaybackObserved |= Context.Playback.bPlaying;
    }

    // 必须同时得到动画退出与 CMC 释放结果 才能清空业务状态和校正目标
    if (State.bEndRequested && State.bAnimationReleased && !Context.Locomotion.bTraversalControlled)
    {
        State.bTraversing = false;
        State.Action = EBBBTraversalAction::None;
        State.bEndRequested = false;
        State.Obstacle.Reset();
        return;
    }

    if (Context.Execution.bIsMirror)
    {
        /** 镜像只执行结束结果的播放清理 不重新决定动作生命周期 */
        State.bPlaybackRequested = true;
        return;
    }

    if (Context.Montages.HasFullBodyRequest())
    {
        // 全身请求只中断已有动作 不再充当攀爬播放的启动确认
        State.bEndRequested |= State.bPlaybackRequested;
    }
    State.bPlaybackRequested |= bOwnPlayback && Context.Playback.MontageInstanceId != INDEX_NONE;

    const bool bEnded = State.bPlaybackObserved && bOwnPlayback && !Context.Playback.bPlaying;
    const double Elapsed = Context.World.WorldTimeSeconds - State.StartTime;
    const bool bFailed = !State.bPlaybackObserved && Elapsed > Context.TraversalConfig.StartTimeout;
    const bool bFault = bFailed || Elapsed > Context.TraversalConfig.MaxDuration || !State.Obstacle.IsValid();

    if (bFault)
    {
        /** 异常结束发布退出请求 动画晚更新释放根运动并淡出专用蒙太奇 */
        State.bEndRequested = true;
    }

    State.bEndRequested |= bEnded;
}
