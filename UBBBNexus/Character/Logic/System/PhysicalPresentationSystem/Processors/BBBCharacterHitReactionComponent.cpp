#include "BBBWork/UBBBNexus/Character/Logic/System/PhysicalPresentationSystem/Processors/BBBCharacterHitReactionComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"

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

void UBBBCharacterHitReactionComponent::BindBodyContacts(USkeletalMeshComponent &Body, bool bBind)
{
    if (bBind)
    {
        Body.OnComponentHit.AddUniqueDynamic(this, &UBBBCharacterHitReactionComponent::OnBodyContact);
        Body.SetAllBodiesNotifyRigidBodyCollision(true);
        return;
    }
    Body.OnComponentHit.RemoveDynamic(this, &UBBBCharacterHitReactionComponent::OnBodyContact);
}

void UBBBCharacterHitReactionComponent::OnBodyContact(UPrimitiveComponent *HitComponent, AActor *OtherActor,
    UPrimitiveComponent *OtherComponent, FVector NormalImpulse, const FHitResult &Hit)
{
    auto *Character = Cast<ABBBCharacter>(GetOwner());
    if (!Character || !Character->IsLocallyControlled() || OtherActor == Character)
    {
        return;
    }
    const auto &State = Character->RuntimeData.PhysicalPresentation.ReadPhysicalPresentationState();
    if (State.bFallRagdoll && !State.bFallLanded && State.bRagdoll)
    {
        FBBBCharacterFallContact Contact;
        Contact.Episode = State.RagdollSnapshot.Episode;
        Contact.Hit = Hit;
        Contact.DownSpeed = State.FallBodySpeeds.FindRef(Hit.MyBoneName);
        Character->SubmitInput(FBBBCharacterFallContactLocalControlPacket{{Contact}});
    }
}
