#include "BBBMonsterSeveringPresentationComponent.h"

#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"

namespace
{
    TMap<TWeakObjectPtr<UStaticMeshComponent>, float> DetachedParts;
    TMap<TWeakObjectPtr<UWorld>, uint64> LastUpdateFrames;
}

UBBBMonsterSeveringPresentationComponent::UBBBMonsterSeveringPresentationComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UBBBMonsterSeveringPresentationComponent::UpdateDetachedParts()
{
    UWorld* World = GetWorld();
    if (!World || LastUpdateFrames.FindRef(World) == GFrameCounter)
    {
        return;
    }
    LastUpdateFrames.FindOrAdd(World) = GFrameCounter;
    const float Now = World->GetTimeSeconds();
    for (auto It = DetachedParts.CreateIterator(); It; ++It)
    {
        UStaticMeshComponent* Piece = It.Key().Get();
        if (!IsValid(Piece))
        {
            It.RemoveCurrent();
            continue;
        }
        if (Piece->GetWorld() != World)
        {
            continue;
        }
        const float Age = Now - It.Value();
        if (Age >= 8.0f)
        {
            Piece->DestroyComponent();
            It.RemoveCurrent();
            continue;
        }
        if (Age > 1.0f && Piece->IsSimulatingPhysics() && Piece->GetPhysicsLinearVelocity().SizeSquared() < 225.0f)
        {
            Piece->PutAllRigidBodiesToSleep();
            Piece->SetSimulatePhysics(false);
            Piece->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        }
    }
    for (auto It = LastUpdateFrames.CreateIterator(); It; ++It)
    {
        if (!It.Key().IsValid())
        {
            It.RemoveCurrent();
        }
    }
}

void UBBBMonsterSeveringPresentationComponent::ApplyDestroyedParts(const uint8 DestroyedParts,
    const TArray<FBBBMonsterSeveredPartDefinition>& Definitions, const FVector& ImpulseDirection)
{
    auto* Mesh = GetOwner()->FindComponentByClass<USkeletalMeshComponent>();
    if (!Mesh || AppliedParts == DestroyedParts)
    {
        return;
    }
    for (const auto& Definition : Definitions)
    {
        const uint8 Bit = 1u << static_cast<uint8>(Definition.Region);
        if ((DestroyedParts & Bit) == 0 || (AppliedParts & Bit) != 0)
        {
            continue;
        }
        const FName Parent = Mesh->GetParentBone(Definition.Bone);
        if (!ensureMsgf(Definition.DetachedMesh && Definition.CapMesh && !Parent.IsNone() && Mesh->GetBoneIndex(Definition.Bone) != INDEX_NONE,
            TEXT("[BBBSevering]Missing sealed geometry Bone=%s Actor=%s"), *Definition.Bone.ToString(), *GetOwner()->GetName()))
        {
            continue;
        }
        const FTransform BoneWorld = Mesh->GetSocketTransform(Definition.Bone);
        auto* Cap = NewObject<UStaticMeshComponent>(GetOwner());
        Cap->SetStaticMesh(Definition.CapMesh);
        Cap->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        Cap->SetCanEverAffectNavigation(false);
        Cap->SetupAttachment(Mesh, Parent);
        Cap->SetRelativeTransform(BoneWorld.GetRelativeTransform(Mesh->GetSocketTransform(Parent)));
        Cap->RegisterComponent();
        Components.Add(Cap);
        if (DetachedParts.Num() < 24 && !GetOwner()->IsHidden() && Mesh->WasRecentlyRendered(0.2f) && Mesh->GetPredictedLODLevel() <= 1)
        {
            auto* Piece = NewObject<UStaticMeshComponent>(GetOwner());
            Piece->SetStaticMesh(Definition.DetachedMesh);
            Piece->SetWorldTransform(BoneWorld);
            Piece->SetCanEverAffectNavigation(false);
            Piece->SetCollisionObjectType(ECC_PhysicsBody);
            Piece->SetCollisionResponseToAllChannels(ECR_Ignore);
            Piece->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
            Piece->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Block);
            Piece->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
            Piece->SetLinearDamping(1.0f);
            Piece->SetAngularDamping(2.5f);
            Piece->RegisterComponent();
            Piece->SetSimulatePhysics(true);
            Piece->SetEnableGravity(true);
            Piece->SetPhysicsLinearVelocity(ImpulseDirection.GetSafeNormal(UE_SMALL_NUMBER, FVector::ForwardVector) * 200.0f + FVector::UpVector * 90.0f);
            Components.Add(Piece);
            DetachedParts.Add(Piece, GetWorld()->GetTimeSeconds());
        }
        Mesh->HideBoneByName(Definition.Bone, EPhysBodyOp::PBO_Term);
        HiddenBones.AddUnique(Definition.Bone);
        AppliedParts |= Bit;
    }
}

void UBBBMonsterSeveringPresentationComponent::ResetPresentation()
{
    for (UStaticMeshComponent* Component : Components)
    {
        DetachedParts.Remove(Component);
        if (IsValid(Component))
        {
            Component->DestroyComponent();
        }
    }
    Components.Reset();
    auto* Mesh = GetOwner()->FindComponentByClass<USkeletalMeshComponent>();
    if (Mesh && !HiddenBones.IsEmpty())
    {
        for (const FName Bone : HiddenBones)
        {
            Mesh->UnHideBoneByName(Bone);
        }
        Mesh->RecreatePhysicsState();
    }
    HiddenBones.Reset();
    AppliedParts = 0;
}

void UBBBMonsterSeveringPresentationComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    ResetPresentation();
    Super::EndPlay(EndPlayReason);
}
