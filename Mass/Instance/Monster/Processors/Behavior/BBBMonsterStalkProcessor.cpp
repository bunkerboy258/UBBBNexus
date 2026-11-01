#include "BBBMonsterStalkProcessor.h"

#include "MassCommonFragments.h"
#include "Engine/World.h"
#include "MassExecutionContext.h"
#include "MassActorSubsystem.h"
#include "NavigationSystem.h"
#include "NavigationPath.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Pawn.h"
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
#include "BBBMonsterBehaviorProcessor.h"

UBBBMonsterStalkProcessor::UBBBMonsterStalkProcessor() : MonsterQuery(*this)
{
    bAutoRegisterWithProcessingPhases = true;
    bRequiresGameThreadExecution = true;
    ProcessingPhase = EMassProcessingPhase::PrePhysics;
    ExecutionFlags = static_cast<uint8>(EProcessorExecutionFlags::Server | EProcessorExecutionFlags::Standalone);
    ExecutionOrder.ExecuteInGroup = BBBMassProcessingGroups::Decision;
    ExecutionOrder.ExecuteAfter.Add(UBBBMonsterBehaviorProcessor::StaticClass()->GetFName());
    ExecutionOrder.ExecuteBefore.Add(BBBMassProcessingGroups::Movement);
}

bool UBBBMonsterStalkProcessor::IsObserved(UWorld& World, const FVector& Point, const AActor* Ignored)
{
    int32 Checked = 0;
    for (FConstPlayerControllerIterator It = World.GetPlayerControllerIterator(); It; ++It)
    {
        auto* Controller = It->Get();
        APawn* Pawn = Controller ? Controller->GetPawn() : nullptr;
        if (!IsValid(Pawn) || !Pawn->CanBeDamaged())
        {
            continue;
        }
        if (++Checked > 16)
        {
            return true;
        }
        FVector Eye;
        FRotator View;
        Controller->GetPlayerViewPoint(Eye, View);
        if (FVector::DistSquared(Eye, Point) > FMath::Square(5000.0f))
        {
            continue;
        }
        FCollisionQueryParams Params(SCENE_QUERY_STAT(BBBStalkerVisibility), false);
        Params.AddIgnoredActor(Pawn);
        Params.AddIgnoredActor(Ignored);
        for (float Height : {0.0f, 65.0f})
        {
            const FVector Sample = Point + FVector::UpVector * Height;
            if (FVector::DotProduct(View.Vector(), (Sample - Eye).GetSafeNormal()) < 0.5f)
            {
                continue;
            }
            FHitResult Hit;
            if (!World.LineTraceSingleByChannel(Hit, Eye, Sample, ECC_Visibility, Params))
            {
                return true;
            }
        }
    }
    return false;
}

void UBBBMonsterStalkProcessor::ConfigureQueries(const TSharedRef<FMassEntityManager>&)
{
    MonsterQuery.AddRequirement<FTransformFragment>(EMassFragmentAccess::ReadOnly);
    MonsterQuery.AddRequirement<FBBBMonsterSpecialFragment>(EMassFragmentAccess::ReadWrite);
    MonsterQuery.AddRequirement<FBBBMonsterCombatFragment>(EMassFragmentAccess::ReadWrite);
    MonsterQuery.AddRequirement<FBBBMonsterBehaviorFragment>(EMassFragmentAccess::ReadWrite);
    MonsterQuery.AddRequirement<FBBBMonsterHealthFragment>(EMassFragmentAccess::ReadOnly);
    MonsterQuery.AddRequirement<FBBBMonsterNetworkFragment>(EMassFragmentAccess::ReadOnly);
    MonsterQuery.AddRequirement<FBBBMonsterTargetFragment>(EMassFragmentAccess::ReadOnly);
    MonsterQuery.AddRequirement<FBBBMonsterMobilityFragment>(EMassFragmentAccess::ReadOnly);
    MonsterQuery.AddRequirement<FMassActorFragment>(EMassFragmentAccess::ReadOnly, EMassFragmentPresence::Optional);
}

void UBBBMonsterStalkProcessor::Execute(FMassEntityManager&, FMassExecutionContext& Context)
{
    UWorld* World = Context.GetWorld();
    auto* Nav = World ? FNavigationSystem::GetCurrent<UNavigationSystemV1>(World) : nullptr;
    if (!World || !Nav)
    {
        return;
    }
    const float Now = World->GetTimeSeconds();
    int32 CandidateBudget = 4;
    const double BudgetStartedAt = FPlatformTime::Seconds();
    MonsterQuery.ForEachEntityChunk(Context, [&](FMassExecutionContext& Chunk)
    {
        auto Specials = Chunk.GetMutableFragmentView<FBBBMonsterSpecialFragment>();
        auto Combats = Chunk.GetMutableFragmentView<FBBBMonsterCombatFragment>();
        auto States = Chunk.GetMutableFragmentView<FBBBMonsterBehaviorFragment>();
        const auto Positions = Chunk.GetFragmentView<FTransformFragment>();
        const auto Rules = Chunk.GetFragmentView<FBBBMonsterNetworkFragment>();
        const auto Targets = Chunk.GetFragmentView<FBBBMonsterTargetFragment>();
        const auto Health = Chunk.GetFragmentView<FBBBMonsterHealthFragment>();
        const auto Injury = Chunk.GetFragmentView<FBBBMonsterMobilityFragment>();
        const auto Actors = Chunk.GetFragmentView<FMassActorFragment>();
        for (int32 I = 0; I < Specials.Num(); ++I)
        {
            const auto* Definition = Rules[I].Definition.Get();
            const auto* Rule = Definition ? Definition->Special.Get() : nullptr;
            if (!Rule || Rule->Kind != EBBBMonsterSpecialKind::Stalker)
            {
                continue;
            }
            auto& Special = Specials[I];
            auto& Fact = Special.Fact;
            const auto& Target = Targets[I];
            const FVector Location = Positions[I].GetTransform().GetLocation();
            if (Health[I].CurrentHealth <= 0 || !Target.bHasTarget || !Target.TargetActor.IsValid()
                || Injury[I].bCrawling || Injury[I].IsStaggering(Now))
            {
                Fact.Phase = 0;
                continue;
            }
            const auto* Actor = !Actors.IsEmpty() ? Actors[I].Get() : nullptr;
            const bool bClose = FVector::DistSquared2D(Location, Target.TargetLocation) <= FMath::Square(Rule->AmbushRange);
            if (Fact.Phase == 6 && Now >= Fact.StartedAt + Fact.Duration)
            {
                Fact.Phase = 0;
                Special.ReadyAt = Now + Rule->Cooldown;
            }
            if (Fact.Phase == 6)
            {
                Combats[I].NextAttackTime = FMath::Min(Combats[I].NextAttackTime,
                    States[I].StateEnteredTime + FMath::Max(Definition->AttackCooldown * Rule->FrenzyCooldownScale,
                        Definition->AttackWindup + Definition->AttackRecovery));
                continue;
            }
            if (Now < Special.ReadyAt)
            {
                continue;
            }
            bool bCanAmbush = bClose && Target.bTargetVisible;
            if (bClose && Fact.Phase == 5 && !bCanAmbush)
            {
                FCollisionQueryParams Params(SCENE_QUERY_STAT(BBBStalkerAmbush), false);
                Params.AddIgnoredActor(Actor);
                Params.AddIgnoredActor(Target.TargetActor.Get());
                FHitResult Obstruction;
                bCanAmbush = !World->LineTraceSingleByChannel(Obstruction,
                    Location + FVector::UpVector * Definition->CapsuleHalfHeight * .7f,
                    Target.TargetActor->GetActorLocation(), ECC_Visibility, Params);
            }
            if (bCanAmbush)
            {
                ++Fact.Serial;
                Fact.Phase = 6;
                Fact.StartedAt = Now;
                Fact.Duration = Rule->FrenzyDuration;
                UE_LOG(LogTemp, Verbose, TEXT("[BBBStalker] 突袭 Entity=%d Serial=%u"), Chunk.GetEntity(I).Index, Fact.Serial);
                continue;
            }
            if (Now < Special.NextStalkCheck || CandidateBudget <= 0 || FPlatformTime::Seconds() - BudgetStartedAt >= 0.0005)
            {
                continue;
            }
            Special.NextStalkCheck = Now + Rule->StalkInterval;
            float BestScore = MAX_flt;
            FVector Best = FVector::ZeroVector;
            const FRotator View = Target.TargetActor->GetActorRotation();
            for (int32 Candidate = 0; Candidate < 8 && CandidateBudget > 0
                && FPlatformTime::Seconds() - BudgetStartedAt < 0.0005; ++Candidate)
            {
                --CandidateBudget;
                const uint8 DirectionIndex = Special.StalkCandidateOffset++ % 8;
                const FVector Direction = View.RotateVector(FVector::ForwardVector).RotateAngleAxis(90.0f + DirectionIndex * 25.0f, FVector::UpVector);
                FNavLocation Projected;
                const FVector Point = Target.TargetLocation + Direction * Rule->StalkRadius;
                if (!Nav->ProjectPointToNavigation(Point, Projected, FVector(100, 100, 250)))
                {
                    continue;
                }
                const FVector Goal = Projected.Location + FVector::UpVector * Definition->CapsuleHalfHeight;
                if (IsObserved(*World, Goal, Actor))
                {
                    continue;
                }
                UNavigationPath* Path = Nav->FindPathToLocationSynchronously(World,
                    Location - FVector::UpVector * Definition->CapsuleHalfHeight, Projected.Location);
                if (!Path || !Path->IsValid() || Path->IsPartial())
                {
                    continue;
                }
                bool bExposedRoute = false;
                const int32 SegmentCount = FMath::Min(Path->PathPoints.Num() - 1, 3);
                for (int32 Segment = 1; Segment <= SegmentCount; ++Segment)
                {
                    const int32 PointIndex = Segment * (Path->PathPoints.Num() - 1) / SegmentCount;
                    const FVector Sample = (Path->PathPoints[PointIndex - 1] + Path->PathPoints[PointIndex]) * 0.5f
                        + FVector::UpVector * Definition->CapsuleHalfHeight;
                    if (IsObserved(*World, Sample, Actor))
                    {
                        bExposedRoute = true;
                        break;
                    }
                }
                if (bExposedRoute)
                {
                    continue;
                }
                const float Score = Path->GetPathLength();
                if (Score < BestScore)
                {
                    BestScore = Score;
                    Best = Goal;
                }
            }
            if (BestScore < MAX_flt)
            {
                ++Fact.Serial;
                Fact.Phase = 5;
                Fact.StartedAt = Now;
                Fact.Duration = Rule->StalkInterval;
                Fact.Goal = Best;
                States[I].State = EBBBMonsterBehavior::Chase;
                Combats[I].AttackTarget.Reset();
                Combats[I].bHitAttempted = true;
                Combats[I].bAttackFinished = true;
            }
            if (BestScore == MAX_flt)
            {
                Fact.Phase = 0;
            }
        }
    });
}
