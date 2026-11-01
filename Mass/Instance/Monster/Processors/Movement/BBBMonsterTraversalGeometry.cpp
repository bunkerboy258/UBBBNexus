#include "BBBMonsterTraversalGeometry.h"

#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Movement/BBBMonsterMovementFragment.h"
#include "NavigationSystem.h"
#include "Engine/World.h"

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
    const FVector Raised(Feet.X, Feet.Y, Ledge.Z + 12);
    FHitResult Block;
    if (World.SweepSingleByChannel(Block, Feet + Offset, Raised + Offset, FQuat::Identity, ECC_Pawn, Capsule, Params))
    {
        return false;
    }
    for (int32 Sample = 0; Sample < 8; ++Sample)
    {
        const FVector Across = Ledge + Inward * (Body.CapsuleRadius + 15 + Sample * 40);
        const FVector Above(Across.X, Across.Y, Ledge.Z + 12);
        if (World.SweepSingleByChannel(Block, Raised + Offset, Above + Offset, FQuat::Identity, ECC_Pawn, Capsule, Params))
        {
            continue;
        }
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
        const FVector LandingAbove(Landing.X, Landing.Y, Ledge.Z + 12);
        if (World.SweepSingleByChannel(Block, Raised + Offset, LandingAbove + Offset, FQuat::Identity, ECC_Pawn, Capsule, Params)
            || World.SweepSingleByChannel(Block, LandingAbove + Offset, Landing + Offset, FQuat::Identity, ECC_Pawn, Capsule, Params))
        {
            continue;
        }
        Exit = Landing;
        return true;
    }
    return false;
}
