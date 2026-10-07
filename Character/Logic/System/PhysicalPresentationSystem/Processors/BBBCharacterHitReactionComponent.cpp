#include "BBBWork/UBBBNexus/Character/Logic/System/PhysicalPresentationSystem/Processors/BBBCharacterHitReactionComponent.h"
#include "Components/SkeletalMeshComponent.h"

void UBBBCharacterHitReactionComponent::ClearSimulation()
{
    ResetHitReactSystem();
    PendingImpulse = {};
    SmoothedBoneWeights.Reset();
    if (IsValid(Mesh))
    {
        Mesh->SetAllBodiesPhysicsBlendWeight(0.0f);
        Mesh->SetAllBodiesSimulatePhysics(false);
    }
    SetComponentTickEnabled(false);
}
