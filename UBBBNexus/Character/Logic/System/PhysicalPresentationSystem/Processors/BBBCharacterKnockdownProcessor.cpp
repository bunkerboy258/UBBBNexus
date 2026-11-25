#include "BBBWork/UBBBNexus/Character/Logic/System/PhysicalPresentationSystem/Processors/BBBCharacterKnockdownProcessor.h"

#include "BBBWork/UBBBNexus/Character/Logic/System/PhysicalPresentationSystem/DomainData/Context/BBBCharacterPhysicalPresentationUpdateContext.h"
#include "BBBWork/UBBBNexus/Character/Logic/RuntimeData/BBBCharacterRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/Config/BBBCharacterConfig.h"
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "Components/SkeletalMeshComponent.h"
#include "PhysicsEngine/BodyInstance.h"

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
                const float UpperScale = Context.Character.GetCharacterConfig().LethalUpperBodyImpulseScale;
                const FVector HorizontalDir = Velocity.GetSafeNormal2D();
                if (UpperScale > 0.0f && !HorizontalDir.IsNearlyZero())
                {
                    const FVector UpperImpulse = HorizontalDir * (Velocity.Size2D() * UpperScale) + FVector::UpVector * 60.0f;
                    for (const FName BoneName : {TEXT("spine_02"), TEXT("spine_03"), TEXT("head")})
                    {
                        if (const FBodyInstance *Body = Context.Mesh.GetBodyInstance(BoneName); Body && Body->IsInstanceSimulatingPhysics())
                        {
                            Context.Mesh.AddImpulse(UpperImpulse, BoneName, true);
                        }
                    }
                }
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
