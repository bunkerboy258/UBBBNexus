#include "BBBWork/UBBBNexus/Character/Logic/System/PhysicalPresentationSystem/Processors/BBBCharacterKnockdownProcessor.h"

#include "BBBWork/UBBBNexus/Character/Logic/System/PhysicalPresentationSystem/DomainData/Context/BBBCharacterPhysicalPresentationUpdateContext.h"
#include "BBBWork/UBBBNexus/Character/Logic/RuntimeData/BBBCharacterRuntimeData.h"
#include "Components/SkeletalMeshComponent.h"

void FBBBCharacterKnockdownProcessor::Update(FBBBCharacterPhysicalPresentationUpdateContext &Context) const
{
    if (Context.Data.External.ReadNetworkIdentityState().bIsMirror)
    {
        return;
    }
    auto &State = Context.Data.PhysicalPresentation.PhysicalPresentationState;
    const auto &Life = Context.Data.Life.ReadLifeState();
    const auto &Hit = Context.Data.Life.ReadHitState();
    const double Now = Context.Data.External.ReadWorldState().WorldTimeSeconds;
    if (!Life.bInitialized || Life.Phase == EBBBCharacterLifePhase::Dead)
    {
        State.ImpactReleaseAt = 0;
        State.PendingImpactVelocity = FVector::ZeroVector;
        return;
    }
    if (Hit.ImpactSerial > State.ImpactSerial)
    {
        State.ImpactSerial = Hit.ImpactSerial;
        if (Hit.Impact.IsValid() && Hit.Impact.HoldSeconds > 0)
        {
            State.ImpactReleaseAt = FMath::Max(State.ImpactReleaseAt, Now + Hit.Impact.HoldSeconds);
            State.bGetUpRequested = false;
            State.bGetUpComplete = false;
            if (State.bRagdoll)
            {
                Context.Mesh.SetAllPhysicsLinearVelocity(Hit.Impact.Velocity, true);
                Context.Mesh.WakeAllRigidBodies();
            }
            if (!State.bRagdoll)
            {
                State.PendingImpactVelocity = Hit.Impact.Velocity;
                State.bRagdollRequested = true;
            }
        }
    }
    if (State.ImpactReleaseAt > 0 && Now >= State.ImpactReleaseAt && State.bRagdoll)
    {
        State.bGetUpRequested = Life.Phase == EBBBCharacterLifePhase::Alive;
        State.ImpactReleaseAt = 0;
    }
}
