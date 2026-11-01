#include "BBBWork/UBBBNexus/Character/Logic/System/PhysicalPresentationSystem/Processors/BBBCharacterRagdollCaptureProcessor.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/PhysicalPresentationSystem/DomainData/Context/BBBCharacterPhysicalPresentationUpdateContext.h"
#include "BBBWork/UBBBNexus/Character/Logic/RuntimeData/BBBCharacterRuntimeData.h"
#include "Components/SkeletalMeshComponent.h"
#include "PhysicsEngine/BodyInstance.h"

void FBBBCharacterRagdollCaptureProcessor::Update(FBBBCharacterPhysicalPresentationUpdateContext &Context) const
{
    auto &State = Context.Data.PhysicalPresentation.PhysicalPresentationState;
    const FBodyInstance *Pelvis = State.bRagdoll ? Context.Mesh.GetBodyInstance(TEXT("pelvis")) : nullptr;
    if (!Pelvis)
    {
        return;
    }
    State.PelvisLocation = Pelvis->GetUnrealWorldTransform().GetLocation();
    if (!Context.Data.External.ReadNetworkIdentityState().bIsMirror)
    {
        auto &Snapshot = State.RagdollSnapshot;
        Snapshot.PelvisLocation = State.PelvisLocation;
        Snapshot.Velocity = Pelvis->GetUnrealWorldVelocity();
        ++Snapshot.Revision;
    }
}
