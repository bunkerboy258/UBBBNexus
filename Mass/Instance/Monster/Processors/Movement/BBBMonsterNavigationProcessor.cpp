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
#include "NavigationPath.h"
#include "NavigationSystem.h"
#include "Engine/World.h"

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

    MonsterQuery.ForEachEntityChunk(Context, [World, WorldTime, NavigationSystem, EntityCount, FirstQueryIndex, &EntityIndex](FMassExecutionContext& Chunk)
    {
        const auto Transforms = Chunk.GetFragmentView<FTransformFragment>();
        const auto Targets = Chunk.GetFragmentView<FBBBMonsterTargetFragment>();
        const auto Movements = Chunk.GetFragmentView<FBBBMonsterMovementFragment>();
        const auto States = Chunk.GetFragmentView<FBBBMonsterBehaviorFragment>();
        auto Navigation = Chunk.GetMutableFragmentView<FBBBMonsterNavigationFragment>();
        for (int32 Index = 0; Index < Chunk.GetNumEntities(); ++Index)
        {
            const bool bMayQuery = ((EntityIndex++ - FirstQueryIndex + EntityCount) % EntityCount) < PathQueriesPerFrame;
            const FBBBMonsterBehaviorFragment& State = States[Index];
            FBBBMonsterNavigationFragment& Path = Navigation[Index];
            const bool bPatrol = State.State == EBBBMonsterBehavior::Patrol;
            const bool bChase = State.State == EBBBMonsterBehavior::Chase;
            const FVector Location = Transforms[Index].GetTransform().GetLocation();

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
            if (bChase && (!Targets[Index].bHasTarget || !Targets[Index].TargetActor.IsValid()))
            {
                Path.bHasPath = false;
                continue;
            }

            while (Path.bHasPath && Path.PathPointIndex < Path.PathPoints.Num() - 1 &&
                FVector::DistSquared2D(Location, Path.PathPoints[Path.PathPointIndex]) <= FMath::Square(15.0f))
            {
                ++Path.PathPointIndex;
            }
            if (bPatrol && Path.bHasPath &&
                FVector::DistSquared2D(Location, Path.Destination) <= FMath::Square(15.0f))
            {
                Path.bReachedDestination = true;
                Path.bHasPath = false;
                continue;
            }
            if (!bMayQuery || WorldTime < Path.NextPathRefreshTime || (bPatrol && Path.bHasPath))
            {
                continue;
            }

            FVector Goal = Targets[Index].TargetLocation;
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
            FNavLocation ProjectedStart;
            const bool bProjected = NavigationSystem->ProjectPointToNavigation(Goal, Projected, FVector(100.0f, 100.0f, 250.0f)) &&
                NavigationSystem->ProjectPointToNavigation(Location, ProjectedStart, FVector(100.0f, 100.0f, 250.0f));
            const UNavigationPath* Result = bProjected
                ? NavigationSystem->FindPathToLocationSynchronously(World, ProjectedStart.Location, Projected.Location)
                : nullptr;
            if (Result == nullptr || !Result->IsValid() || Result->IsPartial() || Result->PathPoints.Num() < 2)
            {
                Path.bHasPath = false;
                Path.PathPoints.Reset();
                Path.TailDistances.Reset();
                UE_LOG(LogTemp, Verbose, TEXT("[UBBBM]No navigable path Entity=%d"), Chunk.GetEntity(Index).Index);
                continue;
            }

            Path.PathPoints = Result->PathPoints;
            Path.TailDistances.SetNumZeroed(Path.PathPoints.Num());
            for (int32 Point = Path.PathPoints.Num() - 2; Point >= 0; --Point)
            {
                Path.TailDistances[Point] = Path.TailDistances[Point + 1] +
                    FVector::Dist2D(Path.PathPoints[Point], Path.PathPoints[Point + 1]);
            }
            Path.PathPointIndex = 1;
            Path.Destination = Path.PathPoints.Last();
            Path.bHasPath = true;
        }
    });
}
