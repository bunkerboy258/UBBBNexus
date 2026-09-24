#include "BBBWork/UBBBNexus/Character/Logic/System/AnimationSystem/Processors/BBBCharacterAnimationMontageProcessor.h"

#include "BBBWork/UBBBNexus/Character/Animation/BBBAnimInstance.h"
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

    ConsumeRequest(
        MontageState.FullBodyMontageRequest,
        MontageState.bFullBodyMontageRequestPending,
        BBBCharacterMontageSlots::FullBody);
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
