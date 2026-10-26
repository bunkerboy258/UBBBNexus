#include "BBBWork/UBBBNexus/Character/Logic/System/AnimationSystem/Processors/BBBCharacterTraversalAnimationProcessor.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/AnimationSystem/DomainData/Context/BBBCharacterAnimationUpdateContext.h"
#include "BBBWork/UBBBNexus/Character/Logic/RuntimeData/BBBCharacterRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/Animation/BBBAnimInstance.h"
#include "Animation/AnimMontage.h"

void FBBBCharacterTraversalAnimationProcessor::Update(FBBBCharacterAnimationUpdateContext &Context) const
{
    const FBBBCharacterTraversalState &Traversal = Context.RuntimeData.Traversal.ReadTraversalState();
    FBBBCharacterTraversalAnimationState &Dispatch = Context.RuntimeData.Animation.TraversalAnimationState;
    UBBBAnimInstance *Layer = Cast<UBBBAnimInstance>(Context.AnimationInstance.GetLinkedAnimLayerInstanceByClass(
        Context.AnimationLayerState.LinkedAnimationLayerClass));
    const float PoseWeight = Context.AnimationInstance.GetSlotMontageGlobalWeight(BBBCharacterMontageSlots::Traversal);
    Dispatch.bPoseActive = PoseWeight > KINDA_SMALL_NUMBER;

    // 专用槽淡出结束后清除本动作的播放引用
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
    Dispatch.Position = 0.0f;

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

}
