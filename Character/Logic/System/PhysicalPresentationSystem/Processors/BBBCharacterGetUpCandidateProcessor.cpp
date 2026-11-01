#include "BBBWork/UBBBNexus/Character/Logic/System/PhysicalPresentationSystem/Processors/BBBCharacterGetUpCandidateProcessor.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/PhysicalPresentationSystem/DomainData/Context/BBBCharacterPhysicalPresentationUpdateContext.h"
#include "BBBWork/UBBBNexus/Character/Logic/RuntimeData/BBBCharacterRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "PhysicsEngine/BodyInstance.h"
#include "Engine/World.h"

void FBBBCharacterGetUpCandidateProcessor::Update(FBBBCharacterPhysicalPresentationUpdateContext &Context) const
{
    auto &State = Context.Data.PhysicalPresentation.PhysicalPresentationState;
    State.bCanGetUp = false;
    const auto &Life = Context.Data.Life.ReadLifeState();
    if (!State.bRagdoll || Life.Phase == EBBBCharacterLifePhase::Dead
        || Context.Data.External.ReadNetworkIdentityState().bIsMirror
        || Context.Data.External.ReadWorldState().WorldTimeSeconds < State.ImpactReleaseAt)
    {
        return;
    }
    FBodyInstance *Body = Context.Mesh.GetBodyInstance(TEXT("pelvis"));
    UWorld *World = Context.Character.GetWorld();
    if (!Body || !World || Body->GetUnrealWorldVelocity().SizeSquared() > FMath::Square(80.0f)
        || Body->GetUnrealWorldAngularVelocityInRadians().SizeSquared() > FMath::Square(PI / 2.0f))
    {
        return;
    }
    FCollisionQueryParams Query(SCENE_QUERY_STAT(BBBGetUpSpace), false, &Context.Character);
    TArray<AActor *> Attached;
    Context.Character.GetAttachedActors(Attached, true, true);
    Query.AddIgnoredActors(Attached);
    const auto &Config = Context.Character.GetCharacterConfig().Locomotion;
    const auto *Capsule = Context.Character.GetCapsuleComponent();
    const float Scale = Capsule->GetShapeScale();
    const FVector Pelvis = Body->GetUnrealWorldTransform().GetLocation();
    FHitResult Ground;
    if (!World->LineTraceSingleByChannel(Ground, Pelvis + FVector(0, 0, 20), Pelvis - FVector(0, 0, 150),
        Capsule->GetCollisionObjectType(), Query, FCollisionResponseParams(Capsule->GetCollisionResponseToChannels()))
        || Ground.ImpactNormal.Z < FMath::Cos(FMath::DegreesToRadians(Config.WalkableFloorAngle)))
    {
        return;
    }
    const auto Fits = [&](float HalfHeight)
    {
        const FVector Center = Ground.ImpactPoint + FVector(0, 0, HalfHeight * Scale + 2.0f);
        const FCollisionShape Shape = FCollisionShape::MakeCapsule(Config.CapsuleRadius * Scale, HalfHeight * Scale);
        if (World->OverlapBlockingTestByChannel(Center, FQuat::Identity, Capsule->GetCollisionObjectType(),
            Shape, Query, FCollisionResponseParams(Capsule->GetCollisionResponseToChannels())))
        {
            return false;
        }
        State.GetUpTransform = FTransform(FRotator(0, Context.Character.GetActorRotation().Yaw, 0), Center,
            Context.Character.GetActorScale3D());
        return true;
    };
    State.bCanGetUp = Fits(Config.CapsuleHalfHeight);
}
