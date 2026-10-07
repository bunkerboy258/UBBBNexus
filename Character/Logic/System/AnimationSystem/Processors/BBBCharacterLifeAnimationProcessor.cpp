#include "BBBWork/UBBBNexus/Character/Logic/System/AnimationSystem/Processors/BBBCharacterLifeAnimationProcessor.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/AnimationSystem/DomainData/Context/BBBCharacterAnimationUpdateContext.h"
#include "BBBWork/UBBBNexus/Character/Logic/RuntimeData/BBBCharacterRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/Config/Animation/BBBCharacterAnimationConfig.h"
#include "BBBWork/UBBBNexus/Character/Animation/BBBAnimInstance.h"

void FBBBCharacterLifeAnimationProcessor::Update(FBBBCharacterAnimationUpdateContext &Context) const
{
    auto &State = Context.RuntimeData.Animation.LifeAnimationState;
    const auto &Life = Context.RuntimeData.Life.ReadLifeState();
    if (!Life.bInitialized || State.AppliedPhase == Life.Phase)
    {
        return;
    }

    State.AppliedPhase = Life.Phase;
    if (Life.Phase == EBBBCharacterLifePhase::Alive)
    {
        return;
    }

    Context.AnimationInstance.Montage_Stop(0.0f);
    Context.AnimationInstance.ClearMontageContributions();
    auto &Requests = Context.AnimationMontageState;
    Requests.FullBodyMontageRequest = nullptr;
    Requests.bFullBodyMontageRequestPending = false;
    Requests.UpperBodyMontageRequest = nullptr;
    Requests.bUpperBodyMontageRequestPending = false;
    Requests.FullBodyAdditivePreAimMontageRequest = nullptr;
    Requests.bFullBodyAdditivePreAimMontageRequestPending = false;
    Requests.UpperBodyAdditiveMontageRequest = nullptr;
    Requests.bUpperBodyAdditiveMontageRequestPending = false;
    Requests.AdditiveHitReactMontageRequest = nullptr;
    Requests.bAdditiveHitReactMontageRequestPending = false;

    const auto &Hit = Context.RuntimeData.Life.ReadHitState();
    if (Life.Phase == EBBBCharacterLifePhase::Downed && Context.AnimationConfig.DownedEntryMontage &&
        Context.WorldState.WorldTimeSeconds - Hit.Time <= 0.25)
    {
        Context.AnimationInstance.RegisterMontageContribution(BBBCharacterMontageSlots::FullBody,
                                                              Context.AnimationConfig.DownedEntryMontage);
    }
}
