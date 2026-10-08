#include "BBBWork/UBBBNexus/Character/Logic/System/AnimationSystem/Processors/BBBCharacterTraversalPlaybackProcessor.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/AnimationSystem/DomainData/Context/BBBCharacterAnimationUpdateContext.h"
#include "BBBWork/UBBBNexus/Character/Logic/RuntimeData/BBBCharacterRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/Animation/BBBAnimInstance.h"
#include "Animation/AnimMontage.h"

void FBBBCharacterTraversalPlaybackProcessor::Update(FBBBCharacterAnimationUpdateContext &Context) const
{
    const FBBBCharacterTraversalState &Traversal = Context.RuntimeData.Traversal.ReadTraversalState();
    FBBBCharacterTraversalAnimationState &Playback = Context.RuntimeData.Animation.TraversalAnimationState;
    if (Traversal.Action == EBBBTraversalAction::None || Playback.LastActionId != Traversal.ActionId)
    {
        return;
    }

    FAnimMontageInstance *Instance = Playback.Montage
        ? Context.AnimationInstance.GetActiveInstanceForMontage(Playback.Montage) : nullptr;
    // 同资产的新播放不能取得旧动作的停止权限
    if (Instance && Playback.MontageInstanceId != INDEX_NONE
        && Playback.MontageInstanceId != Instance->GetInstanceID())
    {
        Instance = nullptr;
    }

    if (Instance)
    {
        // 首次消费就恢复迟到观察者的当前进度 不补播已经经过的入场
        if (Playback.MontageInstanceId == INDEX_NONE && Traversal.PlaybackPosition > 0.0f)
        {
            Context.AnimationInstance.Montage_SetPosition(Playback.Montage,
                FMath::Min(Traversal.PlaybackPosition, Playback.Montage->GetPlayLength()));
        }
        Playback.MontageInstanceId = Instance->GetInstanceID();
        Playback.Position = Instance->GetPosition();
    }
    Playback.bPlaying = Instance && Instance->IsPlaying() && !Instance->IsStopped();
    Playback.bExitWindowReached = Playback.LastWarpEndTime > 0.0f
        && Playback.Position >= Playback.LastWarpEndTime;

    if (!Traversal.bEndRequested || Playback.bRootMotionReleased)
    {
        return;
    }

    // 位移立即退出 尾姿不再推进 混合权重交给主状态机
    if (Instance)
    {
        Instance->PushDisableRootMotion();
        Instance->Pause();
    }
    Playback.bPlaying = false;
    Playback.bRootMotionReleased = true;
}
