#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Processors/Movement/BBBMonsterNavigationProcessor.h"
#include "BBBWork/UBBBNexus/Mass/Core/BBBMassProcessingGroups.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Behavior/BBBMonsterBehaviorFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Movement/BBBMonsterMovementFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Movement/BBBMonsterNavigationFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Perception/BBBMonsterTargetFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Tags/BBBMonsterTag.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Processors/Behavior/BBBMonsterBehaviorProcessor.h"
#include "MassCommonFragments.h"
#include "MassExecutionContext.h"
#include "NavigationSystem.h"
#include "NavMesh/NavMeshPath.h"
#include "NavMesh/RecastNavMesh.h"
#include "Engine/World.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Config/BBBMonsterDefinition.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Network/BBBMonsterNetworkFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Movement/BBBMonsterGroundFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Movement/BBBMonsterMobilityFragment.h"
#include "MassActorSubsystem.h"

UBBBMonsterNavigationProcessor::UBBBMonsterNavigationProcessor()
    : MonsterQuery(*this)
{
    bAutoRegisterWithProcessingPhases = true;
    ProcessingPhase = EMassProcessingPhase::PrePhysics;
    ExecutionOrder.ExecuteInGroup = BBBMassProcessingGroups::Movement;
    ExecutionOrder.ExecuteAfter.Add(BBBMassProcessingGroups::Decision);
    ExecutionOrder.ExecuteAfter.Add(UBBBMonsterBehaviorProcessor::StaticClass()->GetFName());
    bRequiresGameThreadExecution = true;
    ExecutionFlags = static_cast<uint8>(EProcessorExecutionFlags::Server | EProcessorExecutionFlags::Standalone);
}

void UBBBMonsterNavigationProcessor::ConfigureQueries(const TSharedRef<FMassEntityManager>& EntityManager)
{
    MonsterQuery.AddRequirement<FTransformFragment>(EMassFragmentAccess::ReadOnly);
    MonsterQuery.AddRequirement<FBBBMonsterTargetFragment>(EMassFragmentAccess::ReadOnly);
    MonsterQuery.AddRequirement<FBBBMonsterMovementFragment>(EMassFragmentAccess::ReadOnly);
    MonsterQuery.AddRequirement<FBBBMonsterNavigationFragment>(EMassFragmentAccess::ReadWrite);
    MonsterQuery.AddRequirement<FBBBMonsterBehaviorFragment>(EMassFragmentAccess::ReadOnly);
    MonsterQuery.AddRequirement<FBBBMonsterNetworkFragment>(EMassFragmentAccess::ReadOnly);
    MonsterQuery.AddRequirement<FBBBMonsterGroundFragment>(EMassFragmentAccess::ReadOnly);
    MonsterQuery.AddRequirement<FBBBMonsterMobilityFragment>(EMassFragmentAccess::ReadOnly);
    MonsterQuery.AddRequirement<FMassActorFragment>(EMassFragmentAccess::ReadOnly, EMassFragmentPresence::Optional);
    MonsterQuery.AddTagRequirement<FBBBMonsterTag>(EMassFragmentPresence::All);
}

void UBBBMonsterNavigationProcessor::Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context)
{
    UWorld* World = Context.GetWorld();
    UNavigationSystemV1* NavigationSystem = World ? FNavigationSystem::GetCurrent<UNavigationSystemV1>(World) : nullptr;
    if (!ensureMsgf(World && NavigationSystem, TEXT("[UBBBM]Navigation requires a world and navigation system")))
    {
        return;
    }
    if (World->GetNetMode() == NM_Client)
    {
        return;
    }

    const float WorldTime = World->GetTimeSeconds();
    const int32 EntityCount = MonsterQuery.GetNumMatchingEntities();
    if (EntityCount == 0)
    {
        return;
    }

    constexpr int32 PathQueriesPerFrame = 32;
    const int32 FirstQueryIndex = NextPathQueryIndex % EntityCount;
    NextPathQueryIndex = (FirstQueryIndex + PathQueriesPerFrame) % EntityCount;
    int32 EntityIndex = 0;

    TMap<TWeakObjectPtr<AActor>, TSet<int32>> OccupiedSlots;
    MonsterQuery.ForEachEntityChunk(Context, [&OccupiedSlots](FMassExecutionContext& Chunk)
    {
        const auto Paths = Chunk.GetFragmentView<FBBBMonsterNavigationFragment>();
        const auto Targets = Chunk.GetFragmentView<FBBBMonsterTargetFragment>();
        const auto States = Chunk.GetFragmentView<FBBBMonsterBehaviorFragment>();
        for (int32 Index = 0; Index < Chunk.GetNumEntities(); ++Index)
        {
            if (Targets[Index].bTargetVisible && Paths[Index].ApproachActor == Targets[Index].TargetActor &&
                Paths[Index].ApproachSlot != INDEX_NONE && States[Index].State != EBBBMonsterBehavior::Dead)
            {
                OccupiedSlots.FindOrAdd(Targets[Index].TargetActor).Add(Paths[Index].ApproachSlot);
            }
        }
    });

    MonsterQuery.ForEachEntityChunk(Context, [World, WorldTime, NavigationSystem, EntityCount, FirstQueryIndex, &EntityIndex, &OccupiedSlots](FMassExecutionContext& Chunk)
    {
        const auto Transforms = Chunk.GetFragmentView<FTransformFragment>();
        const auto Targets = Chunk.GetFragmentView<FBBBMonsterTargetFragment>();
        const auto Movements = Chunk.GetFragmentView<FBBBMonsterMovementFragment>();
        const auto States = Chunk.GetFragmentView<FBBBMonsterBehaviorFragment>();
        auto Navigation = Chunk.GetMutableFragmentView<FBBBMonsterNavigationFragment>();
        const auto Networks = Chunk.GetFragmentView<FBBBMonsterNetworkFragment>();
        const auto Grounds = Chunk.GetFragmentView<FBBBMonsterGroundFragment>();
        const auto Mobility = Chunk.GetFragmentView<FBBBMonsterMobilityFragment>();
        const auto Actors = Chunk.GetFragmentView<FMassActorFragment>();
        for (int32 Index = 0; Index < Chunk.GetNumEntities(); ++Index)
        {
            const bool bMayQuery = ((EntityIndex++ - FirstQueryIndex + EntityCount) % EntityCount) < PathQueriesPerFrame;
            const FBBBMonsterBehaviorFragment& State = States[Index];
            FBBBMonsterNavigationFragment& Path = Navigation[Index];
            const bool bPatrol = State.State == EBBBMonsterBehavior::Patrol;
            const bool bChase = State.State == EBBBMonsterBehavior::Chase;
            const FVector Location = Transforms[Index].GetTransform().GetLocation();
            const auto* Settings = Networks[Index].Definition.Get();
            if (!ensureMsgf(Settings, TEXT("[BBBNavigation]Missing monster definition")))
            {
                continue;
            }
            const auto& Target = Targets[Index];
            const auto ExcludeGoal = [&Path, &Target, Settings, WorldTime, Index, &Chunk]()
            {
                Path.ExcludedPosition = Target.TargetLocation;
                Path.ExcludedUntil = WorldTime + Settings->NavigationRetryDuration;
                Path.bHasPath = false;
                Path.DetourEndsAt = 0.0f;
                UE_LOG(LogTemp, Verbose, TEXT("[BBBNavigation]Excluded Entity=%d Failures=%d Until=%.2f"),
                    Chunk.GetEntity(Index).Index, Path.FailureCount, Path.ExcludedUntil);
            };
            if (Path.TargetRevision != Target.Revision)
            {
                Path.TargetRevision = Target.Revision;
                Path.FailureCount = 0;
                Path.ProgressTime = WorldTime;
                Path.ProgressPosition = Location;
                Path.NextPathRefreshTime = 0.0f;
                Path.bHasPath = false;
                Path.bReachedDestination = false;
                Path.DetourEndsAt = 0.0f;
                if (Path.ApproachActor != Target.TargetActor || !Target.bTargetVisible)
                {
                    Path.ApproachSlot = INDEX_NONE;
                    Path.ApproachActor.Reset();
                }
            }

            if (Path.ActionId != State.ActionId)
            {
                Path.PathPoints.Reset();
                Path.TailDistances.Reset();
                Path.bHasPath = false;
                Path.bReachedDestination = false;
                Path.PathPointIndex = 1;
                Path.NextPathRefreshTime = 0.0f;
                Path.ActionId = State.ActionId;
            }
            if (!bPatrol && !bChase)
            {
                Path.bHasPath = false;
                continue;
            }
            if (bChase && !Target.bHasTarget)
            {
                Path.bHasPath = false;
                continue;
            }

            if (bChase && Path.ExcludedUntil > WorldTime && FVector::DistSquared(Path.ExcludedPosition, Target.TargetLocation) < FMath::Square(200.0f))
            {
                Path.bHasPath = false;
                continue;
            }
            const bool bPostureChange = Mobility[Index].bCrawling && WorldTime < Mobility[Index].CrawlStartedAt + Settings->CrawlTransitionDuration;
            const bool bPaused = !Grounds[Index].bGrounded || Mobility[Index].IsStaggering(WorldTime) ||
                WorldTime < Mobility[Index].HitStopEndsAt || bPostureChange || Path.bWaitingForSpace || !Path.bHasPath;
            if (bPaused || Path.ProgressTime < 0.0f || FVector::DistSquared2D(Location, Path.ProgressPosition) > FMath::Square(35.0f))
            {
                if (!bPaused && WorldTime >= Path.DetourEndsAt && bChase &&
                    FVector::Dist2D(Location, Target.TargetLocation) + 15.0f < FVector::Dist2D(Path.ProgressPosition, Target.TargetLocation))
                {
                    Path.FailureCount = 0;
                }
                Path.ProgressTime = WorldTime;
                Path.ProgressPosition = Location;
            }
            if (!bPaused && WorldTime - Path.ProgressTime >= Settings->NavigationStuckDuration)
            {
                ++Path.FailureCount;
                Path.ProgressTime = WorldTime;
                Path.NextPathRefreshTime = 0.0f;
                Path.bHasPath = false;
                UE_LOG(LogTemp, Verbose, TEXT("[BBBNavigation]Stuck Entity=%d Attempt=%d"), Chunk.GetEntity(Index).Index, Path.FailureCount);
                if (bChase && Path.FailureCount >= Settings->NavigationFailureLimit)
                {
                    ExcludeGoal();
                    continue;
                }
                if (Path.FailureCount >= 2)
                {
                    const FVector Direction = (bChase ? Target.TargetLocation - Location : Path.Destination - Location).GetSafeNormal2D();
                    const float Side = Chunk.GetEntity(Index).Index % 2 == 0 ? 1.0f : -1.0f;
                    Path.DetourPosition = Location + FVector(-Direction.Y, Direction.X, 0.0f) * Side * 180.0f;
                    Path.DetourEndsAt = WorldTime + 1.0f;
                }
            }

            while (Path.bHasPath && Path.PathPointIndex < Path.PathPoints.Num() - 1 &&
                FVector::DistSquared2D(Location, Path.PathPoints[Path.PathPointIndex]) <= FMath::Square(15.0f))
            {
                const FVector NavigationLocation(Location.X, Location.Y, Path.PathPoints[Path.PathPointIndex].Z);
                FVector Hit;
                if (UNavigationSystemV1::NavigationRaycast(World, NavigationLocation, Path.PathPoints[Path.PathPointIndex + 1], Hit))
                {
                    break;
                }
                ++Path.PathPointIndex;
            }
            if (Path.bHasPath &&
                FVector::DistSquared2D(Location, Path.Destination) <= FMath::Square(15.0f))
            {
                Path.bHasPath = false;
                Path.bReachedDestination = bPatrol || (!Target.bTargetVisible && !Path.bPartialPath && Path.DetourEndsAt <= WorldTime);
                if (Path.DetourEndsAt > WorldTime)
                {
                    Path.DetourEndsAt = 0.0f;
                    Path.NextPathRefreshTime = 0.0f;
                }
                if (bChase && Path.bPartialPath)
                {
                    ++Path.FailureCount;
                    Path.NextPathRefreshTime = WorldTime + 0.5f;
                    if (Path.FailureCount >= Settings->NavigationFailureLimit)
                    {
                        ExcludeGoal();
                    }
                }
                continue;
            }
            if (!bMayQuery || WorldTime < Path.NextPathRefreshTime || (bPatrol && Path.bHasPath))
            {
                continue;
            }

            FVector Goal = Target.TargetLocation;
            Path.bWaitingForSpace = false;
            const bool bEncircle = bChase && Target.bTargetVisible && Target.TargetActor.IsValid() &&
                FVector::DistSquared(Location, Target.TargetLocation) < FMath::Square(Settings->EncircleRange) &&
                FMath::Abs(Location.Z - Target.TargetLocation.Z) <= Settings->MaxStepHeight + 50.0f;
            if (!bEncircle)
            {
                if (Path.ApproachActor.IsValid() && Path.ApproachSlot != INDEX_NONE)
                {
                    OccupiedSlots.FindOrAdd(Path.ApproachActor).Remove(Path.ApproachSlot);
                }
                Path.ApproachSlot = INDEX_NONE;
                Path.ApproachActor.Reset();
            }
            if (bEncircle)
            {
                auto& Slots = OccupiedSlots.FindOrAdd(Target.TargetActor);
                FCollisionQueryParams Params(SCENE_QUERY_STAT(BBBMonsterApproach), false);
                Params.AddIgnoredActor(Target.TargetActor.Get());
                if (!Actors.IsEmpty() && Actors[Index].Get())
                {
                    Params.AddIgnoredActor(Actors[Index].Get());
                }
                float BestCost = FLT_MAX;
                int32 BestSlot = INDEX_NONE;
                int32 ValidSlots = 0;
                FVector BestPoint = Goal;
                for (int32 Slot = 0; Slot < 8; ++Slot)
                {
                    const float Angle = Slot * UE_TWO_PI / 8.0f;
                    const FVector Candidate = Target.TargetLocation + FVector(FMath::Cos(Angle), FMath::Sin(Angle), 0.0f) * Settings->AttackRange * 0.8f;
                    FNavLocation ProjectedSlot;
                    FHitResult Blocker;
                    const FVector Foot(0.0f, 0.0f, Settings->CapsuleHalfHeight);
                    if (!NavigationSystem->ProjectPointToNavigation(Candidate - Foot, ProjectedSlot, FVector(35.0f, 35.0f, 60.0f)) ||
                        World->LineTraceSingleByChannel(Blocker, ProjectedSlot.Location + Foot, Target.TargetLocation, ECC_Pawn, Params))
                    {
                        continue;
                    }
                    FVector Boundary;
                    if (UNavigationSystemV1::NavigationRaycast(World, Target.TargetLocation - Foot, ProjectedSlot.Location, Boundary))
                    {
                        continue;
                    }
                    ++ValidSlots;
                    if (Slots.Contains(Slot) && Slot != Path.ApproachSlot)
                    {
                        continue;
                    }
                    const float Cost = Slot == Path.ApproachSlot ? -1.0f : FVector::DistSquared2D(Location, ProjectedSlot.Location);
                    if (Cost < BestCost)
                    {
                        BestCost = Cost;
                        BestSlot = Slot;
                        BestPoint = ProjectedSlot.Location;
                    }
                }
                if (Path.ApproachSlot != INDEX_NONE)
                {
                    Slots.Remove(Path.ApproachSlot);
                }
                Path.ApproachSlot = BestSlot;
                Path.ApproachActor = Target.TargetActor;
                if (BestSlot == INDEX_NONE)
                {
                    Path.bWaitingForSpace = ValidSlots > 0;
                    Path.bHasPath = false;
                    Path.ProgressTime = WorldTime;
                    Path.NextPathRefreshTime = WorldTime + 0.25f;
                    if (!Path.bWaitingForSpace)
                    {
                        ++Path.FailureCount;
                        if (Path.FailureCount >= Settings->NavigationFailureLimit)
                        {
                            ExcludeGoal();
                        }
                    }
                    continue;
                }
                Slots.Add(BestSlot);
                Goal = BestPoint;
            }
            if (WorldTime < Path.DetourEndsAt)
            {
                Goal = Path.DetourPosition;
            }
            if (bPatrol)
            {
                // 每次巡逻使用独立方向 失败后的重试也换方向
                const FMassEntityHandle Entity = Chunk.GetEntity(Index);
                FRandomStream Random(static_cast<int32>(HashCombine(GetTypeHash(Entity.Index),
                    HashCombine(State.ActionId, GetTypeHash(WorldTime)))));
                const float Angle = Random.FRandRange(0.0f, UE_TWO_PI);
                const float Duration = FMath::Max(State.StateEndsAtTime - WorldTime, 0.1f);
                const float Distance = Movements[Index].WalkSpeed * Duration * 1.25f;
                Goal = Location + FVector(FMath::Cos(Angle), FMath::Sin(Angle), 0.0f) * Distance;
            }

            Path.NextPathRefreshTime = WorldTime + 0.25f;
            FNavLocation Projected;
            const bool bProjected = NavigationSystem->ProjectPointToNavigation(Goal, Projected, FVector(100.0f, 100.0f, 250.0f));
            const ANavigationData* RouteData = NavigationSystem->GetDefaultNavDataInstance();
            if (!RouteData)
            {
                Path.bHasPath = false;
                continue;
            }
            FPathFindingQuery Query(nullptr, *RouteData, Location, bProjected ? Projected.Location : Goal, RouteData->GetDefaultQueryFilter());
            Query.SetAllowPartialPaths(bChase);
            Query.SetRequireNavigableEndLocation(!bChase);
            const FPathFindingResult Result = NavigationSystem->FindPathSync(Query);
            auto NativePath = Result.Path;
            const bool bProjectionMismatch = bChase && !bEncircle && bProjected &&
                FMath::Abs((Goal.Z - Settings->CapsuleHalfHeight) - Projected.Location.Z) > Settings->MaxStepHeight + 25.0f;
            const bool bPartial = NativePath.IsValid() && (NativePath->IsPartial() || !bProjected || bProjectionMismatch);
            const bool bUsefulPartial = NativePath.IsValid() && NativePath->GetPathPoints().Num() >= 2 &&
                FVector::Dist2D(Location, Goal) - FVector::Dist2D(NativePath->GetPathPoints().Last().Location, Goal) >= 50.0f;
            if (!Result.IsSuccessful() || !NativePath.IsValid() || !NativePath->IsValid() || NativePath->GetPathPoints().Num() < 2 ||
                (bPartial && (!bChase || !bUsefulPartial)))
            {
                Path.bHasPath = false;
                Path.PathPoints.Reset();
                Path.TailDistances.Reset();
                ++Path.FailureCount;
                if (bChase && Path.FailureCount >= Settings->NavigationFailureLimit)
                {
                    ExcludeGoal();
                }
                UE_LOG(LogTemp, Verbose, TEXT("[UBBBM]No navigable path Entity=%d"), Chunk.GetEntity(Index).Index);
                continue;
            }

            auto* MeshPath = NativePath.IsValid() ? NativePath->CastPath<FNavMeshPath>() : nullptr;
            const auto* NavData = MeshPath ? Cast<ARecastNavMesh>(MeshPath->GetNavigationDataUsed()) : nullptr;
            if (!ensureMsgf(MeshPath && NavData, TEXT("[UBBBM]Monster route requires a Recast corridor")))
            {
                Path.bHasPath = false;
                continue;
            }
            const float CornerInset = FMath::Max(Movements[Index].CapsuleRadius - NavData->GetConfig().AgentRadius, 0.0f) + 16.0f;
            MeshPath->OffsetFromCorners(CornerInset);
            Path.PathPoints.Reset();
            for (const auto& Point : MeshPath->GetPathPoints())
            {
                Path.PathPoints.Add(Point.Location);
            }
            Path.TailDistances.SetNumZeroed(Path.PathPoints.Num());
            for (int32 Point = Path.PathPoints.Num() - 2; Point >= 0; --Point)
            {
                Path.TailDistances[Point] = Path.TailDistances[Point + 1] +
                    FVector::Dist2D(Path.PathPoints[Point], Path.PathPoints[Point + 1]);
            }
            Path.PathPointIndex = 1;
            Path.Destination = Path.PathPoints.Last();
            Path.bHasPath = true;
            Path.bPartialPath = bPartial;
            Path.bReachedDestination = false;
        }
    });
}
