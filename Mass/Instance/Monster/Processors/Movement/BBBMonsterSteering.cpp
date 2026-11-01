#include "BBBMonsterSteering.h"

#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Movement/BBBMonsterMovementFragment.h"

FVector FBBBMonsterSteering::ConstrainDirection(const FVector& Velocity, const FVector& Direction,
    const float Acceleration, const float TurnRate, const float DeltaSeconds)
{
    const FVector Desired = Direction.GetSafeNormal2D();
    const float Speed = Velocity.Size2D();
    if (Desired.IsNearlyZero() || Speed <= 10.0f)
    {
        return Desired;
    }
    const float Rate = FMath::Min(TurnRate, FMath::RadiansToDegrees(Acceleration / Speed));
    const float Yaw = Velocity.Rotation().Yaw;
    const float Step = FMath::Clamp(FMath::FindDeltaAngleDegrees(Yaw, Desired.Rotation().Yaw),
        -Rate * FMath::Max(DeltaSeconds, 0.0f), Rate * FMath::Max(DeltaSeconds, 0.0f));
    return FRotator(0.0f, Yaw + Step, 0.0f).Vector();
}

void FBBBMonsterSteering::UpdateFacingMode(FBBBMonsterMovementFragment& Movement, const bool bHasTarget,
    const bool bForceTarget, const bool bChasing, const bool bCrowdStationary, const float Distance,
    const float AttackRange, const float BodyRadius, const float Now)
{
    if (!bHasTarget)
    {
        Movement.bFacingTarget = false;
        Movement.CrowdStationarySince = -1.0f;
        Movement.FacingModeChangedAt = Now;
        return;
    }
    if (!bCrowdStationary)
    {
        Movement.CrowdStationarySince = -1.0f;
    }
    if (bCrowdStationary && Movement.CrowdStationarySince < 0.0f)
    {
        Movement.CrowdStationarySince = Now;
    }
    const bool bWaiting = Movement.CrowdStationarySince >= 0.0f && Now - Movement.CrowdStationarySince >= 0.25f;
    const float ExitMargin = Movement.bFacingTarget ? BodyRadius : 0.0f;
    const bool bDesired = bForceTarget || (bChasing && (Distance <= AttackRange + ExitMargin || bWaiting));
    if (bDesired != Movement.bFacingTarget && (bForceTarget || Movement.FacingModeChangedAt < 0.0f
        || Now - Movement.FacingModeChangedAt >= 0.35f))
    {
        Movement.bFacingTarget = bDesired;
        Movement.FacingModeChangedAt = Now;
    }
}
