#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Processors/Movement/BBBMonsterNavigationProcessor.h"
#include "BBBWork/UBBBNexus/Mass/Core/BBBMassNavigationQueryActor.h"
#include "BBBWork/UBBBNexus/Mass/Core/BBBMassProcessingGroups.h"
#include "BBBWork/UBBBNexus/Mass/Core/BBBMassSubsystem.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Config/BBBMonsterDefinition.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Config/BBBMonsterNavigationSettings.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Behavior/BBBMonsterBehaviorFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Movement/BBBMonsterGroundFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Movement/BBBMonsterMobilityFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Movement/BBBMonsterMovementFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Movement/BBBMonsterNavigationFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Network/BBBMonsterNetworkFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Perception/BBBMonsterTargetFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Spawn/BBBMonsterVariationFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Processors/Behavior/BBBMonsterBehaviorProcessor.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Tags/BBBMonsterTag.h"
#include "MassActorSubsystem.h"
#include "MassCommonFragments.h"
#include "MassExecutionContext.h"
#include "NavigationSystem.h"
#include "Navigation/APS_NavigationData.h"
#include "DrawDebugHelpers.h"
#include "Engine/World.h"
#include "HAL/IConsoleManager.h"
#include "HAL/PlatformTime.h"
#include "Stats/Stats.h"

DECLARE_STATS_GROUP(TEXT("BBBMonsterNavigation"), STATGROUP_BBBMonsterNavigation, STATCAT_Advanced);
DECLARE_CYCLE_STAT(TEXT("寻路总耗时"), STAT_BBBNavigationTotal, STATGROUP_BBBMonsterNavigation);
DECLARE_DWORD_COUNTER_STAT(TEXT("待处理请求"), STAT_BBBNavigationPending, STATGROUP_BBBMonsterNavigation);
DECLARE_DWORD_COUNTER_STAT(TEXT("本帧处理请求"), STAT_BBBNavigationSubmitted, STATGROUP_BBBMonsterNavigation);
DECLARE_DWORD_COUNTER_STAT(TEXT("本帧 APS 调用"), STAT_BBBNavigationQueries, STATGROUP_BBBMonsterNavigation);
DECLARE_DWORD_COUNTER_STAT(TEXT("本帧接受路径点"), STAT_BBBNavigationPoints, STATGROUP_BBBMonsterNavigation);
DECLARE_FLOAT_COUNTER_STAT(TEXT("请求处理毫秒"), STAT_BBBNavigationRequestMs, STATGROUP_BBBMonsterNavigation);
DECLARE_FLOAT_COUNTER_STAT(TEXT("最长等待秒数"), STAT_BBBNavigationWait, STATGROUP_BBBMonsterNavigation);

namespace
{
    TAutoConsoleVariable<int32> NavigationDebugEntity(TEXT("bbb.Monster.Navigation.DebugEntity"), -1,
        TEXT("只绘制此实体索引的路线 -1 关闭"), ECVF_Cheat);
    TAutoConsoleVariable<int32> NavigationLogRequests(TEXT("bbb.Monster.Navigation.LogRequests"), 0,
        TEXT("记录 APS 查询耗时和形状参数 0 关闭"), ECVF_Cheat);

    void ClearPath(FBBBMonsterNavigationFragment& Path)
    {
        Path.bHasPath = false;
        Path.NativePath.Reset();
        Path.PathPoints.Reset();
        Path.TailDistances.Reset();
        Path.PathPointIndex = 1;
        Path.RequestSince = -1.0f;
    }

    void ExcludeGoal(FBBBMonsterNavigationFragment& Path, const FBBBMonsterTargetFragment& Target,
        const UBBBMonsterDefinition& Definition, const float WorldTime)
    {
        ClearPath(Path);
        Path.ExcludedPosition = Target.TargetLocation;
        Path.ExcludedUntil = WorldTime + Definition.NavigationRetryDuration;
        Path.RequestSince = -1.0f;
    }
}

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
    MonsterQuery.AddRequirement<FBBBMonsterVariationFragment>(EMassFragmentAccess::ReadOnly);
    MonsterQuery.AddRequirement<FMassActorFragment>(EMassFragmentAccess::ReadOnly, EMassFragmentPresence::Optional);
    MonsterQuery.AddTagRequirement<FBBBMonsterTag>(EMassFragmentPresence::All);
}

void UBBBMonsterNavigationProcessor::Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context)
{
    SCOPE_CYCLE_COUNTER(STAT_BBBNavigationTotal);
    SET_DWORD_STAT(STAT_BBBNavigationPending, 0);
    SET_DWORD_STAT(STAT_BBBNavigationSubmitted, 0);
    SET_DWORD_STAT(STAT_BBBNavigationQueries, 0);
    SET_DWORD_STAT(STAT_BBBNavigationPoints, 0);
    SET_FLOAT_STAT(STAT_BBBNavigationRequestMs, 0.0f);
    SET_FLOAT_STAT(STAT_BBBNavigationWait, 0.0f);

    UWorld* World = Context.GetWorld();
    if (!World || World->GetNetMode() == NM_Client || MonsterQuery.GetNumMatchingEntities() == 0)
    {
        return;
    }
    auto* NavigationSystem = FNavigationSystem::GetCurrent<UNavigationSystemV1>(World);
    auto* RouteData = NavigationSystem ? Cast<AAPS_NavigationData>(NavigationSystem->GetDefaultNavDataInstance()) : nullptr;
    const auto* Budget = GetDefault<UBBBMonsterNavigationSettings>();
    if (!ensureMsgf(RouteData && RouteData->PathAlgorithm == EAPSPathAlgorithm::APS_LowLevelArc && Budget->IsValid(),
        TEXT("[BBBNavigation]需要 APS LowLevelArc 导航数据和有效的寻路预算 请重建 Zombie 导航网格")))
    {
        return;
    }

    // 保存基础多边形走廊以接收动态网格失效通知 不启用逐段重建走廊
    RouteData->bBuildCrowdCompatiblePathCorridor = true;
    RouteData->bRebuildCrowdCorridorAfterArc = false;

    const float WorldTime = World->GetTimeSeconds();
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

    TArray<TPair<FMassEntityHandle, float>> Requests;
    float LongestWait = 0.0f;
    MonsterQuery.ForEachEntityChunk(Context, [World, WorldTime, RouteData, &Requests, &LongestWait, &OccupiedSlots](FMassExecutionContext& Chunk)
    {
        const auto Transforms = Chunk.GetFragmentView<FTransformFragment>();
        const auto Targets = Chunk.GetFragmentView<FBBBMonsterTargetFragment>();
        const auto States = Chunk.GetFragmentView<FBBBMonsterBehaviorFragment>();
        const auto Networks = Chunk.GetFragmentView<FBBBMonsterNetworkFragment>();
        const auto Grounds = Chunk.GetFragmentView<FBBBMonsterGroundFragment>();
        const auto Mobility = Chunk.GetFragmentView<FBBBMonsterMobilityFragment>();
        auto Paths = Chunk.GetMutableFragmentView<FBBBMonsterNavigationFragment>();
        for (int32 Index = 0; Index < Chunk.GetNumEntities(); ++Index)
        {
            auto& Path = Paths[Index];
            const auto& State = States[Index];
            const auto& Target = Targets[Index];
            const auto* Definition = Networks[Index].Definition.Get();
            if (!ensureMsgf(Definition, TEXT("[BBBNavigation]缺少僵尸定义")))
            {
                continue;
            }
            const bool bPatrol = State.State == EBBBMonsterBehavior::Patrol;
            const bool bChase = State.State == EBBBMonsterBehavior::Chase;
            const FVector Location = Transforms[Index].GetTransform().GetLocation();
            const auto ReleaseSlot = [&Path, &OccupiedSlots]()
            {
                if (Path.ApproachActor.IsValid() && Path.ApproachSlot != INDEX_NONE)
                {
                    OccupiedSlots.FindOrAdd(Path.ApproachActor).Remove(Path.ApproachSlot);
                }
                Path.ApproachActor.Reset();
                Path.ApproachSlot = INDEX_NONE;
            };
            if (Path.TargetRevision != Target.Revision || Path.ActionId != State.ActionId)
            {
                ClearPath(Path);
                Path.TargetRevision = Target.Revision;
                Path.ActionId = State.ActionId;
                Path.FailureCount = 0;
                Path.ProgressTime = WorldTime;
                Path.ProgressPosition = Location;
                Path.NextPathRefreshTime = 0.0f;
                Path.RequestSince = -1.0f;
                Path.bReachedDestination = false;
                Path.bWaitingForSpace = false;
            }
            const bool bEncircle = bChase && Target.bTargetVisible && Target.TargetActor.IsValid() &&
                FVector::DistSquared(Location, Target.TargetLocation) < FMath::Square(Definition->EncircleRange) &&
                FMath::Abs(Location.Z - Target.TargetLocation.Z) <= Definition->MaxStepHeight + 50.0f;
            if (!bEncircle || Path.ApproachActor != Target.TargetActor)
            {
                ReleaseSlot();
            }
            if ((!bPatrol && !bChase) || (bChase && !Target.bHasTarget) ||
                (bChase && Path.ExcludedUntil > WorldTime && FVector::DistSquared(Path.ExcludedPosition, Target.TargetLocation) < FMath::Square(200.0f)))
            {
                ClearPath(Path);
                Path.RequestSince = -1.0f;
                Path.bWaitingForSpace = false;
                ReleaseSlot();
                continue;
            }
            if (Path.bHasPath && (!Path.NativePath.IsValid() || !Path.NativePath->IsUpToDate() || !Path.NativePath->IsValid()))
            {
                ClearPath(Path);
                Path.NextPathRefreshTime = 0.0f;
            }

            const bool bPostureChange = Mobility[Index].bCrawling && WorldTime < Mobility[Index].CrawlStartedAt + Definition->CrawlTransitionDuration;
            const bool bMovementSuspended = !Grounds[Index].bGrounded || Mobility[Index].IsStaggering(WorldTime) ||
                WorldTime < Mobility[Index].HitStopEndsAt || bPostureChange;
            const bool bPaused = bMovementSuspended || Path.bWaitingForSpace || !Path.bHasPath;
            if (bPaused || Path.ProgressTime < 0.0f || FVector::DistSquared2D(Location, Path.ProgressPosition) > FMath::Square(35.0f))
            {
                if (!bPaused && bChase &&
                    FVector::Dist2D(Location, Target.TargetLocation) + 15.0f < FVector::Dist2D(Path.ProgressPosition, Target.TargetLocation))
                {
                    Path.FailureCount = 0;
                }
                Path.ProgressTime = WorldTime;
                Path.ProgressPosition = Location;
            }
            if (!bPaused && WorldTime - Path.ProgressTime >= Definition->NavigationStuckDuration)
            {
                ++Path.FailureCount;
                Path.ProgressTime = WorldTime;
                ClearPath(Path);
                Path.NextPathRefreshTime = 0.0f;
                if (bChase && Path.FailureCount >= Definition->NavigationFailureLimit)
                {
                    ExcludeGoal(Path, Target, *Definition, WorldTime);
                    ReleaseSlot();
                    continue;
                }
            }
            while (Path.bHasPath && Path.PathPointIndex < Path.PathPoints.Num() - 1 &&
                FVector::DistSquared2D(Location, Path.PathPoints[Path.PathPointIndex]) <= FMath::Square(15.0f))
            {
                const FVector Start(Location.X, Location.Y, Path.PathPoints[Path.PathPointIndex].Z);
                FVector Hit;
                if (RouteData->Raycast(Start, Path.PathPoints[Path.PathPointIndex + 1], Hit, RouteData->GetDefaultQueryFilter()))
                {
                    break;
                }
                ++Path.PathPointIndex;
            }
            if (Path.bHasPath && FVector::DistSquared2D(Location, Path.Destination) <= FMath::Square(15.0f))
            {
                ClearPath(Path);
                Path.bReachedDestination = bPatrol || (!Target.bTargetVisible && !Path.bPartialPath);
                if (bChase && Path.bPartialPath)
                {
                    ++Path.FailureCount;
                    Path.NextPathRefreshTime = WorldTime + Definition->NavigationNearInterval;
                    if (Path.FailureCount >= Definition->NavigationFailureLimit)
                    {
                        ExcludeGoal(Path, Target, *Definition, WorldTime);
                        ReleaseSlot();
                    }
                }
                continue;
            }

            if (NavigationDebugEntity.GetValueOnGameThread() == Chunk.GetEntity(Index).Index)
            {
                for (int32 Point = 1; Point < Path.PathPoints.Num(); ++Point)
                {
                    DrawDebugLine(World, Path.PathPoints[Point - 1] + FVector(0.0, 0.0, 8.0),
                        Path.PathPoints[Point] + FVector(0.0, 0.0, 8.0), Path.bPartialPath ? FColor::Orange : FColor::Cyan);
                }
            }
            const float Distance = FVector::Dist2D(Location, Target.TargetLocation);
            const bool bFar = Distance >= Definition->NavigationFarDistance;
            const float MoveThreshold = bFar ? Definition->NavigationFarGoalThreshold : Definition->NavigationNearGoalThreshold;
            const bool bNeedsPath = !Path.bHasPath || (bChase &&
                (FVector::DistSquared(Target.TargetLocation, Path.RequestedGoal) >= FMath::Square(MoveThreshold) ||
                    WorldTime - Path.LastPathTime >= Definition->NavigationMaxPathAge || (bEncircle && Path.ApproachSlot == INDEX_NONE)));
            if (!bNeedsPath || bMovementSuspended || Path.bReachedDestination || WorldTime < Path.NextPathRefreshTime)
            {
                Path.RequestSince = -1.0f;
                continue;
            }
            if (Path.RequestSince < 0.0f)
            {
                Path.RequestSince = WorldTime;
            }
            const float Wait = FMath::Max(WorldTime - Path.RequestSince, 0.0f);
            LongestWait = FMath::Max(LongestWait, Wait);
            Requests.Emplace(Chunk.GetEntity(Index), Wait + (Path.bHasPath ? 0.0f : 2.0f));
        }
    });

    SET_DWORD_STAT(STAT_BBBNavigationPending, Requests.Num());
    SET_FLOAT_STAT(STAT_BBBNavigationWait, LongestWait);
    if (Requests.IsEmpty())
    {
        return;
    }
    Requests.Sort([](const auto& Left, const auto& Right)
    {
        return Left.Value == Right.Value ? Left.Key.Index < Right.Key.Index : Left.Value > Right.Value;
    });
    auto* Mass = World->GetSubsystem<UBBBMassSubsystem>();
    auto* QueryOwner = Mass ? Mass->GetNavigationQueryActor() : nullptr;
    if (!ensureMsgf(QueryOwner, TEXT("[BBBNavigation]无法创建世界级 APS 查询载体")))
    {
        return;
    }

    const double RequestStart = FPlatformTime::Seconds();
    int32 Submitted = 0;
    int32 QueryCount = 0;
    int32 AcceptedPoints = 0;
    for (const auto& Request : Requests)
    {
        if (Submitted >= Budget->MaxRequestsPerFrame ||
            (Submitted > 0 && (FPlatformTime::Seconds() - RequestStart) * 1000.0 >= Budget->TimeBudgetMs))
        {
            break;
        }
        ++Submitted;
        const FMassEntityHandle Entity = Request.Key;
        auto& Path = EntityManager.GetFragmentDataChecked<FBBBMonsterNavigationFragment>(Entity);
        const auto& Target = EntityManager.GetFragmentDataChecked<FBBBMonsterTargetFragment>(Entity);
        const auto& State = EntityManager.GetFragmentDataChecked<FBBBMonsterBehaviorFragment>(Entity);
        const auto& Movement = EntityManager.GetFragmentDataChecked<FBBBMonsterMovementFragment>(Entity);
        const auto& Ground = EntityManager.GetFragmentDataChecked<FBBBMonsterGroundFragment>(Entity);
        const auto& Variation = EntityManager.GetFragmentDataChecked<FBBBMonsterVariationFragment>(Entity);
        const auto& Definition = *EntityManager.GetFragmentDataChecked<FBBBMonsterNetworkFragment>(Entity).Definition;
        const FVector Location = EntityManager.GetFragmentDataChecked<FTransformFragment>(Entity).GetTransform().GetLocation();
        const bool bPatrol = State.State == EBBBMonsterBehavior::Patrol;
        const bool bChase = State.State == EBBBMonsterBehavior::Chase;
        const float Distance = FVector::Dist2D(Location, Target.TargetLocation);
        const bool bFar = bChase && Distance >= Definition.NavigationFarDistance;
        const float Interval = bFar ? Definition.NavigationFarInterval : Definition.NavigationNearInterval;
        Path.NextPathRefreshTime = WorldTime + Interval;
        Path.RequestSince = -1.0f;
        Path.bWaitingForSpace = false;
        const auto Fail = [&Path, &Target, &Definition, WorldTime, bChase]()
        {
            ++Path.FailureCount;
            if (bChase && Path.FailureCount >= Definition.NavigationFailureLimit)
            {
                ExcludeGoal(Path, Target, Definition, WorldTime);
            }
        };
        FVector Goal = Target.TargetLocation;
        const bool bEncircle = bChase && Target.bTargetVisible && Target.TargetActor.IsValid() &&
            FVector::DistSquared(Location, Target.TargetLocation) < FMath::Square(Definition.EncircleRange) &&
            FMath::Abs(Location.Z - Target.TargetLocation.Z) <= Definition.MaxStepHeight + 50.0f;
        if (bEncircle)
        {
            auto& Slots = OccupiedSlots.FindOrAdd(Target.TargetActor);
            FCollisionQueryParams Params(SCENE_QUERY_STAT(BBBMonsterApproach), false);
            Params.AddIgnoredActor(Target.TargetActor.Get());
            const auto* Actor = EntityManager.GetFragmentDataPtr<FMassActorFragment>(Entity);
            if (Actor && Actor->Get())
            {
                Params.AddIgnoredActor(Actor->Get());
            }
            float BestCost = FLT_MAX;
            int32 BestSlot = INDEX_NONE;
            int32 ValidSlots = 0;
            for (int32 Slot = 0; Slot < 8; ++Slot)
            {
                const float Angle = Slot * UE_TWO_PI / 8.0f;
                const FVector Candidate = Target.TargetLocation + FVector(FMath::Cos(Angle), FMath::Sin(Angle), 0.0f) * Definition.AttackRange * 0.8f;
                const FVector Foot(0.0f, 0.0f, Definition.CapsuleHalfHeight);
                FNavLocation Projected;
                FHitResult Blocker;
                if (!NavigationSystem->ProjectPointToNavigation(Candidate - Foot, Projected, FVector(35.0f, 35.0f, 60.0f), RouteData) ||
                    World->LineTraceSingleByChannel(Blocker, Projected.Location + Foot, Target.TargetLocation, ECC_Pawn, Params))
                {
                    continue;
                }
                FVector Boundary;
                if (RouteData->Raycast(Target.TargetLocation - Foot, Projected.Location, Boundary, RouteData->GetDefaultQueryFilter()))
                {
                    continue;
                }
                ++ValidSlots;
                if (Slots.Contains(Slot) && Slot != Path.ApproachSlot)
                {
                    continue;
                }
                const float Cost = Slot == Path.ApproachSlot ? -1.0f : FVector::DistSquared2D(Location, Projected.Location);
                if (Cost < BestCost)
                {
                    BestCost = Cost;
                    BestSlot = Slot;
                    Goal = Projected.Location;
                }
            }
            Slots.Remove(Path.ApproachSlot);
            Path.ApproachSlot = BestSlot;
            Path.ApproachActor = Target.TargetActor;
            if (BestSlot == INDEX_NONE)
            {
                Path.bWaitingForSpace = ValidSlots > 0;
                ClearPath(Path);
                Path.ProgressTime = WorldTime;
                if (!Path.bWaitingForSpace)
                {
                    Fail();
                }
                continue;
            }
            Slots.Add(BestSlot);
        }
        if (bPatrol)
        {
            FRandomStream Random(static_cast<int32>(HashCombine(Variation.Seed, HashCombine(State.ActionId, GetTypeHash(Path.FailureCount)))));
            const float Angle = Random.FRandRange(0.0f, UE_TWO_PI);
            const float Duration = FMath::Max(State.StateEndsAtTime - WorldTime, 0.1f);
            Goal = Location + FVector(FMath::Cos(Angle), FMath::Sin(Angle), 0.0f) * Movement.WalkSpeed * Duration * 1.25f;
        }
        const float HalfHeight = Ground.CapsuleHalfHeight > 0.0f ? Ground.CapsuleHalfHeight : Movement.CapsuleHalfHeight;
        FNavLocation Start;
        if (!NavigationSystem->ProjectPointToNavigation(Location - FVector(0.0f, 0.0f, HalfHeight), Start,
            FVector(Movement.CapsuleRadius, Movement.CapsuleRadius, Definition.MaxStepHeight + 25.0f), RouteData))
        {
            ClearPath(Path);
            Fail();
            continue;
        }
        const FVector GoalFoot = bEncircle ? Goal : Goal - FVector(0.0f, 0.0f, bChase ? Definition.CapsuleHalfHeight : HalfHeight);
        FNavLocation End;
        bool bProjected = NavigationSystem->ProjectPointToNavigation(GoalFoot, End, FVector(100.0f, 100.0f, 250.0f), RouteData);
        if (!bProjected && bChase)
        {
            // 目标在网格外时只尝试有限范围内的边界位置 仍按部分路径处理
            bProjected = NavigationSystem->ProjectPointToNavigation(GoalFoot, End, FVector(1000.0f, 1000.0f, 250.0f), RouteData);
        }
        const FVector QueryEnd = bProjected ? End.Location : GoalFoot;
        const auto& Profile = bPatrol ? Definition.PatrolNavigation : Definition.ChaseNavigation;
        Profile.Sample(Variation.Seed, Path.ArcStrength, Path.LaneOffset);
        float ShapeScale = bPatrol ? 1.0f : FMath::Clamp((Distance - Definition.EncircleRange) /
            (Definition.NavigationFullShapeDistance - Definition.EncircleRange), 0.0f, 1.0f);
        if (bFar)
        {
            ShapeScale = 0.0f;
        }
        if (Path.FailureCount > 0)
        {
            ShapeScale *= Path.FailureCount % 2 == 0 ? 0.5f : -0.5f;
        }
        Path.ArcStrength *= ShapeScale;
        Path.LaneOffset *= ShapeScale;
        QueryOwner->SetParameters(Path.ArcStrength, Profile.NumMidPoints, Profile.MidpointMode, Path.LaneOffset);
        FPathFindingQuery Query(QueryOwner, *RouteData, Start.Location, QueryEnd, RouteData->GetDefaultQueryFilter());
        Query.SetAllowPartialPaths(bChase);
        Query.SetRequireNavigableEndLocation(!bChase);
        const double QueryStart = FPlatformTime::Seconds();
        const FPathFindingResult Result = NavigationSystem->FindPathSync(Query);
        Path.LastQueryMs = static_cast<float>((FPlatformTime::Seconds() - QueryStart) * 1000.0);
        QueryOwner->ResetParameters();
        ++QueryCount;
        const auto NativePath = Result.Path;
        if (NativePath.IsValid())
        {
            NativePath->EnableRecalculationOnInvalidation(false);
        }
        const bool bValid = Result.IsSuccessful() && NativePath.IsValid() && NativePath->IsValid() && NativePath->GetPathPoints().Num() >= 2 &&
            !NativePath->GetPathPoints().ContainsByPredicate([](const FNavPathPoint& Point)
            {
                return Point.Location.ContainsNaN();
            });
        const bool bProjectionMismatch = !bProjected || FVector::DistSquared(GoalFoot, QueryEnd) > FMath::Square(Definition.MaxStepHeight + 25.0f);
        // APS 低层模式可能缺少部分路径标记 必须同时核验实际终点
        const bool bPartial = bValid && (NativePath->IsPartial() || bProjectionMismatch ||
            FVector::DistSquared(NativePath->GetPathPoints().Last().Location, QueryEnd) > FMath::Square(15.0f));
        const bool bUsefulPartial = bValid && FVector::Dist2D(Location, Goal) -
            FVector::Dist2D(NativePath->GetPathPoints().Last().Location, Goal) >= 50.0f;
        if (NavigationLogRequests.GetValueOnGameThread() != 0)
        {
            UE_LOG(LogTemp, Log, TEXT("[BBBNavigation]实体=%d APS毫秒=%.3f 弧度=%.3f 偏移=%.1f 有效=%d 部分路径=%d"),
                Entity.Index, Path.LastQueryMs, Path.ArcStrength, Path.LaneOffset, bValid, bPartial);
        }
        if (!bValid || (bPartial && (!bChase || !bUsefulPartial)))
        {
            Fail();
            continue;
        }
        Path.NativePath = NativePath;
        Path.PathPoints.Reset();
        for (const auto& Point : NativePath->GetPathPoints())
        {
            Path.PathPoints.Add(Point.Location);
        }
        AcceptedPoints += Path.PathPoints.Num();
        Path.TailDistances.SetNumZeroed(Path.PathPoints.Num());
        for (int32 Point = Path.PathPoints.Num() - 2; Point >= 0; --Point)
        {
            Path.TailDistances[Point] = Path.TailDistances[Point + 1] + FVector::Dist2D(Path.PathPoints[Point], Path.PathPoints[Point + 1]);
        }
        Path.PathPointIndex = 1;
        Path.Destination = Path.PathPoints.Last();
        Path.RequestedGoal = Target.TargetLocation;
        Path.LastPathTime = WorldTime;
        Path.bHasPath = true;
        Path.bPartialPath = bPartial;
        Path.bReachedDestination = false;
    }
    SET_DWORD_STAT(STAT_BBBNavigationSubmitted, Submitted);
    SET_DWORD_STAT(STAT_BBBNavigationQueries, QueryCount);
    SET_DWORD_STAT(STAT_BBBNavigationPoints, AcceptedPoints);
    SET_FLOAT_STAT(STAT_BBBNavigationRequestMs, static_cast<float>((FPlatformTime::Seconds() - RequestStart) * 1000.0));
}
