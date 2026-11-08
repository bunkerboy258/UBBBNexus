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
    if (!Life.bInitialized)
    {
        State.ImpactReleaseAt = 0;
        State.PendingImpactVelocity = FVector::ZeroVector;
        return;
    }
    if (Hit.ImpactSerial > State.ImpactSerial)
    {
        State.ImpactSerial = Hit.ImpactSerial;
        const FVector Velocity = Hit.ImpactVelocity + Hit.ImpactImpulse / FMath::Max(1.0f, Context.Mesh.GetMass());
        if (Hit.ImpactHoldSeconds > 0 || !Velocity.IsNearlyZero())
        {
            State.ImpactReleaseAt = FMath::Max(State.ImpactReleaseAt, Now + FMath::Max(0.2f, Hit.ImpactHoldSeconds));
            State.bGetUpRequested = false;
            State.bGetUpComplete = false;
            if (State.bRagdoll)
            {
                Context.Mesh.SetAllPhysicsLinearVelocity(Velocity, true);
                Context.Mesh.WakeAllRigidBodies();
            }
            if (!State.bRagdoll)
            {
                State.PendingImpactVelocity = Velocity;
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
