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

    State.AppliedPhase = Life.Phase;
    State.bAppliedRescueHelping = bHelping;
    State.DownedEntryStartTime = -1.0;
    if (Life.Phase == EBBBCharacterLifePhase::Alive && !bHelping)
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
    Requests.TraversalMontageRequest = nullptr;
    Requests.bTraversalMontageRequestPending = false;

    const auto &Hit = Context.RuntimeData.Life.ReadHitState();
    const double HitAge = Context.WorldState.WorldTimeSeconds - Hit.Time;
    if (Life.Phase == EBBBCharacterLifePhase::Downed && Hit.Serial != 0 && HitAge >= 0.0 && HitAge <= 0.25)
    {
        State.DownedEntryStartTime = Hit.Time;
    }
}
