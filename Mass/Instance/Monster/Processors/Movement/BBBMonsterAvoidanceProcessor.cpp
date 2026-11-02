#include "BBBMonsterAvoidanceProcessor.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Combat/BBBMonsterSpecialFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Config/BBBMonsterSpecialDefinition.h"

#include "BBBMonsterNavigationProcessor.h"
#include "BBBMonsterTraversalProcessor.h"
#include "BBBMonsterLocomotionProcessor.h"
#include "BBBMonsterSteering.h"
#include "BBBWork/UBBBNexus/Mass/Core/BBBMassProcessingGroups.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Config/BBBMonsterDefinition.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Behavior/BBBMonsterBehaviorFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Movement/BBBMonsterAvoidanceFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Movement/BBBMonsterMovementFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Movement/BBBMonsterNavigationFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Movement/BBBMonsterMobilityFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Movement/BBBMonsterTraversalFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Network/BBBMonsterNetworkFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Tags/BBBMonsterTag.h"
#include "MassCommonFragments.h"
#include "MassMovementFragments.h"
#include "MassExecutionContext.h"
#include "NavMesh/RecastHelpers.h"
#include "DetourCrowd/DetourObstacleAvoidance.h"
#include "Engine/World.h"

UBBBMonsterAvoidanceProcessor::UBBBMonsterAvoidanceProcessor() : MonsterQuery(*this)
{
    bAutoRegisterWithProcessingPhases = true;
    bRequiresGameThreadExecution = true;
    ProcessingPhase = EMassProcessingPhase::PrePhysics;
    ExecutionOrder.ExecuteInGroup = BBBMassProcessingGroups::Movement;
    ExecutionOrder.ExecuteAfter.Add(UBBBMonsterTraversalProcessor::StaticClass()->GetFName());
    ExecutionFlags = static_cast<uint8>(EProcessorExecutionFlags::Server | EProcessorExecutionFlags::Standalone);
}

void UBBBMonsterAvoidanceProcessor::ConfigureQueries(const TSharedRef<FMassEntityManager>& EntityManager)
{
    MonsterQuery.AddRequirement<FTransformFragment>(EMassFragmentAccess::ReadOnly);
    MonsterQuery.AddRequirement<FMassVelocityFragment>(EMassFragmentAccess::ReadOnly);
    MonsterQuery.AddRequirement<FBBBMonsterAvoidanceFragment>(EMassFragmentAccess::ReadWrite);
    MonsterQuery.AddRequirement<FBBBMonsterBehaviorFragment>(EMassFragmentAccess::ReadOnly);
    MonsterQuery.AddRequirement<FBBBMonsterNavigationFragment>(EMassFragmentAccess::ReadOnly);
    MonsterQuery.AddRequirement<FBBBMonsterMovementFragment>(EMassFragmentAccess::ReadOnly);
    MonsterQuery.AddRequirement<FBBBMonsterMobilityFragment>(EMassFragmentAccess::ReadOnly);
    MonsterQuery.AddRequirement<FBBBMonsterTraversalFragment>(EMassFragmentAccess::ReadOnly);
    MonsterQuery.AddRequirement<FBBBMonsterSpecialFragment>(EMassFragmentAccess::ReadOnly, EMassFragmentPresence::Optional);
    MonsterQuery.AddRequirement<FBBBMonsterNetworkFragment>(EMassFragmentAccess::ReadOnly);
    MonsterQuery.AddTagRequirement<FBBBMonsterTag>(EMassFragmentPresence::All);
}

void UBBBMonsterAvoidanceProcessor::Execute(FMassEntityManager&, FMassExecutionContext& Context)
{
    const float Now = Context.GetWorld()->GetTimeSeconds();
    const float Dt = Context.GetDeltaTimeSeconds();
    constexpr float CellSize = 500;
    using FNeighbor = TTuple<FMassEntityHandle, FVector, FVector, FVector, float, float>;
    TMap<FIntPoint, TArray<FNeighbor>> Buckets;
    MonsterQuery.ForEachEntityChunk(Context, [&](FMassExecutionContext& Chunk)
    {
        const auto Transforms = Chunk.GetFragmentView<FTransformFragment>();
        const auto Velocities = Chunk.GetFragmentView<FMassVelocityFragment>();
        const auto States = Chunk.GetFragmentView<FBBBMonsterBehaviorFragment>();
        const auto Paths = Chunk.GetFragmentView<FBBBMonsterNavigationFragment>();
        const auto Bodies = Chunk.GetFragmentView<FBBBMonsterMovementFragment>();
        const auto Injuries = Chunk.GetFragmentView<FBBBMonsterMobilityFragment>();
        const auto Rules = Chunk.GetFragmentView<FBBBMonsterNetworkFragment>();
        const auto Traversals = Chunk.GetFragmentView<FBBBMonsterTraversalFragment>();
        auto Crowds = Chunk.GetMutableFragmentView<FBBBMonsterAvoidanceFragment>();
        for (int32 I = 0; I < Chunk.GetNumEntities(); ++I)
        {
            auto& C = Crowds[I];
            C.DesiredVelocity = FVector::ZeroVector;
            C.RecommendedVelocity = FVector::ZeroVector;
            C.SeparationVelocity = FVector::ZeroVector;
            C.bCrowdConstrained = false;
            C.NeighborCount = 0;
            C.VelocitySamples = 0;
            if (States[I].State == EBBBMonsterBehavior::Dead || Traversals[I].Fact.Kind != 0 || !Rules[I].Definition.IsValid())
            {
                continue;
            }
            const auto& Rule = *Rules[I].Definition;
            const auto& Body = Bodies[I];
            const auto& Injury = Injuries[I];
            const bool bPatrol = States[I].State == EBBBMonsterBehavior::Patrol;
            const bool bChase = States[I].State == EBBBMonsterBehavior::Chase;
            const auto Specials = Chunk.GetFragmentView<FBBBMonsterSpecialFragment>();
            const bool bSpecialMove = !Specials.IsEmpty() && Specials[I].Fact.HoldsMovement();
            const FVector Location = Transforms[I].GetTransform().GetLocation();
            if (!bSpecialMove && (bPatrol || bChase) && Paths[I].bHasPath && !Paths[I].bReachedDestination && !Injury.IsStaggering(Now)
                && Now >= Injury.HitStopEndsAt && (!Injury.bCrawling || Now >= Injury.CrawlStartedAt + Rule.CrawlTransitionDuration))
            {
                const auto Gait = UBBBMonsterLocomotionProcessor::SelectGait(Body, Paths[I].RemainingDistance, bPatrol);
                float Speed = Gait == EBBBMonsterGait::Walk ? (bChase && Body.MaxGait == EBBBMonsterGait::Walk
                    ? Body.WalkerChaseWalkSpeed : Body.WalkSpeed) : Gait == EBBBMonsterGait::Run ? Body.RunSpeed : Body.SprintSpeed;
                if (Injury.bCrawling)
                {
                    Speed = Rule.CrawlSpeed;
                }
                Speed *= Injury.GetSpeedRatio(Now);
                if (!Specials.IsEmpty() && Rule.Special && Rule.Special->Kind == EBBBMonsterSpecialKind::Stalker)
                {
                    if (Specials[I].Fact.Phase == 5)
                    {
                        Speed = FMath::Min(Speed, Body.WalkSpeed);
                    }
                    if (Specials[I].Fact.Phase == 6 && !Injury.bCrawling)
                    {
                        Speed = Rule.Special->RushSpeed * Injury.GetSpeedRatio(Now);
                    }
                }
                if (bPatrol)
                {
                    Speed = FMath::Min(Speed, FMath::Sqrt(2 * Body.Deceleration * FMath::Max(0.0f, Paths[I].RemainingDistance - 10)));
                }
                float Accelerated = UBBBMonsterLocomotionProcessor::CalculateSpeed(Body, Velocities[I].Value.Size2D(), Speed,
                    Paths[I].RemainingDistance, Dt);
                if (Now < Injury.SlowRecoveryStartedAt)
                {
                    Accelerated = FMath::Min(Accelerated, Speed);
                }
                const FVector GoalDelta = Paths[I].FollowTarget - Location;
                FVector Direction = GoalDelta.GetSafeNormal2D();
                const bool bCurvedTravel = !Injury.bCrawling && Paths[I].TraversalPoly == 0
                    && (Specials.IsEmpty() || Specials[I].Fact.Phase == 0);
                if (bCurvedTravel)
                {
                    const float ArrivalDistance = bChase ? Rule.AttackRange + Body.CapsuleRadius : 10.0f;
                    Direction = FBBBMonsterSteering::ApplyTravelCurve(GoalDelta, Rule, Rules[I].InstanceId,
                        Now, Paths[I].RemainingDistance, ArrivalDistance, Body.CapsuleRadius, Gait != EBBBMonsterGait::Walk);
                }
                C.DesiredVelocity = Direction * Accelerated;
            }
            const FIntPoint Cell(FMath::FloorToInt(Location.X / CellSize), FMath::FloorToInt(Location.Y / CellSize));
            Buckets.FindOrAdd(Cell).Emplace(Chunk.GetEntity(I), Location, Velocities[I].Value,
                C.DesiredVelocity, C.CollisionRadius, Injury.bCrawling ? Rule.CrawlCapsuleHalfHeight : Body.CapsuleHalfHeight);
        }
    });
    if (!Sampler)
    {
        Sampler = MakeShared<dtObstacleAvoidanceQuery>();
        if (!Sampler->init(16, 24, 0))
        {
            Sampler.Reset();
            return;
        }
    }
    MonsterQuery.ForEachEntityChunk(Context, [&](FMassExecutionContext& Chunk)
    {
        const auto Transforms = Chunk.GetFragmentView<FTransformFragment>();
        const auto Velocities = Chunk.GetFragmentView<FMassVelocityFragment>();
        const auto Paths = Chunk.GetFragmentView<FBBBMonsterNavigationFragment>();
        const auto Bodies = Chunk.GetFragmentView<FBBBMonsterMovementFragment>();
        auto Crowds = Chunk.GetMutableFragmentView<FBBBMonsterAvoidanceFragment>();
        for (int32 I = 0; I < Chunk.GetNumEntities(); ++I)
        {
            auto& C = Crowds[I];
            if (C.DesiredVelocity.IsNearlyZero())
            {
                continue;
            }
            Sampler->reset();
            const FVector Location = Transforms[I].GetTransform().GetLocation();
            const FIntPoint Cell(FMath::FloorToInt(Location.X / CellSize), FMath::FloorToInt(Location.Y / CellSize));
            TArray<TPair<float, const FNeighbor*>, TInlineAllocator<16>> Closest;
            const float Search = FMath::Clamp(FMath::Max(C.NeighborSearchRadius, C.DesiredVelocity.Size() * 0.7f), 180.0f, 500.0f);
            for (int32 X = -1; X <= 1; ++X)
            {
                for (int32 Y = -1; Y <= 1; ++Y)
                {
                    const auto* Neighbors = Buckets.Find(Cell + FIntPoint(X, Y));
                    if (!Neighbors)
                    {
                        continue;
                    }
                    for (const auto& N : *Neighbors)
                    {
                        const float Distance = FVector::DistSquared2D(Location, N.Get<1>());
                        if (N.Get<0>() == Chunk.GetEntity(I) || Distance > FMath::Square(Search)
                            || FMath::Abs(Location.Z - N.Get<1>().Z) >= FMath::Min(Bodies[I].CapsuleHalfHeight, N.Get<5>()) * 1.5f)
                        {
                            continue;
                        }
                        if (Closest.Num() == 16 && Distance >= Closest.Last().Key)
                        {
                            continue;
                        }
                        if (Closest.Num() == 16)
                        {
                            Closest.Pop(EAllowShrinking::No);
                        }
                        Closest.Emplace(Distance, &N);
                        Closest.Sort([](const auto& A, const auto& B) { return A.Key < B.Key; });
                    }
                }
            }
            for (const auto& Entry : Closest)
            {
                const auto& N = *Entry.Value;
                const auto Position = Unreal2RecastPoint(N.Get<1>());
                const auto Velocity = Unreal2RecastPoint(FVector(N.Get<2>().X, N.Get<2>().Y, 0));
                const auto Desired = Unreal2RecastPoint(N.Get<3>());
                Sampler->addCircle(&Position.X, N.Get<4>(), &Velocity.X, &Desired.X);
                FVector Away = Location - N.Get<1>();
                Away.Z = 0;
                const float Distance = FMath::Sqrt(Entry.Key);
                const float Overlap = C.CollisionRadius + N.Get<4>() + 2 - Distance;
                if (Overlap > 0)
                {
                    if (Distance < 0.01f)
                    {
                        Away = FVector(Chunk.GetEntity(I).Index < N.Get<0>().Index ? -1 : 1, 0, 0);
                    }
                    C.SeparationVelocity += Away.GetSafeNormal2D() * FMath::Min(60.0f, Overlap * 5);
                }
            }
            C.NeighborCount = Closest.Num();
            for (int32 W = 0; W + 1 < Paths[I].Walls.Num() && W < 48; W += 2)
            {
                const auto A = Unreal2RecastPoint(Paths[I].Walls[W]);
                const auto B = Unreal2RecastPoint(Paths[I].Walls[W + 1]);
                Sampler->addSegment(&A.X, &B.X);
            }
            C.SeparationVelocity = C.SeparationVelocity.GetClampedToMaxSize(60);
            const float MaxSpeed = C.DesiredVelocity.Size();
            const FVector Intent = (C.DesiredVelocity + C.SeparationVelocity * FMath::Min(C.AvoidanceWeight, 2.0f)).GetClampedToMaxSize(MaxSpeed);
            const auto Position = Unreal2RecastPoint(Location);
            const auto Velocity = Unreal2RecastPoint(FVector(Velocities[I].Value.X, Velocities[I].Value.Y, 0));
            const auto Desired = Unreal2RecastPoint(Intent);
            dtObstacleAvoidanceParams Params{};
            Params.velBias = 0.4;
            Params.weightDesVel = 2;
            Params.weightCurVel = 0.75;
            Params.weightSide = 0.75;
            Params.weightToi = 2.5;
            Params.horizTime = 0.7;
            Params.sideBiasThreshold = 0.05;
            Params.weightRightBias = 0.1;
            Params.patternIdx = 0xff;
            Params.adaptiveDivs = 7;
            Params.adaptiveRings = 2;
            Params.adaptiveDepth = 3;
            dtReal Result[3] = {};
            C.VelocitySamples = Sampler->sampleVelocityAdaptive(&Position.X, C.CollisionRadius, MaxSpeed, 1,
                &Velocity.X, &Desired.X, Result, &Params);
            C.RecommendedVelocity = (Recast2UnrealPoint(Result) + C.SeparationVelocity).GetClampedToMaxSize(MaxSpeed);
            C.RecommendedVelocity.Z = 0;
            C.bCrowdConstrained = C.NeighborCount > 0 && C.RecommendedVelocity.SizeSquared() < C.DesiredVelocity.SizeSquared() * 0.25;
        }
    });
}
