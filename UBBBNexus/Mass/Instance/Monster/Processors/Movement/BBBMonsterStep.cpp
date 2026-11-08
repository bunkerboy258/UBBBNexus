#include "BBBMonsterStep.h"

#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Movement/BBBMonsterMovementFragment.h"
#include "Engine/World.h"

bool FBBBMonsterStep::Solve(UWorld& World, const FBBBMonsterMovementFragment& Body,
    const FVector& Start, const FVector& Delta, const FCollisionQueryParams& Params,
    FVector& Center, FVector& Normal)
{
    if (Body.MaxStepHeight <= 0 || Delta.IsNearlyZero())
    {
        return false;
    }
    const auto Capsule = FCollisionShape::MakeCapsule(Body.CapsuleRadius, Body.CapsuleHalfHeight);
    const auto Support = FCollisionShape::MakeCapsule(Body.CapsuleRadius - 0.5f, Body.CapsuleHalfHeight - 0.5f);
    const FVector Raised = Start + FVector::UpVector * (Body.MaxStepHeight + 0.5f);
    const FVector Candidate = Raised + FVector(Delta.X, Delta.Y, 0);
    FHitResult Hit;
    if (World.SweepSingleByChannel(Hit, Start, Raised, FQuat::Identity, ECC_Pawn, Capsule, Params)
        || World.SweepSingleByChannel(Hit, Raised, Candidate, FQuat::Identity, ECC_Pawn, Capsule, Params))
    {
        return false;
    }
    if (!World.SweepSingleByChannel(Hit, Candidate + FVector(0, 0, 0.5f),
        Candidate - FVector(0, 0, Body.MaxStepHeight + 4), FQuat::Identity, ECC_Pawn, Support, Params)
        || !Hit.IsValidBlockingHit() || Hit.ImpactNormal.Z < Body.WalkableFloorZ || Hit.Normal.Z <= KINDA_SMALL_NUMBER
        || Hit.ImpactPoint.Z > Start.Z - Body.CapsuleHalfHeight + Body.MaxStepHeight + 0.5f)
    {
        return false;
    }
    Center = Hit.Location + FVector::UpVector * (0.5f + 0.5f / Hit.Normal.Z);
    Normal = Hit.ImpactNormal;
    return Center.Z <= Start.Z + Body.MaxStepHeight + 1.5f;
}
