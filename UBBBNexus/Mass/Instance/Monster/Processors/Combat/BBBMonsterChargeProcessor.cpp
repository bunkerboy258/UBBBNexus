#include "BBBMonsterChargeProcessor.h"

#include "MassCommonFragments.h"
#include "Engine/World.h"
#include "MassMovementFragments.h"
#include "MassExecutionContext.h"
#include "MassActorSubsystem.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"
#include "Components/CapsuleComponent.h"
#include "BBBWork/UBBBNexus/Mass/Core/BBBMassProcessingGroups.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Config/BBBMonsterDefinition.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Config/BBBMonsterSpecialDefinition.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Combat/BBBMonsterSpecialFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Combat/BBBMonsterCombatFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Behavior/BBBMonsterBehaviorFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Health/BBBMonsterHealthFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Network/BBBMonsterNetworkFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Perception/BBBMonsterTargetFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Movement/BBBMonsterMobilityFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Movement/BBBMonsterGroundFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Movement/BBBMonsterTraversalFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Processors/Behavior/BBBMonsterBehaviorProcessor.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Processors/Movement/BBBMonsterLocomotionProcessor.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Processors/Movement/BBBMonsterNavigation.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Movement/BBBMonsterNavigationFragment.h"
#include "BBBWork/UBBBNexus/Combat/BBBHitReceiver.h"

UBBBMonsterChargeProcessor::UBBBMonsterChargeProcessor() : MonsterQuery(*this)
{
    bAutoRegisterWithProcessingPhases = true;
    bRequiresGameThreadExecution = true;
    ProcessingPhase = EMassProcessingPhase::PrePhysics;
    ExecutionFlags = static_cast<uint8>(EProcessorExecutionFlags::Server | EProcessorExecutionFlags::Standalone);
    ExecutionOrder.ExecuteInGroup = BBBMassProcessingGroups::Decision;
    ExecutionOrder.ExecuteAfter.Add(UBBBMonsterBehaviorProcessor::StaticClass()->GetFName());
    ExecutionOrder.ExecuteBefore.Add(BBBMassProcessingGroups::Movement);
}

void UBBBMonsterChargeProcessor::ConfigureQueries(const TSharedRef<FMassEntityManager>&)
{
    MonsterQuery.AddRequirement<FTransformFragment>(EMassFragmentAccess::ReadWrite);
    MonsterQuery.AddRequirement<FMassVelocityFragment>(EMassFragmentAccess::ReadWrite);
    MonsterQuery.AddRequirement<FBBBMonsterSpecialFragment>(EMassFragmentAccess::ReadWrite);
    MonsterQuery.AddRequirement<FBBBMonsterCombatFragment>(EMassFragmentAccess::ReadWrite);
    MonsterQuery.AddRequirement<FBBBMonsterBehaviorFragment>(EMassFragmentAccess::ReadOnly);
    MonsterQuery.AddRequirement<FBBBMonsterHealthFragment>(EMassFragmentAccess::ReadOnly);
    MonsterQuery.AddRequirement<FBBBMonsterNetworkFragment>(EMassFragmentAccess::ReadOnly);
    MonsterQuery.AddRequirement<FBBBMonsterTargetFragment>(EMassFragmentAccess::ReadOnly);
    MonsterQuery.AddRequirement<FBBBMonsterMobilityFragment>(EMassFragmentAccess::ReadOnly);
    MonsterQuery.AddRequirement<FBBBMonsterGroundFragment>(EMassFragmentAccess::ReadWrite);
    MonsterQuery.AddRequirement<FBBBMonsterNavigationFragment>(EMassFragmentAccess::ReadWrite);
    MonsterQuery.AddRequirement<FBBBMonsterTraversalFragment>(EMassFragmentAccess::ReadOnly);
    MonsterQuery.AddRequirement<FMassActorFragment>(EMassFragmentAccess::ReadOnly, EMassFragmentPresence::Optional);
}

void UBBBMonsterChargeProcessor::Execute(FMassEntityManager&, FMassExecutionContext& Context)
{
    UWorld* World = Context.GetWorld();
    if (!World || Context.GetDeltaTimeSeconds() <= 0)
    {
        return;
    }
    const float Now = World->GetTimeSeconds();
    const float Delta = FMath::Min(Context.GetDeltaTimeSeconds(), 0.25f);
    MonsterQuery.ForEachEntityChunk(Context, [&](FMassExecutionContext& Chunk)
    {
        auto Transforms = Chunk.GetMutableFragmentView<FTransformFragment>();
        auto Speeds = Chunk.GetMutableFragmentView<FMassVelocityFragment>();
        auto Specials = Chunk.GetMutableFragmentView<FBBBMonsterSpecialFragment>();
        auto Combat = Chunk.GetMutableFragmentView<FBBBMonsterCombatFragment>();
        auto Ground = Chunk.GetMutableFragmentView<FBBBMonsterGroundFragment>();
        auto Paths = Chunk.GetMutableFragmentView<FBBBMonsterNavigationFragment>();
        const auto Network = Chunk.GetFragmentView<FBBBMonsterNetworkFragment>();
        const auto Targets = Chunk.GetFragmentView<FBBBMonsterTargetFragment>();
        const auto Health = Chunk.GetFragmentView<FBBBMonsterHealthFragment>();
        const auto Injury = Chunk.GetFragmentView<FBBBMonsterMobilityFragment>();
        const auto Traversal = Chunk.GetFragmentView<FBBBMonsterTraversalFragment>();
        const auto Actors = Chunk.GetFragmentView<FMassActorFragment>();
        for (int32 I = 0; I < Specials.Num(); ++I)
        {
            const auto* Definition = Network[I].Definition.Get();
            const auto* Rule = Definition ? Definition->Special.Get() : nullptr;
            if (!Rule || Rule->Kind != EBBBMonsterSpecialKind::Charger)
            {
                continue;
            }
            auto& Special = Specials[I];
            auto& Fact = Special.Fact;
            auto& Transform = Transforms[I].GetMutableTransform();
            const FVector Location = Transform.GetLocation();
            const auto SetPhase = [&](uint8 Phase, float Duration)
            {
                if (Fact.Phase == 2 || Phase == 2)
                {
                    FBBBMonsterNavigation::Reset(Paths[I]);
                    Paths[I].NextPathRefreshTime = Now;
                }
                Fact.Phase = Phase;
                Fact.StartedAt = Now;
                Fact.Duration = Duration;
                UE_LOG(LogTemp, Verbose, TEXT("[BBBCharger] Entity=%d Serial=%u Phase=%u"), Chunk.GetEntity(I).Index, Fact.Serial, Phase);
            };
            if (Health[I].CurrentHealth <= 0 || Injury[I].bCrawling || Injury[I].IsStaggering(Now)
                || Traversal[I].Fact.Kind != 0 || !Ground[I].bGrounded)
            {
                if (Fact.Phase != 0)
                {
                    SetPhase(0, 0);
                    Special.ReadyAt = Now + Rule->Cooldown;
                    Speeds[I].Value = FVector::ZeroVector;
                }
                continue;
            }
            if (Fact.Phase == 0)
            {
                const auto* Target = Targets[I].TargetActor.Get();
                const float Distance = FVector::Dist2D(Location, Targets[I].TargetLocation);
                if (Now < Special.ReadyAt || !Targets[I].bTargetVisible || !IsValid(Target) || !Target->CanBeDamaged()
                    || Distance < Rule->MinimumRange || Distance > Rule->MaximumRange)
                {
                    continue;
                }
                FVector Direction = (Targets[I].TargetLocation - Location).GetSafeNormal2D();
                FCollisionQueryParams Params(SCENE_QUERY_STAT(BBBChargeReady), false);
                Params.AddIgnoredActor(Target);
                if (!Actors.IsEmpty())
                {
                    Params.AddIgnoredActor(Actors[I].Get());
                }
                FHitResult Wall;
                if (World->SweepSingleByChannel(Wall, Location, Location + Direction * Distance, FQuat::Identity,
                    ECC_WorldStatic, FCollisionShape::MakeCapsule(Definition->CollisionRadius, Definition->CapsuleHalfHeight - 2), Params))
                {
                    continue;
                }
                ++Fact.Serial;
                Fact.Goal = Location + Direction * Rule->RushDistance;
                Special.RushTravelled = 0;
                Special.HitPlayerCount = 0;
                for (auto& Player : Special.HitPlayers)
                {
                    Player.Reset();
                }
                Combat[I].AttackTarget.Reset();
                Combat[I].bHitAttempted = true;
                Combat[I].bAttackFinished = true;
                Transform.SetRotation(Direction.ToOrientationQuat());
                Speeds[I].Value = FVector::ZeroVector;
                SetPhase(1, Rule->Windup);
                continue;
            }
            Speeds[I].Value = FVector::ZeroVector;
            if (Fact.Phase == 1 && Now >= Fact.StartedAt + Fact.Duration)
            {
                SetPhase(2, Rule->RushDistance / Rule->RushSpeed);
            }
            if (Fact.Phase == 3)
            {
                if (Now >= Fact.StartedAt + Fact.Duration)
                {
                    SetPhase(0, 0);
                }
                continue;
            }
            if (Fact.Phase != 2)
            {
                continue;
            }
            const FVector Direction = Transform.GetRotation().GetForwardVector().GetSafeNormal2D();
            const float Length = FMath::Min(Rule->RushSpeed * Delta, Rule->RushDistance - Special.RushTravelled);
            FVector End = Location + Direction * FMath::Max(Length, 0.0f);
            FCollisionQueryParams Params(SCENE_QUERY_STAT(BBBChargeMove), false);
            if (!Actors.IsEmpty())
            {
                Params.AddIgnoredActor(Actors[I].Get());
            }
            TArray<APawn*, TInlineAllocator<32>> Players;
            for (FConstPlayerControllerIterator It = World->GetPlayerControllerIterator(); It && Players.Num() < 32; ++It)
            {
                APawn* Pawn = It->IsValid() ? It->Get()->GetPawn() : nullptr;
                if (IsValid(Pawn))
                {
                    Players.Add(Pawn);
                    Params.AddIgnoredActor(Pawn);
                }
            }
            bool bStopped = false;
            const auto Shape = FCollisionShape::MakeCapsule(Definition->CollisionRadius, Definition->CapsuleHalfHeight - 2);
            FHitResult Wall;
            if (World->SweepSingleByChannel(Wall, Location, End, FQuat::Identity, ECC_Pawn, Shape, Params))
            {
                End = FMath::Lerp(Location, End, FMath::Max(0.0f, Wall.Time - 0.01f));
                bStopped = true;
            }
            FHitResult Floor;
            const FVector Feet = End - FVector::UpVector * Definition->CapsuleHalfHeight;
            const bool bFloor = World->LineTraceSingleByChannel(Floor, Feet + FVector::UpVector * Definition->MaxStepHeight,
                Feet - FVector::UpVector * (Definition->MaxStepHeight + 10), ECC_WorldStatic, Params);
            if (!bFloor || Floor.ImpactNormal.Z < FMath::Cos(FMath::DegreesToRadians(Definition->MaxWalkableSlopeAngle)))
            {
                End = Location;
                bStopped = true;
            }
            if (bFloor && !bStopped)
            {
                End.Z = Floor.ImpactPoint.Z + Definition->CapsuleHalfHeight;
            }
            for (APawn* Pawn : Players)
            {
                auto* Capsule = Cast<UPrimitiveComponent>(Pawn->GetRootComponent());
                FHitResult Hit;
                if (Capsule && Pawn->CanBeDamaged() && Capsule->SweepComponent(Hit, Location, End, FQuat::Identity, Shape))
                {
                    if (!MakeArrayView(Special.HitPlayers, Special.HitPlayerCount).Contains(Pawn)
                        && Special.HitPlayerCount < 32 && Pawn->CanBeDamaged())
                    {
                        Special.HitPlayers[Special.HitPlayerCount++] = Pawn;
                        FBBBHitContribution Value{Rule->RushDamage, Hit.BoneName, Hit.ImpactPoint, Direction};
                        Value.SourceId = Network[I].InstanceId;
                        Value.AttackId = FGuid(0, 0, 0, Fact.Serial);
                        Value.AddVelocity = Direction * Rule->ImpactSpeed + FVector::UpVector * Rule->ImpactLift;
                        Value.AddHoldSeconds = Rule->KnockdownDuration;
                        BBBHitReceiver::Submit(Pawn, Value);
                    }
                }
            }
            Special.RushTravelled += FVector::Dist2D(Location, End);
            Transform.SetLocation(End);
            Ground[I].RequestedDistance = Length;
            Ground[I].MovedDistance = FVector::Dist2D(Location, End);
            if (bFloor && !bStopped)
            {
                Ground[I].SupportNormal = Floor.ImpactNormal;
            }
            Speeds[I].Value = (End - Location) / FMath::Max(Delta, 0.001f);
            if (bStopped || Special.RushTravelled >= Rule->RushDistance - 1 || Now >= Fact.StartedAt + Fact.Duration)
            {
                SetPhase(3, Rule->Recovery);
                Special.ReadyAt = Now + Rule->Recovery + Rule->Cooldown;
                Speeds[I].Value = FVector::ZeroVector;
            }
        }
    });
}
