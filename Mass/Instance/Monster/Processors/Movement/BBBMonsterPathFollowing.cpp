#include "BBBMonsterPathFollowing.h"

#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Movement/BBBMonsterMovementFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Movement/BBBMonsterNavigationFragment.h"
#include "Engine/World.h"
#include "NavigationData.h"

namespace
{
    bool CanTraverse(UWorld& World, const ANavigationData& Navigation, const FVector& From, const FVector& To,
        const FBBBMonsterMovementFragment& Geometry, const AActor* IgnoredActor)
    {
        FVector Boundary;
        if (Navigation.Raycast(From, To, Boundary, Navigation.GetDefaultQueryFilter()))
        {
            return false;
        }

        FCollisionQueryParams Params(SCENE_QUERY_STAT(BBBMonsterFollow), false);
        if (IgnoredActor)
        {
            Params.AddIgnoredActor(IgnoredActor);
        }
        const auto FindFloor = [&World, &Geometry, &Params](const FVector& Feet, FHitResult& Floor)
        {
            const float Range = Geometry.MaxStepHeight + 12.0f;
            return World.LineTraceSingleByChannel(Floor, Feet + FVector::UpVector * Range,
                Feet - FVector::UpVector * Range, ECC_Pawn, Params)
                && Floor.ImpactNormal.Z >= Geometry.WalkableFloorZ
                && FMath::Abs(Floor.ImpactPoint.Z - Feet.Z) <= Range;
        };
        FHitResult PreviousFloor;
        if (!FindFloor(From, PreviousFloor))
        {
            return false;
        }
        const int32 FloorSamples = FMath::Clamp(FMath::CeilToInt(FVector::Dist2D(From, To)
            / FMath::Max(Geometry.CapsuleRadius, 30.0f)), 1, 10);
        for (int32 Sample = 1; Sample <= FloorSamples; ++Sample)
        {
            FHitResult Floor;
            if (!FindFloor(FMath::Lerp(From, To, static_cast<float>(Sample) / FloorSamples), Floor))
            {
                return false;
            }
            /** 平坦步面间的高度突变按当前姿态台阶处理 不能把斜向净空当成会飞 */
            if (PreviousFloor.ImpactNormal.Z > 0.99f && Floor.ImpactNormal.Z > 0.99f
                && Floor.ImpactPoint.Z - PreviousFloor.ImpactPoint.Z > Geometry.MaxStepHeight + 0.5f)
            {
                return false;
            }
            PreviousFloor = Floor;
        }

        const auto Capsule = FCollisionShape::MakeCapsule(FMath::Max(1.0f, Geometry.CapsuleRadius - 0.5f),
            FMath::Max(Geometry.CapsuleRadius, Geometry.CapsuleHalfHeight - 0.5f));
        const FVector Offset(0.0f, 0.0f, Geometry.CapsuleHalfHeight + 1.0f);
        const FVector Start = From + Offset;
        const FVector End = To + Offset;
        FHitResult Hit;
        if (!World.SweepSingleByChannel(Hit, Start, End, FQuat::Identity, ECC_Pawn, Capsule, Params))
        {
            return true;
        }
        if (Geometry.MaxStepHeight <= 0.0f)
        {
            return false;
        }
        const FVector Lift = FVector::UpVector * Geometry.MaxStepHeight;
        return !World.SweepSingleByChannel(Hit, Start, Start + Lift, FQuat::Identity, ECC_Pawn, Capsule, Params)
            && !World.SweepSingleByChannel(Hit, Start + Lift, End + Lift, FQuat::Identity, ECC_Pawn, Capsule, Params);
    }
}

bool FBBBMonsterPathFollowing::UpdateProgress(FBBBMonsterNavigationFragment& Path, const FVector& Feet,
    const float StepHeight, const bool bGrounded)
{
    if (!Path.bHasPath || !Path.PathPoints.IsValidIndex(Path.PathPointIndex)
        || !Path.TailDistances.IsValidIndex(Path.PathPointIndex))
    {
        return false;
    }
    const float HeightTolerance = FMath::Max(20.0f, StepHeight + 8.0f);
    constexpr float ArrivalDistance = 15.0f;
    /** 只允许进入已经通过身体通行检查的前瞻段 避免切角后仍被旧拐点拖回 */
    if (bGrounded && Path.bHasFollowTarget)
    {
        int32 BestSegment = Path.PathPointIndex;
        FVector Closest = FMath::ClosestPointOnSegment(Feet,
            Path.PathPoints[BestSegment - 1], Path.PathPoints[BestSegment]);
        double BestDistanceSquared = FVector::DistSquared2D(Feet, Closest);
        const int32 LastSegment = FMath::Min3(Path.FollowSegmentIndex,
            Path.PathPointIndex + 7, Path.PathPoints.Num() - 1);
        for (int32 Segment = Path.PathPointIndex + 1; Segment <= LastSegment; ++Segment)
        {
            Closest = FMath::ClosestPointOnSegment(Feet, Path.PathPoints[Segment - 1], Path.PathPoints[Segment]);
            const double DistanceSquared = FVector::DistSquared2D(Feet, Closest);
            if (FMath::Abs(Feet.Z - Closest.Z) <= HeightTolerance && DistanceSquared + 1.0 < BestDistanceSquared)
            {
                BestSegment = Segment;
                BestDistanceSquared = DistanceSquared;
            }
        }
        Path.PathPointIndex = BestSegment;
    }
    for (int32 Attempt = 0; Attempt < 8 && Path.PathPointIndex < Path.PathPoints.Num() - 1; ++Attempt)
    {
        const FVector& End = Path.PathPoints[Path.PathPointIndex];
        const FVector& Start = Path.PathPoints[FMath::Max(0, Path.PathPointIndex - 1)];
        const bool bNear = FVector::DistSquared2D(Feet, End) <= FMath::Square(ArrivalDistance);
        const bool bPassed = FVector::DotProduct(FVector(Feet.X - End.X, Feet.Y - End.Y, 0.0),
            FVector(End.X - Start.X, End.Y - Start.Y, 0.0)) > 0.0f;
        if (!bGrounded || FMath::Abs(Feet.Z - End.Z) > HeightTolerance || (!bNear && !bPassed))
        {
            break;
        }
        const FVector Closest = FMath::ClosestPointOnSegment(Feet, Start, End);
        if (!bNear && FVector::DistSquared2D(Feet, Closest) > FMath::Square(ArrivalDistance))
        {
            break;
        }
        ++Path.PathPointIndex;
    }

    const FVector& End = Path.PathPoints[Path.PathPointIndex];
    const FVector& Start = Path.PathPoints[FMath::Max(0, Path.PathPointIndex - 1)];
    const FVector Closest = FMath::ClosestPointOnSegment(Feet, Start, End);
    Path.OffPathDistance = FVector::Dist2D(Feet, Closest);
    Path.RemainingDistance = static_cast<float>(FVector::Distance(Closest, End)) + Path.TailDistances[Path.PathPointIndex];
    if (FMath::Abs(Feet.Z - Closest.Z) > HeightTolerance)
    {
        Path.Condition = EBBBMonsterNavigationCondition::HeightMismatch;
    }
    if (Path.bHasFollowTarget && (Path.PathPointIndex > Path.FollowSegmentIndex
        || FVector::DotProduct(Path.FollowTarget - Feet, End - Start) < 0.0f))
    {
        Path.bHasFollowTarget = false;
    }
    return bGrounded && FVector::DistSquared2D(Feet, Path.Destination) <= FMath::Square(ArrivalDistance)
        && FMath::Abs(Feet.Z - Path.Destination.Z) <= HeightTolerance;
}

void FBBBMonsterPathFollowing::RefreshTarget(UWorld& World, const ANavigationData& Navigation,
    FBBBMonsterNavigationFragment& Path, const FVector& Feet, const FBBBMonsterMovementFragment& Geometry,
    const float LookAhead, const AActor* IgnoredActor, const float Now)
{
    if (!Path.bHasPath || !Path.PathPoints.IsValidIndex(Path.PathPointIndex))
    {
        return;
    }
    const FVector& SegmentEnd = Path.PathPoints[Path.PathPointIndex];
    FVector Candidate = FMath::ClosestPointOnSegment(Feet,
        Path.PathPoints[FMath::Max(0, Path.PathPointIndex - 1)], SegmentEnd);
    float Remaining = LookAhead;
    int32 CandidateSegment = Path.PathPointIndex;
    for (int32 Segment = Path.PathPointIndex; Segment < Path.PathPoints.Num() && Segment < Path.PathPointIndex + 8; ++Segment)
    {
        const FVector& End = Path.PathPoints[Segment];
        const float Distance = FVector::Distance(Candidate, End);
        CandidateSegment = Segment;
        if (Remaining <= Distance)
        {
            Candidate = FMath::Lerp(Candidate, End, Remaining / FMath::Max(Distance, 0.01f));
            break;
        }
        Remaining -= Distance;
        Candidate = End;
    }

    if (CanTraverse(World, Navigation, Feet, Candidate, Geometry, IgnoredActor))
    {
        Path.FollowTarget = Candidate;
        Path.FollowSegmentIndex = CandidateSegment;
        Path.bHasFollowTarget = true;
        Path.bLocalBlocked = false;
        Path.Condition = EBBBMonsterNavigationCondition::Following;
        return;
    }
    if (CandidateSegment != Path.PathPointIndex && CanTraverse(World, Navigation, Feet, SegmentEnd, Geometry, IgnoredActor))
    {
        Path.FollowTarget = SegmentEnd;
        Path.FollowSegmentIndex = Path.PathPointIndex;
        Path.bHasFollowTarget = true;
        Path.bLocalBlocked = false;
        Path.Condition = EBBBMonsterNavigationCondition::Following;
        return;
    }

    const FVector Forward = (SegmentEnd - Feet).GetSafeNormal2D();
    const FVector Side(-Forward.Y, Forward.X, 0.0f);
    const int8 Preferred = Path.AvoidanceSide == 0 ? 1 : Path.AvoidanceSide;
    for (int32 Attempt = 0; Attempt < 2; ++Attempt)
    {
        if (Attempt > 0 && Now < Path.AvoidanceSideUntil)
        {
            break;
        }
        const int8 Choice = Attempt == 0 ? Preferred : -Preferred;
        Candidate = Feet + Forward * FMath::Min(LookAhead, 75.0f)
            + Side * Choice * (Geometry.CapsuleRadius * 1.5f + 15.0f);
        if (CanTraverse(World, Navigation, Feet, Candidate, Geometry, IgnoredActor))
        {
            Path.AvoidanceSide = Choice;
            Path.AvoidanceSideUntil = Now + 0.75f;
            Path.FollowTarget = Candidate;
            Path.FollowSegmentIndex = Path.PathPointIndex;
            Path.bHasFollowTarget = true;
            Path.bLocalBlocked = false;
            Path.Condition = EBBBMonsterNavigationCondition::Following;
            return;
        }
    }
    Path.FollowTarget = Feet;
    Path.FollowSegmentIndex = Path.PathPointIndex;
    Path.bHasFollowTarget = true;
    Path.bLocalBlocked = true;
    Path.Condition = EBBBMonsterNavigationCondition::Blocked;
}
