#include "BBBWork/UBBBNexus/Character/Logic/System/AnimationSystem/Processors/BBBCharacterLifeAnimationProcessor.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/AnimationSystem/DomainData/Context/BBBCharacterAnimationUpdateContext.h"
#include "BBBWork/UBBBNexus/Character/Logic/RuntimeData/BBBCharacterRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/Animation/BBBAnimInstance.h"

void FBBBCharacterLifeAnimationProcessor::Update(FBBBCharacterAnimationUpdateContext &Context) const
{
    auto &State = Context.RuntimeData.Animation.LifeAnimationState;
    const auto &Life = Context.RuntimeData.Life.ReadLifeState();
    const bool bHelping = Context.RuntimeData.Life.ReadRescueState().bHelping;
    if (!Life.bInitialized || (State.AppliedPhase == Life.Phase && State.bAppliedRescueHelping == bHelping))
    {
        return;
    }

    if (State.AppliedPhase != Life.Phase)
    {
        State.PreviousPhase = State.AppliedPhase;
        State.AppliedPhase = Life.Phase;
        State.PhaseStartTime = Context.WorldState.WorldTimeSeconds;
    }
    State.bAppliedRescueHelping = bHelping;
    if (Life.Phase == EBBBCharacterLifePhase::Alive && !bHelping)
    {
        return;
    }

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

}
