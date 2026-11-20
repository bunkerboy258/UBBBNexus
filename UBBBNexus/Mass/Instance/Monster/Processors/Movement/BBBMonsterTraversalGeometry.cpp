#include "BBBMonsterTraversalGeometry.h"

#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Movement/BBBMonsterMovementFragment.h"
#include "NavigationSystem.h"
#include "Engine/World.h"

namespace
{
    constexpr float LedgeClearance = 2.0f;
}

FVector FBBBMonsterTraversalGeometry::EvaluateFeet(const FVector& Start, const FVector& Ledge,
    const FVector& End, const float Radius, const float Progress)
{
    const FVector Direction = (End - Start).GetSafeNormal2D();
    const float Distance = FVector::Dist2D(Start, End);
    const float EdgeDistance = FVector::DotProduct(Ledge - Start, Direction);
    const float ApproachDistance = FMath::Clamp(EdgeDistance - Radius, 0.0f, Distance);
    const float ClearanceHeight = FMath::Max3(Start.Z, Ledge.Z, End.Z) + LedgeClearance;
    FVector Approach = Start + Direction * ApproachDistance;
    Approach.Z = ClearanceHeight;
    FVector AboveExit = End;
    AboveExit.Z = ClearanceHeight;

    const float Lift = FMath::SmoothStep(0.0f, 0.42f, Progress);
    const float Cross = FMath::SmoothStep(0.42f, 0.82f, Progress);
    const float Settle = FMath::SmoothStep(0.82f, 1.0f, Progress);
    return FMath::Lerp(Start, Approach, Lift) + (AboveExit - Approach) * Cross + (End - AboveExit) * Settle;
}

bool FBBBMonsterTraversalGeometry::IsPathClear(UWorld& World, const FVector& Start, const FVector& Ledge,
    const FVector& End, const FBBBMonsterMovementFragment& Body, const FCollisionQueryParams& Params)
{
    const FVector Offset(0, 0, Body.CapsuleHalfHeight + 1.0f);
    const auto Capsule = FCollisionShape::MakeCapsule(Body.CapsuleRadius - 0.5f, Body.CapsuleHalfHeight - 0.5f);
    FVector Last = Start + Offset;
    FHitResult Hit;
    for (int32 Sample = 1; Sample <= 30; ++Sample)
    {
        const FVector Next = EvaluateFeet(Start, Ledge, End, Body.CapsuleRadius, Sample / 30.0f) + Offset;
        if (World.SweepSingleByChannel(Hit, Last, Next, FQuat::Identity, ECC_Pawn, Capsule, Params))
        {
            return false;
        }
        Last = Next;
    }
    return World.LineTraceSingleByChannel(Hit, End + FVector(0, 0, 10), End - FVector(0, 0, 15), ECC_Pawn, Params)
        && Hit.ImpactNormal.Z >= Body.WalkableFloorZ;
}

bool FBBBMonsterTraversalGeometry::Find(UWorld& World, const FVector& Feet, const FVector& Direction,
    const FBBBMonsterMovementFragment& Body, const float MaxHeight, const FCollisionQueryParams& Params,
    FVector& Ledge, FVector& Exit)
{
    auto* Navigation = FNavigationSystem::GetCurrent<UNavigationSystemV1>(&World);
    const auto* Nav = Navigation ? Navigation->GetDefaultNavDataInstance() : nullptr;
    const FVector Forward = Direction.GetSafeNormal2D();
    if (!Nav || Forward.IsNearlyZero())
    {
        return false;
    }
    const FVector Offset(0, 0, Body.CapsuleHalfHeight + 1);
    const auto Capsule = FCollisionShape::MakeCapsule(Body.CapsuleRadius - 0.5f, Body.CapsuleHalfHeight - 0.5f);
    FHitResult Wall;
    if (!World.SweepSingleByChannel(Wall, Feet + Offset, Feet + Offset + Forward * (Body.CapsuleRadius * 2 + 60),
        FQuat::Identity, ECC_Pawn, Capsule, Params) || Wall.bStartPenetrating || FMath::Abs(Wall.ImpactNormal.Z) > 0.5f
        || !Wall.GetComponent() || Wall.GetComponent()->Mobility == EComponentMobility::Movable)
    {
        return false;
    }
    const FVector Inward = -Wall.ImpactNormal.GetSafeNormal2D();
    const FVector TopProbe = Wall.ImpactPoint + Inward * 8;
    FHitResult Top;
    if (!World.LineTraceSingleByChannel(Top, FVector(TopProbe.X, TopProbe.Y, Feet.Z + MaxHeight + 1),
        FVector(TopProbe.X, TopProbe.Y, Feet.Z + Body.MaxStepHeight), ECC_Pawn, Params)
        || Top.ImpactNormal.Z < Body.WalkableFloorZ)
    {
        return false;
    }
    const float Height = Top.ImpactPoint.Z - Feet.Z;
    if (Height <= Body.MaxStepHeight + 0.5f || Height > MaxHeight)
    {
        return false;
    }
    Ledge = FVector(Wall.ImpactPoint.X, Wall.ImpactPoint.Y, Top.ImpactPoint.Z);
    for (int32 Sample = 0; Sample < 8; ++Sample)
    {
        const FVector Across = Ledge + Inward * (Body.CapsuleRadius + 15 + Sample * 40);
        const FVector Above(Across.X, Across.Y, Ledge.Z + LedgeClearance);
        FHitResult Floor;
        const FVector Below(Across.X, Across.Y, Feet.Z - Body.MaxStepHeight - 5);
        if (!World.SweepSingleByChannel(Floor, Above + Offset, Below + Offset, FQuat::Identity, ECC_Pawn, Capsule, Params)
            || Floor.bStartPenetrating || Floor.ImpactNormal.Z < Body.WalkableFloorZ)
        {
            continue;
        }
        const FVector Candidate = Floor.Location - Offset + FVector(0, 0, 0.5f);
        FNavLocation Projected;
        if (!Navigation->ProjectPointToNavigation(Candidate, Projected,
            FVector(Body.CapsuleRadius * 0.5f, Body.CapsuleRadius * 0.5f, 20), Nav)
            || FMath::Abs(Projected.Location.Z - Candidate.Z) > 15
            || FVector::DistSquared2D(Projected.Location, Candidate) > FMath::Square(Body.CapsuleRadius * 0.5f))
        {
            continue;
        }
        FVector Landing = Projected.Location;
        FHitResult Support;
        if (!World.LineTraceSingleByChannel(Support, Landing + FVector(0, 0, 20), Landing - FVector(0, 0, 25), ECC_Pawn, Params)
            || Support.ImpactNormal.Z < Body.WalkableFloorZ)
        {
            continue;
        }
        Landing.Z = Support.ImpactPoint.Z;
        if (!IsPathClear(World, Feet, Ledge, Landing, Body, Params))
        {
            continue;
        }
        Exit = Landing;
        return true;
    }
    return false;
}
