#include "BBBWork/UBBBNexus/Character/Logic/System/LifeSystem/Processors/BBBCharacterRescueCandidateProcessor.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/LifeSystem/DomainData/Context/BBBCharacterLifeUpdateContext.h"
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Engine/World.h"
#include "EngineUtils.h"

namespace
{
FVector Feet(const ABBBCharacter &Character)
{
    return Character.GetActorLocation() - FVector(0, 0, Character.GetCapsuleComponent()->GetScaledCapsuleHalfHeight());
}

void IgnoreAttachments(const AActor &Actor, FCollisionQueryParams &Query)
{
    Query.AddIgnoredActor(&Actor);
    TArray<AActor *> Attached;
    Actor.GetAttachedActors(Attached, true, true);
    Query.AddIgnoredActors(Attached);
}
}

void FBBBCharacterRescueCandidateProcessor::Update(FBBBCharacterLifeUpdateContext &Context) const
{
    auto &Candidate = Context.Data.Life.RescueCandidateState;
    Candidate.Target.Reset();
    Candidate.EligiblePartners.Reset();
    Candidate.bCanRecover = false;
    Candidate.bRecoverCrouched = false;
    if (Context.Data.External.ReadNetworkIdentityState().bIsMirror)
    {
        return;
    }
    const auto Phase = Context.Data.Life.ReadLifeState().Phase;
    UWorld *World = Context.Character.GetWorld();
    if (!World || Phase == EBBBCharacterLifePhase::Dead)
    {
        return;
    }
    const UCapsuleComponent *Capsule = Context.Character.GetCapsuleComponent();
    if (Phase == EBBBCharacterLifePhase::Downed)
    {
        FCollisionQueryParams Query(SCENE_QUERY_STAT(BBBRescueRecovery), false);
        IgnoreAttachments(Context.Character, Query);
        const auto &Config = Context.Config.Locomotion;
        const float Scale = Capsule->GetShapeScale();
        const float Radius = FMath::Max(1.0f, Config.CapsuleRadius) * Scale;
        const auto Fits = [&](float HalfHeight)
        {
            const float Height = FMath::Max(Config.CapsuleRadius, HalfHeight) * Scale;
            const FVector Center = Feet(Context.Character) + FVector(0, 0, Height);
            const FCollisionShape Shape = FCollisionShape::MakeCapsule(Radius, FMath::Max(Radius, Height - 0.5f));
            return !World->OverlapBlockingTestByChannel(Center, FQuat::Identity, Capsule->GetCollisionObjectType(),
                Shape, Query, FCollisionResponseParams(Capsule->GetCollisionResponseToChannels()));
        };
        Candidate.bCanRecover = Fits(Config.CapsuleHalfHeight);
        if (!Candidate.bCanRecover)
        {
            Candidate.bCanRecover = Fits(Config.CrouchedHalfHeight);
            Candidate.bRecoverCrouched = Candidate.bCanRecover;
        }
    }
    if (Phase == EBBBCharacterLifePhase::Alive &&
        Context.Data.Traversal.ReadTraversalState().Action != EBBBTraversalAction::None)
    {
        return;
    }
    float Nearest = FMath::Square(FMath::Max(1.0f, Context.Config.RescueDistance));
    for (TActorIterator<ABBBCharacter> It(World); It; ++It)
    {
        ABBBCharacter *Other = *It;
        if (Other == &Context.Character || !IsValid(Other) || !Other->IsPlayerControlled())
        {
            continue;
        }
        const auto OtherPhase = Other->GetLifePhase();
        if ((Phase == EBBBCharacterLifePhase::Alive && OtherPhase != EBBBCharacterLifePhase::Downed) ||
            (Phase == EBBBCharacterLifePhase::Downed && OtherPhase != EBBBCharacterLifePhase::Alive))
        {
            continue;
        }
        const float Distance = FVector::DistSquared(Feet(Context.Character), Feet(*Other));
        if (Distance > FMath::Square(FMath::Max(1.0f, Context.Config.RescueDistance)))
        {
            continue;
        }
        FCollisionQueryParams Query(SCENE_QUERY_STAT(BBBRescueSight), false);
        IgnoreAttachments(Context.Character, Query);
        IgnoreAttachments(*Other, Query);
        FHitResult Hit;
        if (World->LineTraceSingleByChannel(Hit, Context.Character.GetActorLocation(), Other->GetActorLocation(),
            ECC_Visibility, Query))
        {
            continue;
        }
        Candidate.EligiblePartners.Add(Other);
        if (Phase == EBBBCharacterLifePhase::Alive && !Other->IsRescueReceiving() && Distance <= Nearest)
        {
            Candidate.Target = Other;
            Nearest = Distance;
        }
    }
}
