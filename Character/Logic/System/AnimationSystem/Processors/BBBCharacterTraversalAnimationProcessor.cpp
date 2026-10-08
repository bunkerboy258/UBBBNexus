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
    UBBBAnimInstance *Layer = Cast<UBBBAnimInstance>(Context.AnimationInstance.GetLinkedAnimLayerInstanceByClass(
        Context.AnimationLayerState.LinkedAnimationLayerClass));
    const float PoseWeight = Layer ? Layer->GetSlotNodeGlobalWeight(BBBCharacterMontageSlots::Traversal) : 0.0f;
    Dispatch.bPoseActive = PoseWeight > KINDA_SMALL_NUMBER;

    // 控制权交接不推进尾姿 等状态混合结束后再清除播放实例
    if (Dispatch.bRootMotionReleased && !Dispatch.bPoseActive && Dispatch.Montage)
    {
        Context.AnimationInstance.RegisterMontageContribution(BBBCharacterMontageSlots::Traversal, nullptr);
        Dispatch.Montage = nullptr;
        Dispatch.MontageInstanceId = INDEX_NONE;
    }

    if (Traversal.Action == EBBBTraversalAction::None || Traversal.bEndRequested
        || Traversal.ActionId == Dispatch.LastActionId || !Layer)
    {
        return;
    }

    // 资产选择仅读取 Base 配置 不再借事件图提交玩法输入
    UAnimMontage *Montage = Layer->SelectTraversalMontage(Traversal.Action);
    Dispatch.LastActionId = Traversal.ActionId;
    Dispatch.Montage = nullptr;
    Dispatch.MontageInstanceId = INDEX_NONE;
    Dispatch.bPlaying = false;
    Dispatch.bRootMotionReleased = false;
    Dispatch.bExitWindowReached = false;
    Dispatch.Position = 0.0f;
    Dispatch.LastWarpEndTime = 0.0f;
    Dispatch.ContactWarpEndTime = 0.0f;

    if (!Montage || Montage->SlotAnimTracks.Num() != 1
        || Montage->SlotAnimTracks[0].SlotName != BBBCharacterMontageSlots::Traversal)
    {
        UE_LOG(LogTemp, Error, TEXT("攀爬动画必须使用独立 Traversal 槽 Action=%d Montage=%s"),
            static_cast<int32>(Traversal.Action), *GetNameSafe(Montage));
        return;
    }

    // 新动作清除尚未消费的装备表现请求 不改写实际装备领域
    Context.AnimationMontageState.FullBodyMontageRequest = nullptr;
    Context.AnimationMontageState.bFullBodyMontageRequestPending = false;
    Context.AnimationMontageState.UpperBodyMontageRequest = nullptr;
    Context.AnimationMontageState.bUpperBodyMontageRequestPending = false;
    Context.AnimationInstance.RegisterMontageContribution(BBBCharacterMontageSlots::FullBody, nullptr);
    Context.AnimationInstance.RegisterMontageContribution(BBBCharacterMontageSlots::UpperBody, nullptr);
    Context.AnimationInstance.RegisterMontageContribution(BBBCharacterMontageSlots::Traversal, nullptr);
    Dispatch.Montage = Montage;
    Context.AnimationMontageState.TraversalMontageRequest = Montage;
    Context.AnimationMontageState.bTraversalMontageRequestPending = true;

    // 退出窗口读取实际元数据 不依赖通知在本地视野内触发
    for (const FAnimNotifyEvent &Notify : Montage->Notifies)
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
