#include "BBBWork/UBBBNexus/Character/Logic/System/AnimationSystem/Processors/BBBCharacterAnimationMontageProcessor.h"

#include "BBBWork/UBBBNexus/Character/Animation/BBBAnimInstance.h"
#include "BBBWork/UBBBNexus/Character/Logic/RuntimeData/BBBCharacterRuntimeData.h"
#include "Animation/AnimMontage.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/AnimationSystem/DomainData/Context/BBBCharacterAnimationUpdateContext.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/AnimationSystem/DomainData/States/BBBCharacterAnimationMontageState.h"

void FBBBCharacterAnimationMontageProcessor::Update(
    FBBBCharacterAnimationUpdateContext &Context) const
{
    FBBBCharacterAnimationMontageState &MontageState = Context.AnimationMontageState;

    // 待消费标记区分未收到输入和显式空引用 清空后再交接可避免请求跨帧重复执行
    const auto ConsumeRequest = [&Context](
        TObjectPtr<UAnimMontage> &MontageRequest,
        bool &bRequestPending,
        const FName SlotName)
    {
        if (!bRequestPending)
        {
            return;
        }

        UAnimMontage *RequestedMontage = MontageRequest.Get();
        MontageRequest = nullptr;
        bRequestPending = false;
        Context.AnimationInstance.RegisterMontageContribution(SlotName, RequestedMontage);
    };

    const FBBBCharacterTraversalState &Traversal = Context.RuntimeData.Traversal.ReadTraversalState();
    const FBBBCharacterTraversalAnimationState &Playback = Context.RuntimeData.Animation.ReadTraversalAnimationState();
    const bool bRestoreCurrentPose = MontageState.HasFullBodyAnimationRequest()
        && MontageState.FullBodyMontageRequest == Playback.Montage
        && Traversal.PlaybackPosition > 0.0f && !Traversal.bEndRequested;

    ConsumeRequest(
        MontageState.FullBodyMontageRequest,
        MontageState.bFullBodyMontageRequestPending,
        BBBCharacterMontageSlots::FullBody);
    if (bRestoreCurrentPose)
    {
        // 迟到观察者直接显示当前动画进度 不补播进入视野以前的过程
        Context.AnimationInstance.Montage_SetPosition(Playback.Montage,
            FMath::Min(Traversal.PlaybackPosition, Playback.Montage->GetPlayLength()));
    }

    ConsumeRequest(
        MontageState.UpperBodyMontageRequest,
        MontageState.bUpperBodyMontageRequestPending,
        BBBCharacterMontageSlots::UpperBody);
    ConsumeRequest(
        MontageState.FullBodyAdditivePreAimMontageRequest,
        MontageState.bFullBodyAdditivePreAimMontageRequestPending,
        BBBCharacterMontageSlots::FullBodyAdditivePreAim);
    ConsumeRequest(
        MontageState.UpperBodyAdditiveMontageRequest,
        MontageState.bUpperBodyAdditiveMontageRequestPending,
        BBBCharacterMontageSlots::UpperBodyAdditive);
    ConsumeRequest(
        MontageState.AdditiveHitReactMontageRequest,
        MontageState.bAdditiveHitReactMontageRequestPending,
        BBBCharacterMontageSlots::AdditiveHitReact);
}
