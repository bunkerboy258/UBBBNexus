#include "BBBWork/UBBBNexus/Character/Logic/System/AnimationSystem/Processors/BBBCharacterTraversalAnimationProcessor.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/AnimationSystem/DomainData/Context/BBBCharacterAnimationUpdateContext.h"
#include "BBBWork/UBBBNexus/Character/Logic/RuntimeData/BBBCharacterRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/Animation/BBBAnimInstance.h"
#include "Animation/AnimMontage.h"
#include "AnimNotifyState_MotionWarping.h"
#include "RootMotionModifier.h"

void FBBBCharacterTraversalAnimationProcessor::Update(FBBBCharacterAnimationUpdateContext &Context) const
{
    const FBBBCharacterTraversalState &Traversal = Context.RuntimeData.Traversal.ReadTraversalState();
    FBBBCharacterTraversalAnimationState &Dispatch = Context.RuntimeData.Animation.TraversalAnimationState;
    FAnimMontageInstance *PoseInstance = Dispatch.MontageInstanceId != INDEX_NONE
        ? Context.AnimationInstance.GetMontageInstanceForID(Dispatch.MontageInstanceId) : nullptr;
    Dispatch.bPoseActive = PoseInstance && PoseInstance->GetWeight() > KINDA_SMALL_NUMBER;
    if (Traversal.Action == EBBBTraversalAction::None)
    {
        return;
    }

    if (Traversal.ActionId != Dispatch.LastActionId && !Traversal.bEndRequested)
    {
        UBBBAnimInstance *Layer = Cast<UBBBAnimInstance>(Context.AnimationInstance.GetLinkedAnimLayerInstanceByClass(
            Context.AnimationLayerState.LinkedAnimationLayerClass));
        if (!Layer)
        {
            return;
        }
        // 攀爬可以中断已有全身操作 清除旧请求后再让 Base 层选择攀爬动画
        Context.AnimationMontageState.FullBodyMontageRequest = nullptr;
        Context.AnimationMontageState.bFullBodyMontageRequestPending = false;
        Context.AnimationInstance.RegisterMontageContribution(BBBCharacterMontageSlots::FullBody, nullptr);
        Context.AnimationInstance.RegisterMontageContribution(BBBCharacterMontageSlots::UpperBody, nullptr);
        Dispatch.LastActionId = Traversal.ActionId;
        Dispatch.Montage = nullptr;
        Dispatch.MontageInstanceId = INDEX_NONE;
        Dispatch.bPlaying = false;
        Dispatch.bPoseActive = true;
        Dispatch.bRootMotionReleased = false;
        Dispatch.bExitWindowReached = false;
        Dispatch.Position = 0.0f;
        Dispatch.LastWarpEndTime = 0.0f;
        Dispatch.ContactWarpEndTime = 0.0f;
        Layer->TraversalRequested(Traversal.Action);
    }

    // 捕获本动作提交的初始动画 后来的 FullBody 请求不能改写所属关系
    if (!Dispatch.Montage && Context.AnimationMontageState.HasFullBodyAnimationRequest() && !Traversal.bEndRequested)
    {
        Dispatch.Montage = Context.AnimationMontageState.FullBodyMontageRequest;
        for (const FAnimNotifyEvent &Notify : Dispatch.Montage->Notifies)
        {
            const UAnimNotifyState_MotionWarping *Window = Cast<UAnimNotifyState_MotionWarping>(Notify.NotifyStateClass);
            const URootMotionModifier_Warp *Warp = Window ? Cast<URootMotionModifier_Warp>(Window->RootMotionModifier) : nullptr;
            if (Warp && (Warp->WarpTargetName == TEXT("TraversalContact") || Warp->WarpTargetName == TEXT("TraversalEnd")))
            {
                Dispatch.LastWarpEndTime = FMath::Max(Dispatch.LastWarpEndTime, Notify.GetEndTriggerTime());
            }
            if (Warp && Warp->WarpTargetName == TEXT("TraversalContact"))
            {
                Dispatch.ContactWarpEndTime = FMath::Max(Dispatch.ContactWarpEndTime, Notify.GetEndTriggerTime());
            }
        }
    }

    if (Traversal.bEndRequested && Dispatch.LastActionId != Traversal.ActionId)
    {
        Dispatch.LastActionId = Traversal.ActionId;
    }

    FAnimMontageInstance *Instance = Dispatch.Montage
        ? Context.AnimationInstance.GetActiveInstanceForMontage(Dispatch.Montage) : nullptr;
    // 同一资产被后续请求重播时不再视作本次攀爬 避免停止新操作
    if (Instance && Dispatch.MontageInstanceId != INDEX_NONE
        && Dispatch.MontageInstanceId != Instance->GetInstanceID())
    {
        Instance = nullptr;
    }
    if (Instance)
    {
        Dispatch.MontageInstanceId = Instance->GetInstanceID();
        Dispatch.Position = Instance->GetPosition();
    }
    Dispatch.bPlaying = Instance && Instance->IsPlaying() && !Instance->IsStopped();
    Dispatch.bExitWindowReached = Dispatch.LastWarpEndTime > 0.0f
        && Dispatch.Position >= Dispatch.LastWarpEndTime;

    if (Traversal.bEndRequested)
    {
        // 停止位移贡献但保留姿势淡出 让 CMC 在下一帧安全接回玩家输入
        if (Instance && !Dispatch.bRootMotionReleased)
        {
            Instance->PushDisableRootMotion();
            // 从实际交接姿势淡出 避免继续推进到源动画的站立收尾
            Instance->Pause();
            Context.AnimationInstance.Montage_Stop(Dispatch.Montage->BlendOut.GetBlendTime(), Dispatch.Montage);
        }
        Dispatch.bRootMotionReleased = true;
    }
}
