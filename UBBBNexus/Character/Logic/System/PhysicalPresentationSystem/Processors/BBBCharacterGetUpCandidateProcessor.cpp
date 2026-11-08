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
    const double PreviousStableSince = State.FallGetUpStableSince;
    State.FallGetUpStableSince = -1.0;
    const double Now = Context.Data.External.ReadWorldState().WorldTimeSeconds;
    const auto &Life = Context.Data.Life.ReadLifeState();
    if (!State.bRagdoll || Life.Phase == EBBBCharacterLifePhase::Dead
        || Context.Data.External.ReadNetworkIdentityState().bIsMirror
        || Now < State.ImpactReleaseAt || (State.bFallRagdoll && !State.bFallLanded))
    {
        return;
    }
    const auto IsBodyMoving = [](const FBodyInstance &Body)
    {
        return Body.GetUnrealWorldVelocity().SizeSquared() > FMath::Square(80.0f)
            || Body.GetUnrealWorldAngularVelocityInRadians().SizeSquared() > FMath::Square(PI / 2.0f);
    };
    const FBodyInstance *Body = Context.Mesh.GetBodyInstance(TEXT("pelvis"));
    UWorld *World = Context.Character.GetWorld();
    if (!Body || !World || IsBodyMoving(*Body))
    {
        return;
    }
    if (State.bFallRagdoll)
    {
        // 骨盆短暂停住时 躯干仍可能在撞地后翻转
        for (const FName Bone : {FName(TEXT("spine_01")), FName(TEXT("spine_02")), FName(TEXT("spine_03"))})
        {
            const FBodyInstance *Spine = Context.Mesh.GetBodyInstance(Bone);
            if (Spine && IsBodyMoving(*Spine))
            {
                return;
            }
        }
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
    if (!Fits(Config.CapsuleHalfHeight))
    {
        return;
    }
    if (State.bFallRagdoll)
    {
        constexpr double FallGetUpStableSeconds = 0.2;
        State.FallGetUpStableSince = PreviousStableSince >= 0.0 ? PreviousStableSince : Now;
        if (Now - State.FallGetUpStableSince < FallGetUpStableSeconds)
        {
            return;
        }
    }
    State.bCanGetUp = true;
}
