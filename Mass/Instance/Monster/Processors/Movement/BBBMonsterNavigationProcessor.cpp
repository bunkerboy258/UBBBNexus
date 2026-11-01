#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Processors/Movement/BBBMonsterNavigationProcessor.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Combat/BBBMonsterSpecialFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Movement/BBBMonsterTraversalFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Config/BBBMonsterClimbArea.h"

#include "BBBWork/UBBBNexus/Mass/Core/BBBMassProcessingGroups.h"
#include "BBBWork/UBBBNexus/Mass/Core/BBBMassSubsystem.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Config/BBBMonsterDefinition.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Config/BBBMonsterNavigationSettings.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Behavior/BBBMonsterBehaviorFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Movement/BBBMonsterGroundFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Movement/BBBMonsterMobilityFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Movement/BBBMonsterMovementFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Movement/BBBMonsterNavigationFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Movement/BBBMonsterAvoidanceFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Network/BBBMonsterNetworkFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Perception/BBBMonsterTargetFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Spawn/BBBMonsterVariationFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Processors/Behavior/BBBMonsterBehaviorProcessor.h"
#include "BBBMonsterNavigation.h"
#include "MassMovementFragments.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Tags/BBBMonsterTag.h"
#include "MassActorSubsystem.h"
#include "MassCommonFragments.h"
#include "MassExecutionContext.h"
#include "NavigationSystem.h"
#include "NavMesh/RecastNavMesh.h"
#include "DrawDebugHelpers.h"
#include "Engine/World.h"
#include "HAL/IConsoleManager.h"
#include "HAL/PlatformTime.h"
#include "Stats/Stats.h"

#include "NavMesh/RecastHelpers.h"
#include "NavMesh/RecastQueryFilter.h"
#include "NavMesh/NavMeshPath.h"
#include "DetourCrowd/DetourPathCorridor.h"

namespace
{
    TAutoConsoleVariable<int32> DebugEntity(TEXT("bbb.Monster.Navigation.DebugEntity"), -1,
        TEXT("显示指定实体的路线 速度与停止原因 -1关闭"), ECVF_Cheat);
    TAutoConsoleVariable<int32> LogRequests(TEXT("bbb.Monster.Navigation.LogRequests"), 0,
        TEXT("记录完整寻路耗时与原因 0关闭"), ECVF_Cheat);
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
    MonsterQuery.AddRequirement<FMassVelocityFragment>(EMassFragmentAccess::ReadOnly);
    MonsterQuery.AddRequirement<FBBBMonsterTargetFragment>(EMassFragmentAccess::ReadOnly);
    MonsterQuery.AddRequirement<FBBBMonsterSpecialFragment>(EMassFragmentAccess::ReadOnly, EMassFragmentPresence::Optional);
    MonsterQuery.AddRequirement<FBBBMonsterMovementFragment>(EMassFragmentAccess::ReadOnly);
    MonsterQuery.AddRequirement<FBBBMonsterAvoidanceFragment>(EMassFragmentAccess::ReadOnly);
    MonsterQuery.AddRequirement<FBBBMonsterNavigationFragment>(EMassFragmentAccess::ReadWrite);
    MonsterQuery.AddRequirement<FBBBMonsterBehaviorFragment>(EMassFragmentAccess::ReadOnly);
    MonsterQuery.AddRequirement<FBBBMonsterNetworkFragment>(EMassFragmentAccess::ReadOnly);
    MonsterQuery.AddRequirement<FBBBMonsterTraversalFragment>(EMassFragmentAccess::ReadOnly);
    MonsterQuery.AddRequirement<FBBBMonsterGroundFragment>(EMassFragmentAccess::ReadOnly);
    MonsterQuery.AddRequirement<FBBBMonsterMobilityFragment>(EMassFragmentAccess::ReadOnly);
    MonsterQuery.AddRequirement<FBBBMonsterVariationFragment>(EMassFragmentAccess::ReadOnly);
    MonsterQuery.AddRequirement<FMassActorFragment>(EMassFragmentAccess::ReadOnly, EMassFragmentPresence::Optional);
    MonsterQuery.AddTagRequirement<FBBBMonsterTag>(EMassFragmentPresence::All);
}

void UBBBMonsterNavigationProcessor::Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context)
{
    UWorld* World = Context.GetWorld();
    auto* Navigation = World ? FNavigationSystem::GetCurrent<UNavigationSystemV1>(World) : nullptr;
    auto* Nav = Navigation ? Cast<ARecastNavMesh>(Navigation->GetDefaultNavDataInstance()) : nullptr;
    if (!Nav || !Nav->GetRecastMesh() || MonsterQuery.GetNumMatchingEntities() == 0)
    {
        return;
    }
    if (!NavQuery || NavQuery->getAttachedNavMesh() != Nav->GetRecastMesh())
    {
        NavQuery = MakeShared<dtNavMeshQuery>();
        if (dtStatusFailed(NavQuery->init(Nav->GetRecastMesh(), 2048)))
        {
            NavQuery.Reset();
            return;
        }
    }
    const float Now = World->GetTimeSeconds();
    const float Dt = FMath::Clamp(Context.GetDeltaTimeSeconds(), 0.0f, 0.25f);
    const auto& Budget = *GetDefault<UBBBMonsterNavigationSettings>();
    TArray<TPair<FMassEntityHandle, float>> Requests;
    MonsterQuery.ForEachEntityChunk(Context, [&](FMassExecutionContext& Chunk)
    {
        const auto Transforms = Chunk.GetFragmentView<FTransformFragment>();
        const auto States = Chunk.GetFragmentView<FBBBMonsterBehaviorFragment>();
        const auto Targets = Chunk.GetFragmentView<FBBBMonsterTargetFragment>();
        const auto Rules = Chunk.GetFragmentView<FBBBMonsterNetworkFragment>();
        const auto Bodies = Chunk.GetFragmentView<FBBBMonsterMovementFragment>();
        const auto Grounds = Chunk.GetFragmentView<FBBBMonsterGroundFragment>();
        const auto Injuries = Chunk.GetFragmentView<FBBBMonsterMobilityFragment>();
        const auto Crowds = Chunk.GetFragmentView<FBBBMonsterAvoidanceFragment>();
        const auto Traversals = Chunk.GetFragmentView<FBBBMonsterTraversalFragment>();
        auto Paths = Chunk.GetMutableFragmentView<FBBBMonsterNavigationFragment>();
        for (int32 I = 0; I < Chunk.GetNumEntities(); ++I)
        {
            auto& Path = Paths[I];
            const auto* Rule = Rules[I].Definition.Get();
            if (!Rule)
            {
                continue;
            }
            const bool bChase = States[I].State == EBBBMonsterBehavior::Chase;
            const bool bPatrol = States[I].State == EBBBMonsterBehavior::Patrol;
            const FVector Location = Transforms[I].GetTransform().GetLocation();
            const bool bNewTarget = Path.TargetRevision != Targets[I].Revision;
            const bool bNewPatrol = bPatrol && Path.ActionId != States[I].ActionId;
            if (bNewTarget || bNewPatrol)
            {
                FBBBMonsterNavigation::Reset(Path);
                Path.bReachedDestination = false;
                Path.FailureCount = 0;
                Path.ExcludedUntil = 0;
                Path.NextPathRefreshTime = Now;
                Path.ProgressTime = Now;
                Path.ProgressPosition = Location;
            }
            Path.TargetRevision = Targets[I].Revision;
            Path.ActionId = States[I].ActionId;
            const auto Specials = Chunk.GetFragmentView<FBBBMonsterSpecialFragment>();
            if (Traversals[I].Fact.Kind != 0 || States[I].State == EBBBMonsterBehavior::Attack
                || (!Specials.IsEmpty() && Specials[I].Fact.HoldsMovement()))
            {
                Path.Condition = EBBBMonsterNavigationCondition::Suspended;
                Path.ProgressTime = Now;
                Path.ProgressPosition = Location;
                Path.BlockedDuration = 0;
                Path.StopReason = Traversals[I].Fact.Kind != 0 ? FName(TEXT("Traversal"))
                    : !Specials.IsEmpty() && Specials[I].Fact.HoldsMovement() ? FName(TEXT("SpecialAction")) : FName(TEXT("Attack"));
                continue;
            }
            if ((!bPatrol && !bChase) || (bChase && !Targets[I].bHasTarget))
            {
                FBBBMonsterNavigation::Reset(Path);
                Path.RequestSince = -1;
                Path.Condition = EBBBMonsterNavigationCondition::Suspended;
                Path.StopReason = States[I].State == EBBBMonsterBehavior::Idle ? FName(TEXT("PatrolIdle")) : FName(TEXT("Behavior"));
                continue;
            }
            if (Path.bHasPath && (Path.bCrawlingPath != Injuries[I].bCrawling || !Path.NativePath.IsValid()
                || !Path.NativePath->IsUpToDate() || !Path.NativePath->IsValid()
                || !Path.Corridor || !Path.Filter || !Path.Corridor->isValid(16, NavQuery.Get(),
                    static_cast<const FRecastQueryFilter*>(Path.Filter->GetImplementation()))))
            {
                FBBBMonsterNavigation::Reset(Path);
                Path.LastRepathCondition = EBBBMonsterNavigationCondition::PathInvalidated;
                Path.NextPathRefreshTime = Now;
            }
            const bool bPaused = !Grounds[I].bGrounded || Injuries[I].IsStaggering(Now)
                || Now < Injuries[I].HitStopEndsAt
                || (Injuries[I].bCrawling && Now < Injuries[I].CrawlStartedAt + Rule->CrawlTransitionDuration);
            Path.MotionEfficiency = Grounds[I].RequestedDistance > 1
                ? FMath::Clamp(Grounds[I].MovedDistance / Grounds[I].RequestedDistance, 0.0f, 1.0f) : 1;
            if (bPaused || FVector::DistSquared2D(Location, Path.ProgressPosition) >= FMath::Square(25.0f))
            {
                Path.ProgressTime = Now;
                Path.ProgressPosition = Location;
                Path.BlockedDuration = 0;
            }
            if (!bPaused && !Crowds[I].bCrowdConstrained && Grounds[I].RequestedDistance > 1 && Path.MotionEfficiency < 0.1f)
            {
                Path.BlockedDuration += Dt;
            }
            Path.StopReason = NAME_None;
            if (bPaused)
            {
                Path.StopReason = !Grounds[I].bGrounded ? FName(TEXT("Airborne")) : Injuries[I].IsStaggering(Now)
                    ? FName(TEXT("Stagger")) : Now < Injuries[I].HitStopEndsAt ? FName(TEXT("HitStop")) : FName(TEXT("PostureChange"));
            }
            const bool bCrowdStopped = Crowds[I].bCrowdConstrained && Grounds[I].MovedDistance < 1;
            if (!bCrowdStopped)
            {
                Path.CrowdSince = -1;
            }
            if (bCrowdStopped && Path.CrowdSince < 0)
            {
                Path.CrowdSince = Now;
            }
            const bool bChangeApproach = bChase && !bPaused && Path.TraversalPoly == 0 && Path.CrowdSince >= 0
                && Now - Path.CrowdSince >= Rule->NavigationStuckDuration * 2;
            if (bChangeApproach)
            {
                ++Path.ApproachAttempt;
                Path.CrowdSince = Now;
                Path.NextPathRefreshTime = Now;
            }
            if (!Path.bHasPath)
            {
                Path.StopReason = Now < Path.ExcludedUntil ? FName(TEXT("NoRoute")) : FName(TEXT("PathRequest"));
            }
            Path.Condition = bPaused ? EBBBMonsterNavigationCondition::Suspended : !Path.bHasPath
                ? EBBBMonsterNavigationCondition::WaitingForPath : EBBBMonsterNavigationCondition::Following;
            if (!bPaused && Crowds[I].bCrowdConstrained)
            {
                Path.Condition = EBBBMonsterNavigationCondition::WaitingForNeighbor;
                Path.StopReason = TEXT("Crowd");
            }
            const bool bStuck = Path.bHasPath && !bPaused && !Crowds[I].bCrowdConstrained
                && (Path.BlockedDuration >= Rule->NavigationStuckDuration
                    || (Path.ProgressTime >= 0 && Now - Path.ProgressTime >= Rule->NavigationStuckDuration));
            const FVector DesiredGoal = !Specials.IsEmpty() && Specials[I].Fact.Phase == 5
                ? Specials[I].Fact.Goal : Targets[I].TargetLocation;
            const float GoalDistance = FVector::Dist2D(Location, DesiredGoal);
            const bool bFar = bChase && GoalDistance >= Rule->NavigationFarDistance;
            const float Threshold = bFar ? Rule->NavigationFarGoalThreshold : Rule->NavigationNearGoalThreshold;
            const bool bMoved = bChase && FVector::DistSquared(DesiredGoal, Path.RequestedGoal) > FMath::Square(Threshold);
            const bool bExpired = bChase && Now - Path.LastPathTime >= Rule->NavigationMaxPathAge;
            if (bStuck)
            {
                Path.LastRepathCondition = EBBBMonsterNavigationCondition::NoProgress;
                Path.Condition = EBBBMonsterNavigationCondition::Blocked;
                Path.StopReason = TEXT("Environment");
            }
            if (bMoved)
            {
                Path.LastRepathCondition = EBBBMonsterNavigationCondition::TargetMoved;
            }
            if (bExpired && !bMoved && !bStuck)
            {
                Path.LastRepathCondition = EBBBMonsterNavigationCondition::PathExpired;
            }
            if ((!Path.bHasPath || bMoved || bExpired || bStuck || bChangeApproach) && !bPaused && !Path.bReachedDestination
                && Now >= Path.NextPathRefreshTime && Now >= Path.ExcludedUntil)
            {
                if (Path.RequestSince < 0)
                {
                    Path.RequestSince = Now;
                }
                Requests.Emplace(Chunk.GetEntity(I), Now - Path.RequestSince + (Path.bHasPath ? 0.0f : 2.0f));
            }
        }
    });
    Requests.Sort([](const auto& A, const auto& B) { return A.Value > B.Value; });
    const double Started = FPlatformTime::Seconds();
    int32 Submitted = 0;
    for (const auto& Request : Requests)
    {
        if (Submitted >= Budget.MaxRequestsPerFrame || (Submitted > 0 && (FPlatformTime::Seconds() - Started) * 1000 >= Budget.TimeBudgetMs))
        {
            break;
        }
        ++Submitted;
        const auto Entity = Request.Key;
        auto& Path = EntityManager.GetFragmentDataChecked<FBBBMonsterNavigationFragment>(Entity);
        const auto& State = EntityManager.GetFragmentDataChecked<FBBBMonsterBehaviorFragment>(Entity);
        auto Target = EntityManager.GetFragmentDataChecked<FBBBMonsterTargetFragment>(Entity);
        const auto* Special = EntityManager.GetFragmentDataPtr<FBBBMonsterSpecialFragment>(Entity);
        if (Special && Special->Fact.Phase == 5)
        {
            Target.TargetLocation = Special->Fact.Goal;
            Target.bTargetVisible = false;
        }
        const auto& Body = EntityManager.GetFragmentDataChecked<FBBBMonsterMovementFragment>(Entity);
        const auto& Injury = EntityManager.GetFragmentDataChecked<FBBBMonsterMobilityFragment>(Entity);
        const auto& Ground = EntityManager.GetFragmentDataChecked<FBBBMonsterGroundFragment>(Entity);
        const auto& Rule = *EntityManager.GetFragmentDataChecked<FBBBMonsterNetworkFragment>(Entity).Definition;
        const FVector Location = EntityManager.GetFragmentDataChecked<FTransformFragment>(Entity).GetTransform().GetLocation();
        const bool bChase = State.State == EBBBMonsterBehavior::Chase;
        const float HalfHeight = Ground.CapsuleHalfHeight > 0 ? Ground.CapsuleHalfHeight : Body.CapsuleHalfHeight;
        const FVector Feet = Location - FVector::UpVector * HalfHeight;
        FVector Goal = Target.TargetLocation - FVector::UpVector * (bChase ? Rule.CapsuleHalfHeight : HalfHeight);
        if (bChase && Target.bTargetVisible && FVector::Dist2D(Location, Target.TargetLocation) < Rule.ApproachRange
            && FMath::Abs(Feet.Z - Goal.Z) <= Body.MaxStepHeight + 30)
        {
            const FVector Toward = (Target.TargetLocation - Location).GetSafeNormal2D();
            const FVector Side(-Toward.Y, Toward.X, 0);
            const uint32 Seed = EntityManager.GetFragmentDataChecked<FBBBMonsterVariationFragment>(Entity).Seed;
            const float Bias = (static_cast<int32>((GetTypeHash(Seed + Path.ApproachAttempt * 7919) >> 8) % 201) - 100) * 0.01f;
            Goal += Side * Bias * FMath::Min(Rule.ApproachSpread, Rule.AttackRange * 0.65f);
        }
        FNavLocation Start;
        FNavLocation End;
        const bool bStart = Navigation->ProjectPointToNavigation(Feet, Start, FVector(Body.CapsuleRadius + 20, Body.CapsuleRadius + 20, Body.MaxStepHeight + 30), Nav);
        const bool bEnd = Navigation->ProjectPointToNavigation(Goal, End, FVector(150, 150, 250), Nav);
        Path.RequestSince = -1;
        Path.NextPathRefreshTime = Now + (FVector::Dist2D(Location, Target.TargetLocation) >= Rule.NavigationFarDistance
            ? Rule.NavigationFarInterval : Rule.NavigationNearInterval) * (0.9f + (Entity.Index % 7) * 0.03f);
        ++Path.RepathCount;
        const auto Fail = [&]()
        {
            ++Path.FailureCount;
            if (!Path.bHasPath)
            {
                Path.Condition = EBBBMonsterNavigationCondition::WaitingForPath;
                Path.StopReason = TEXT("PathRequest");
                if (Path.FailureCount >= Rule.NavigationFailureLimit)
                {
                    Path.Condition = EBBBMonsterNavigationCondition::Unreachable;
                    Path.StopReason = TEXT("NoRoute");
                    Path.ExcludedPosition = Target.TargetLocation;
                    Path.ExcludedUntil = Now + Rule.NavigationRetryDuration;
                }
            }
        };
        if (!bStart || !bEnd)
        {
            Fail();
            continue;
        }
        auto Filter = Nav->GetDefaultQueryFilter()->GetCopy();
        TraversalLimit = Rule.Traversal.bEnabled && !Injury.bCrawling && Injury.AttackRatio >= 0.99f ? Rule.Traversal.MaximumHeight : 0;
        if (TraversalLimit <= 0)
        {
            Filter->SetExcludedArea(Nav->GetAreaID(UBBBMonsterClimbArea::StaticClass()));
        }
        FPathFindingQuery RequestQuery(this, *Nav, Start.Location, End.Location, Filter);
        RequestQuery.SetAllowPartialPaths(bChase);
        const double QueryStart = FPlatformTime::Seconds();
        const auto Result = ARecastNavMesh::FindPath(Nav->GetConfig(), RequestQuery);
        TraversalLimit = 0;
        Path.LastQueryMs = (FPlatformTime::Seconds() - QueryStart) * 1000;
        const auto* MeshPath = Result.Path.IsValid() ? Result.Path->CastPath<FNavMeshPath>() : nullptr;
        if (!Result.IsSuccessful() || !MeshPath || MeshPath->PathCorridor.IsEmpty() || Result.Path->GetPathPoints().IsEmpty())
        {
            Fail();
            continue;
        }
        auto Corridor = MakeShared<dtPathCorridor>();
        if (!Corridor->init(FMath::Max(64, MeshPath->PathCorridor.Num() + 32)))
        {
            Fail();
            continue;
        }
        const auto RcStart = Unreal2RecastPoint(Start.Location);
        const FVector EndPoint = Result.Path->GetPathPoints().Last().Location;
        const auto RcEnd = Unreal2RecastPoint(EndPoint);
        Corridor->reset(MeshPath->PathCorridor[0], &RcStart.X);
        Corridor->setCorridor(&RcEnd.X, MeshPath->PathCorridor.GetData(), MeshPath->PathCorridor.Num());
        Result.Path->EnableRecalculationOnInvalidation(false);
        Path.NativePath = Result.Path;
        Path.Corridor = MoveTemp(Corridor);
        Path.Query = NavQuery;
        Path.Filter = Filter;
        Path.Destination = EndPoint;
        Path.QueryDestination = EndPoint;
        Path.RequestedGoal = Target.TargetLocation;
        Path.LastPathTime = Now;
        Path.bHasPath = true;
        Path.bReachedDestination = false;
        Path.bPartialPath = Result.Path->IsPartial() || FVector::DistSquared(End.Location, Goal) > FMath::Square(Rule.MaxStepHeight + 25);
        Path.bCrawlingPath = Injury.bCrawling;
        Path.FailureCount = 0;
        Path.NextBoundaryTime = 0;
        Path.TraversalPoly = 0;
        Path.TraversalLink = FNavLinkId::Invalid;
        if (LogRequests.GetValueOnGameThread() != 0)
        {
            UE_LOG(LogTemp, Log, TEXT("[BBBDetour] Entity=%d Polys=%d QueryMs=%.3f Partial=%d"), Entity.Index,
                MeshPath->PathCorridor.Num(), Path.LastQueryMs, Path.bPartialPath);
        }
    }
    TArray<TPair<FMassEntityHandle, float>> BoundaryRequests;
    MonsterQuery.ForEachEntityChunk(Context, [&](FMassExecutionContext& Chunk)
    {
        const auto Paths = Chunk.GetFragmentView<FBBBMonsterNavigationFragment>();
        for (int32 I = 0; I < Paths.Num(); ++I)
        {
            if (Paths[I].bHasPath && Now >= Paths[I].NextBoundaryTime)
            {
                BoundaryRequests.Emplace(Chunk.GetEntity(I), Paths[I].NextBoundaryTime);
            }
        }
    });
    BoundaryRequests.Sort([](const auto& A, const auto& B) { return A.Value < B.Value; });
    TSet<FMassEntityHandle> AllowedBoundaries;
    for (int32 I = 0; I < FMath::Min(BoundaryRequests.Num(), Budget.MaxLocalChecksPerFrame); ++I)
    {
        AllowedBoundaries.Add(BoundaryRequests[I].Key);
    }
    int32 Boundaries = 0;
    const double BoundaryStart = FPlatformTime::Seconds();
    MonsterQuery.ForEachEntityChunk(Context, [&](FMassExecutionContext& Chunk)
    {
        auto Paths = Chunk.GetMutableFragmentView<FBBBMonsterNavigationFragment>();
        const auto Transforms = Chunk.GetFragmentView<FTransformFragment>();
        const auto Rules = Chunk.GetFragmentView<FBBBMonsterNetworkFragment>();
        const auto Bodies = Chunk.GetFragmentView<FBBBMonsterMovementFragment>();
        const auto Grounds = Chunk.GetFragmentView<FBBBMonsterGroundFragment>();
        const auto Traversals = Chunk.GetFragmentView<FBBBMonsterTraversalFragment>();
        const auto Velocities = Chunk.GetFragmentView<FMassVelocityFragment>();
        for (int32 I = 0; I < Chunk.GetNumEntities(); ++I)
        {
            auto& P = Paths[I];
            if (!P.bHasPath || !P.Corridor || Traversals[I].Fact.Kind != 0 || !Rules[I].Definition.IsValid())
            {
                continue;
            }
            const auto& Rule = *Rules[I].Definition;
            const auto* Filter = static_cast<const FRecastQueryFilter*>(P.Filter->GetImplementation());
            const FVector Location = Transforms[I].GetTransform().GetLocation();
            const FVector Feet = Location - FVector::UpVector * (Grounds[I].CapsuleHalfHeight > 0 ? Grounds[I].CapsuleHalfHeight : Bodies[I].CapsuleHalfHeight);
            FBBBMonsterNavigation::Feedback(P, Feet);
            const auto& Target = Chunk.GetFragmentView<FBBBMonsterTargetFragment>()[I];
            const auto& State = Chunk.GetFragmentView<FBBBMonsterBehaviorFragment>()[I];
            if (State.State == EBBBMonsterBehavior::Chase && Target.bTargetVisible && !P.bPartialPath)
            {
                const auto Specials = Chunk.GetFragmentView<FBBBMonsterSpecialFragment>();
                const FVector DesiredGoal = !Specials.IsEmpty() && Specials[I].Fact.Phase == 5
                    ? Specials[I].Fact.Goal : Target.TargetLocation;
                const FVector Shift = DesiredGoal - P.RequestedGoal;
                const float Threshold = FVector::Dist2D(Location, Target.TargetLocation) >= Rule.NavigationFarDistance
                    ? Rule.NavigationFarGoalThreshold : Rule.NavigationNearGoalThreshold;
                if (Shift.SizeSquared() <= FMath::Square(Threshold * 2))
                {
                    const auto NewTarget = Unreal2RecastPoint(P.QueryDestination + Shift);
                    P.Corridor->moveTargetPosition(&NewTarget.X, NavQuery.Get(), Filter);
                    P.Destination = Recast2UnrealPoint(P.Corridor->getTarget());
                }
            }
            dtReal Verts[18];
            unsigned char Flags[6];
            dtPolyRef Polys[6];
            const int32 Count = P.Corridor->findCorners(Verts, Flags, Polys, 6, NavQuery.Get(), Filter, 0, 10, true);
            P.Corners.Reset(6);
            P.TraversalPoly = 0;
            P.TraversalLink = FNavLinkId::Invalid;
            for (int32 Corner = 0; Corner < Count; ++Corner)
            {
                P.Corners.Add(Recast2UnrealPoint(Verts + Corner * 3));
                if ((Flags[Corner] & DT_STRAIGHTPATH_OFFMESH_CONNECTION) != 0)
                {
                    P.TraversalPoly = Polys[Corner];
                    P.TraversalLink = Nav->GetNavLinkUserId(Polys[Corner]);
                    P.TraversalEntry = P.Corners.Last();
                    dtPolyRef Refs[2];
                    dtReal A[3];
                    dtReal B[3];
                    const auto Position = Unreal2RecastPoint(Feet);
                    if (P.Corridor->canMoveOverOffmeshConnection(Polys[Corner], Refs, &Position.X, A, B, NavQuery.Get()))
                    {
                        P.TraversalEntry = Recast2UnrealPoint(A);
                        P.TraversalExit = Recast2UnrealPoint(B);
                    }
                    break;
                }
            }
            P.FollowTarget = P.Corners.IsEmpty() ? P.Destination : P.Corners[0];
            if (P.Corners.Num() > 1 && FVector::Dist2D(Feet, P.Corners[0]) < Bodies[I].CapsuleRadius)
            {
                const bool bFirstLink = (Flags[0] & DT_STRAIGHTPATH_OFFMESH_CONNECTION) != 0;
                if (!bFirstLink)
                {
                    const FVector B = (P.Corners[1] - P.Corners[0]).GetSafeNormal2D();
                    P.FollowTarget = P.Corners[0] + B * FMath::Min(30.0f, FVector::Dist2D(Feet, P.Corners[0]));
                }
            }
            P.RemainingDistance = 0;
            FVector Previous = Feet;
            for (const FVector& Corner : P.Corners)
            {
                P.RemainingDistance += FVector::Dist(Previous, Corner);
                Previous = Corner;
            }
            P.RemainingDistance += FVector::Dist(Previous, P.Destination);
            if (P.Corners.Num() > 0 && FVector::DistSquared2D(Feet, P.FollowTarget) < 1 && P.TraversalPoly == 0)
            {
                P.FollowTarget = P.Destination;
            }
            const bool bArrived = Grounds[I].bGrounded && P.TraversalPoly == 0
                && FVector::DistSquared2D(Feet, P.Destination) < FMath::Square(15.0f)
                && FMath::Abs(Feet.Z - P.Destination.Z) < Bodies[I].MaxStepHeight + 10;
            if (bArrived)
            {
                P.bReachedDestination = !P.bPartialPath && (Chunk.GetFragmentView<FBBBMonsterBehaviorFragment>()[I].State == EBBBMonsterBehavior::Patrol
                    || !Chunk.GetFragmentView<FBBBMonsterTargetFragment>()[I].bTargetVisible);
                P.Condition = EBBBMonsterNavigationCondition::Arrived;
                P.StopReason = P.bPartialPath ? FName(TEXT("PartialBoundary")) : FName(TEXT("Destination"));
                P.NextPathRefreshTime = FMath::Max(P.NextPathRefreshTime, Now + Rule.NavigationNearInterval);
                if (P.bPartialPath)
                {
                    P.LastPathTime = -1;
                }
            }
            if (AllowedBoundaries.Contains(Chunk.GetEntity(I)) && Boundaries < Budget.MaxLocalChecksPerFrame
                && (Boundaries == 0 || (FPlatformTime::Seconds() - BoundaryStart) * 1000 < Budget.LocalTimeBudgetMs))
            {
                ++Boundaries;
                dtPolyRef Neighbors[16];
                dtPolyRef WallRefs[24];
                dtReal Walls[144];
                int32 NeighborCount = 0;
                int32 WallCount = 0;
                NavQuery->findWallsInNeighbourhood(P.Corridor->getFirstPoly(), P.Corridor->getPos(), 400, Filter,
                    Neighbors, &NeighborCount, 16, Walls, WallRefs, &WallCount, 24);
                P.Walls.Reset(48);
                for (int32 W = 0; W < WallCount; ++W)
                {
                    if (WallRefs[W] == 0)
                    {
                        P.Walls.Add(Recast2UnrealPoint(Walls + W * 6));
                        P.Walls.Add(Recast2UnrealPoint(Walls + W * 6 + 3));
                    }
                }
                P.NextBoundaryTime = Now + Rule.NavigationFollowInterval;
            }
            if (DebugEntity.GetValueOnGameThread() == Chunk.GetEntity(I).Index)
            {
                Previous = Feet;
                for (const auto& Corner : P.Corners)
                {
                    DrawDebugLine(World, Previous + FVector(0, 0, 5), Corner + FVector(0, 0, 5), FColor::Cyan);
                    Previous = Corner;
                }
                const auto& Crowd = Chunk.GetFragmentView<FBBBMonsterAvoidanceFragment>()[I];
                DrawDebugDirectionalArrow(World, Location, Location + Crowd.DesiredVelocity * 0.3, 10, FColor::Yellow);
                DrawDebugDirectionalArrow(World, Location, Location + Crowd.RecommendedVelocity * 0.3, 10, FColor::Green);
                DrawDebugDirectionalArrow(World, Location, Location + Velocities[I].Value * 0.3, 10, FColor::White);
                DrawDebugString(World, Location + FVector(0, 0, 110), FString::Printf(TEXT("%s Reason=%s Path=%d Neighbors=%d Samples=%d Repath=%u"),
                    *StaticEnum<EBBBMonsterNavigationCondition>()->GetNameStringByValue(static_cast<int64>(P.Condition)), *P.StopReason.ToString(),
                    P.bHasPath, Crowd.NeighborCount, Crowd.VelocitySamples, P.RepathCount), nullptr, FColor::White, 0, true);
            }
        }
    });
}
