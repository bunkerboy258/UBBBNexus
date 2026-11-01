#include "BBBMonsterTraversalProcessor.h"

#include "BBBMonsterNavigationProcessor.h"
#include "BBBMonsterLocomotionProcessor.h"
#include "BBBWork/UBBBNexus/Mass/Core/BBBMassProcessingGroups.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Config/BBBMonsterDefinition.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Config/BBBMonsterNavigationSettings.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Movement/BBBMonsterTraversalFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Movement/BBBMonsterNavigationFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Movement/BBBMonsterGroundFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Movement/BBBMonsterMobilityFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Network/BBBMonsterNetworkFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Behavior/BBBMonsterBehaviorFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Health/BBBMonsterHealthFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Presentation/BBBMonsterClimbLinkActor.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Tags/BBBMonsterTag.h"
#include "DrawDebugHelpers.h"
#include "MassActorSubsystem.h"
#include "MassCommonFragments.h"
#include "MassMovementFragments.h"
#include "MassExecutionContext.h"
#include "NavigationSystem.h"
#include "NavLinkCustomInterface.h"

namespace
{
    TAutoConsoleVariable<int32> DrawTraversal(TEXT("bbb.Monster.Traversal.Draw"), 0,
        TEXT("显示攀爬入口 台沿 出口及轨迹 0关闭 1全部 2仅活动"), ECVF_Cheat);

    float Smooth(const float A, const float B, const float Value)
    {
        const float T = FMath::Clamp((Value - A) / (B - A), 0.0f, 1.0f);
        return T * T * (3.0f - 2.0f * T);
    }
}

UBBBMonsterTraversalProcessor::UBBBMonsterTraversalProcessor() : MonsterQuery(*this)
{
    bAutoRegisterWithProcessingPhases = true;
    bRequiresGameThreadExecution = true;
    ProcessingPhase = EMassProcessingPhase::PrePhysics;
    ExecutionFlags = static_cast<uint8>(EProcessorExecutionFlags::Server | EProcessorExecutionFlags::Standalone);
    ExecutionOrder.ExecuteInGroup = BBBMassProcessingGroups::Movement;
    ExecutionOrder.ExecuteAfter.Add(UBBBMonsterNavigationProcessor::StaticClass()->GetFName());
    ExecutionOrder.ExecuteBefore.Add(UBBBMonsterLocomotionProcessor::StaticClass()->GetFName());
}

FVector UBBBMonsterTraversalProcessor::EvaluateFeet(const FBBBMonsterTraversalFact& Fact,
    const FBBBMonsterTraversalSettings& Settings, const float Progress)
{
    const float P = FMath::Clamp(Progress, 0.0f, 1.0f);
    const auto& Samples = Fact.Kind == 1 ? Settings.MantleTrajectory : Settings.ClimbTrajectory;
    if (Samples.Num() < 2)
    {
        return Fact.Start;
    }
    const float Frame = P * (Samples.Num() - 1);
    const int32 Index = FMath::Min(FMath::FloorToInt(Frame), Samples.Num() - 2);
    const FVector Root = FMath::Lerp(Samples[Index], Samples[Index + 1], Frame - Index);
    const float Forward = Smooth(0.55f, 0.93f, P) * FMath::Clamp(static_cast<float>(Root.X), 0.0f, 1.0f);
    FVector Feet = FMath::Lerp(Fact.Start, Fact.End, Forward);
    const float Rise = FMath::Max(FMath::Clamp(static_cast<float>(Root.Z), 0.0f, 1.0f), Smooth(0.05f, 0.55f, P));
    Feet.Z = FMath::Lerp(Fact.Start.Z, Fact.End.Z, Rise) + 8.0f * FMath::Sin(PI * P);
    return Feet;
}

void UBBBMonsterTraversalProcessor::ConfigureQueries(const TSharedRef<FMassEntityManager>&)
{
    MonsterQuery.AddRequirement<FTransformFragment>(EMassFragmentAccess::ReadWrite);
    MonsterQuery.AddRequirement<FMassVelocityFragment>(EMassFragmentAccess::ReadWrite);
    MonsterQuery.AddRequirement<FBBBMonsterTraversalFragment>(EMassFragmentAccess::ReadWrite);
    MonsterQuery.AddRequirement<FBBBMonsterNavigationFragment>(EMassFragmentAccess::ReadWrite);
    MonsterQuery.AddRequirement<FBBBMonsterGroundFragment>(EMassFragmentAccess::ReadWrite);
    MonsterQuery.AddRequirement<FBBBMonsterNetworkFragment>(EMassFragmentAccess::ReadOnly);
    MonsterQuery.AddRequirement<FBBBMonsterMovementFragment>(EMassFragmentAccess::ReadOnly);
    MonsterQuery.AddRequirement<FBBBMonsterBehaviorFragment>(EMassFragmentAccess::ReadOnly);
    MonsterQuery.AddRequirement<FBBBMonsterMobilityFragment>(EMassFragmentAccess::ReadOnly);
    MonsterQuery.AddRequirement<FBBBMonsterHealthFragment>(EMassFragmentAccess::ReadOnly);
    MonsterQuery.AddRequirement<FMassActorFragment>(EMassFragmentAccess::ReadOnly);
    MonsterQuery.AddTagRequirement<FBBBMonsterTag>(EMassFragmentPresence::All);
}

void UBBBMonsterTraversalProcessor::Execute(FMassEntityManager&, FMassExecutionContext& Context)
{
    UWorld* World = Context.GetWorld();
    auto* Navigation = FNavigationSystem::GetCurrent<UNavigationSystemV1>(World);
    if (!World || !Navigation || Context.GetDeltaTimeSeconds() <= SMALL_NUMBER)
    {
        return;
    }
    const float Now = World->GetTimeSeconds();
    const auto* Budget = GetDefault<UBBBMonsterNavigationSettings>();
    TArray<FVector> Occupied;
    MonsterQuery.ForEachEntityChunk(Context, [&Occupied](FMassExecutionContext& Chunk)
    {
        for (const auto& Traversal : Chunk.GetFragmentView<FBBBMonsterTraversalFragment>())
        {
            if (Traversal.Fact.Kind != 0)
            {
                Occupied.Add(Traversal.Fact.Start);
                Occupied.Add(Traversal.Fact.End);
            }
        }
    });
    int32 Checks = 0;
    const double CheckStart = FPlatformTime::Seconds();
    MonsterQuery.ForEachEntityChunk(Context, [&](FMassExecutionContext& Chunk)
    {
        auto Transforms = Chunk.GetMutableFragmentView<FTransformFragment>();
        auto Velocities = Chunk.GetMutableFragmentView<FMassVelocityFragment>();
        auto Traversals = Chunk.GetMutableFragmentView<FBBBMonsterTraversalFragment>();
        auto Paths = Chunk.GetMutableFragmentView<FBBBMonsterNavigationFragment>();
        auto Grounds = Chunk.GetMutableFragmentView<FBBBMonsterGroundFragment>();
        const auto Networks = Chunk.GetFragmentView<FBBBMonsterNetworkFragment>();
        const auto Movements = Chunk.GetFragmentView<FBBBMonsterMovementFragment>();
        const auto States = Chunk.GetFragmentView<FBBBMonsterBehaviorFragment>();
        const auto Mobility = Chunk.GetFragmentView<FBBBMonsterMobilityFragment>();
        const auto Health = Chunk.GetFragmentView<FBBBMonsterHealthFragment>();
        const auto Actors = Chunk.GetFragmentView<FMassActorFragment>();
        for (int32 Index = 0; Index < Chunk.GetNumEntities(); ++Index)
        {
            auto& Traversal = Traversals[Index];
            auto& Fact = Traversal.Fact;
            auto& Path = Paths[Index];
            auto& Ground = Grounds[Index];
            auto& Transform = Transforms[Index].GetMutableTransform();
            auto& Velocity = Velocities[Index].Value;
            const auto* Definition = Networks[Index].Definition.Get();
            if (!Definition)
            {
                continue;
            }
            const auto& Rule = Definition->Traversal;
            const auto& Body = Movements[Index];
            const uint8 RequiredParts = (1 << static_cast<uint8>(EBBBMonsterHitRegion::LeftArm))
                | (1 << static_cast<uint8>(EBBBMonsterHitRegion::RightArm));
            const bool bAble = Rule.bEnabled && Rule.IsValid() && !Mobility[Index].bCrawling
                && Health[Index].CurrentHealth > 0 && (Health[Index].DestroyedParts & RequiredParts) == 0;
            FCollisionQueryParams Params(SCENE_QUERY_STAT(BBBMonsterTraversal), false);
            if (Actors[Index].Get())
            {
                Params.AddIgnoredActor(Actors[Index].Get());
            }
            const FVector Offset(0, 0, Body.CapsuleHalfHeight + 1.0f);
            const auto Capsule = FCollisionShape::MakeCapsule(Body.CapsuleRadius - 0.5f, Body.CapsuleHalfHeight - 0.5f);
            const auto ClearMotion = [&](const FVector& From, const FVector& To)
            {
                FHitResult Hit;
                return !World->SweepSingleByChannel(Hit, From, To, FQuat::Identity, ECC_Pawn, Capsule, Params);
            };
            const auto Abort = [&](FName Reason)
            {
                Fact.Kind = 0;
                ++Fact.Serial;
                Traversal.Link.Reset();
                Traversal.Failure = Reason;
                Traversal.RetryAt = Now + Rule.RetrySeconds;
                Ground.bGrounded = false;
                Ground.UnsupportedSince = Now;
                Path.bHasPath = false;
                Path.bHasFollowTarget = false;
                Path.TraversalPoint = INDEX_NONE;
                Path.NextPathRefreshTime = Traversal.RetryAt;
                Velocity.Z = FMath::Min(Velocity.Z, 0.0);
            };
            if (Fact.Kind != 0)
            {
                if (!bAble || !Traversal.Link.IsValid() || States[Index].State == EBBBMonsterBehavior::Dead)
                {
                    Abort(TEXT("Interrupted"));
                    continue;
                }
                const float P = FMath::Clamp((Now - Fact.StartedAt) / Fact.Duration, 0.0f, 1.0f);
                const FVector Destination = EvaluateFeet(Fact, Rule, P) + Offset;
                const FVector Previous = Transform.GetLocation();
                const int32 Steps = FMath::Clamp(FMath::CeilToInt((Destination - Previous).Size() / 10.0f), 1, 32);
                bool bClear = true;
                FVector Last = Previous;
                for (int32 Step = 1; Step <= Steps; ++Step)
                {
                    const FVector Next = FMath::Lerp(Previous, Destination, static_cast<float>(Step) / Steps);
                    if (!ClearMotion(Last, Next))
                    {
                        bClear = false;
                        break;
                    }
                    Last = Next;
                }
                if (!bClear)
                {
                    Abort(TEXT("CorridorBlocked"));
                    continue;
                }
                Transform.SetLocation(Destination);
                Velocity = (Destination - Previous) / Context.GetDeltaTimeSeconds();
                Ground.bGrounded = false;
                Ground.UnsupportedSince = -1;
                if (P >= 1.0f)
                {
                    FHitResult Floor;
                    if (!World->LineTraceSingleByChannel(Floor, Fact.End + FVector(0, 0, 10), Fact.End - FVector(0, 0, 15), ECC_Pawn, Params)
                        || Floor.ImpactNormal.Z < Body.WalkableFloorZ)
                    {
                        Abort(TEXT("LandingLost"));
                        continue;
                    }
                    Fact.Kind = 0;
                    ++Fact.Serial;
                    Traversal.Link.Reset();
                    Ground.bGrounded = true;
                    Ground.SupportNormal = Floor.ImpactNormal;
                    Ground.UnsupportedSince = -1;
                    Velocity = FVector::ZeroVector;
                    Path.PathPointIndex = FMath::Min(Path.TraversalPoint + 2, Path.PathPoints.Num() - 1);
                    Path.TraversalPoint = INDEX_NONE;
                    if (Path.NativePath.IsValid())
                    {
                        const auto& Nodes = Path.NativePath->GetPathPoints();
                        for (int32 Point = Path.PathPointIndex; Point < Nodes.Num() - 1; ++Point)
                        {
                            if (Nodes[Point].CustomNavLinkId != FNavLinkId::Invalid)
                            {
                                Path.TraversalPoint = Point;
                                break;
                            }
                        }
                    }
                    Path.bHasFollowTarget = false;
                    Path.ProgressTime = Now;
                    Path.BlockedDuration = 0;
                    Path.NextPathRefreshTime = Now + 0.25f;
                }
                continue;
            }
            if (!bAble || Now < Traversal.RetryAt || !Ground.bGrounded || Mobility[Index].IsStaggering(Now)
                || Now < Mobility[Index].HitStopEndsAt || !Path.bHasPath || !Path.NativePath.IsValid()
                || Path.TraversalPoint == INDEX_NONE || !Path.NativePath->GetPathPoints().IsValidIndex(Path.TraversalPoint))
            {
                continue;
            }
            const auto& Node = Path.NativePath->GetPathPoints()[Path.TraversalPoint];
            auto* Interface = Navigation->GetCustomLink(Node.CustomNavLinkId);
            auto* Component = Interface ? Cast<UActorComponent>(Interface->GetLinkOwner()) : nullptr;
            auto* Link = Interface ? Cast<ABBBMonsterClimbLinkActor>(Interface->GetLinkOwner()) : nullptr;
            if (!Link && Component)
            {
                Link = Cast<ABBBMonsterClimbLinkActor>(Component->GetOwner());
            }
            if (!Link)
            {
                Traversal.Failure = TEXT("UnknownLink");
                continue;
            }
            const FVector Feet = Transform.GetLocation() - Offset;
            const FVector Entry = Link->GetEntry();
            if (FVector::Dist2D(Feet, Entry) > Rule.EntryTolerance || FMath::Abs(Feet.Z - Entry.Z) > Body.MaxStepHeight + 5)
            {
                continue;
            }
            if (Checks >= Budget->MaxTraversalChecksPerFrame
                || (Checks > 0 && (FPlatformTime::Seconds() - CheckStart) * 1000.0 > Budget->TraversalTimeBudgetMs))
            {
                continue;
            }
            ++Checks;
            const FVector End = Link->GetExit();
            const float Height = End.Z - Entry.Z;
            if (Height <= Body.MaxStepHeight || Height > Rule.MaximumHeight)
            {
                Traversal.Failure = TEXT("HeightOutsideRange");
                Traversal.RetryAt = Now + Rule.RetrySeconds;
                continue;
            }
            if (Occupied.ContainsByPredicate([&](const FVector& Point)
                { return FVector::DistSquared(Point, Entry) < FMath::Square(Body.CapsuleRadius * 2 + 10)
                    || FVector::DistSquared(Point, End) < FMath::Square(Body.CapsuleRadius * 2 + 10); }))
            {
                Traversal.Failure = TEXT("WaitingForEntry");
                Traversal.RetryAt = Now + 0.15f;
                Path.ProgressTime = Now;
                Path.BlockedDuration = 0;
                continue;
            }
            const FVector Direction = (End - Entry).GetSafeNormal2D();
            const float Angle = FMath::Abs(FMath::FindDeltaAngleDegrees(Transform.Rotator().Yaw, Direction.Rotation().Yaw));
            if (Angle > 8.0f)
            {
                Transform.SetRotation(UBBBMonsterLocomotionProcessor::TurnTowards(Transform.GetRotation(), Direction,
                    Context.GetDeltaTimeSeconds(), Definition->RunTurnRate));
                Path.ProgressTime = Now;
                Traversal.Failure = TEXT("AligningEntry");
                continue;
            }
            FBBBMonsterTraversalFact Proposed;
            Proposed.Kind = Height <= Rule.MantleHeight ? 1 : 2;
            Proposed.Serial = Fact.Serial + 1;
            Proposed.StartedAt = Now;
            Proposed.Start = Feet;
            Proposed.End = End;
            Proposed.Ledge = Link->GetLedge();
            const float Variation = 0.94f + (GetTypeHash(Chunk.GetEntity(Index).Index) % 13) * 0.01f;
            Proposed.Duration = (Proposed.Kind == 1 ? Rule.MantleSeconds : Rule.ClimbSeconds) * Variation;
            const auto& Samples = Proposed.Kind == 1 ? Rule.MantleTrajectory : Rule.ClimbTrajectory;
            bool bClear = Samples.Num() >= 2;
            FVector Last = Transform.GetLocation();
            for (int32 Sample = 1; bClear && Sample <= 24; ++Sample)
            {
                const FVector Next = EvaluateFeet(Proposed, Rule, Sample / 24.0f) + Offset;
                bClear = ClearMotion(Last, Next);
                Last = Next;
            }
            FHitResult Floor;
            bClear = bClear && World->LineTraceSingleByChannel(Floor, End + FVector(0, 0, 10), End - FVector(0, 0, 15), ECC_Pawn, Params)
                && Floor.ImpactNormal.Z >= Body.WalkableFloorZ;
            if (!bClear)
            {
                Traversal.Failure = TEXT("NoSafeCorridor");
                Traversal.RetryAt = Now + Rule.RetrySeconds;
                continue;
            }
            Fact = Proposed;
            Traversal.Link = Link;
            Traversal.Failure = NAME_None;
            Occupied.Add(Entry);
            Occupied.Add(End);

            Velocity = FVector::ZeroVector;
            Ground.bGrounded = false;
            Ground.UnsupportedSince = -1;
        }
    });
    if (DrawTraversal.GetValueOnGameThread() != 0)
    {
        MonsterQuery.ForEachEntityChunk(Context, [&](FMassExecutionContext& Chunk)
        {
            const auto Traversals = Chunk.GetFragmentView<FBBBMonsterTraversalFragment>();
            const auto Networks = Chunk.GetFragmentView<FBBBMonsterNetworkFragment>();
            const auto Transforms = Chunk.GetFragmentView<FTransformFragment>();
            for (int32 Index = 0; Index < Chunk.GetNumEntities(); ++Index)
            {
                const auto& T = Traversals[Index];
                if (T.Fact.Kind == 0)
                {
                    if (DrawTraversal.GetValueOnGameThread() == 1 && !T.Failure.IsNone())
                    {
                        DrawDebugString(World, Transforms[Index].GetTransform().GetLocation(), T.Failure.ToString(), nullptr, FColor::Orange, 0, true);
                    }
                    continue;
                }
                const auto* D = Networks[Index].Definition.Get();
                if (!D)
                {
                    continue;
                }
                FVector Previous = T.Fact.Start;
                for (int32 Sample = 1; Sample <= 24; ++Sample)
                {
                    const FVector Next = EvaluateFeet(T.Fact, D->Traversal, Sample / 24.0f);
                    DrawDebugLine(World, Previous, Next, FColor::Cyan, false, 0, 0, 2);
                    Previous = Next;
                }
                DrawDebugSphere(World, T.Fact.Ledge, 7, 8, FColor::Red, false, 0);
            }
        });
    }
}
