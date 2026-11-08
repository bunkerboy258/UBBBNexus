#include "BBBMonsterSteering.h"

#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Config/BBBMonsterDefinition.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Movement/BBBMonsterMovementFragment.h"

FVector FBBBMonsterSteering::ApplyTravelCurve(const FVector& GoalDelta, const UBBBMonsterDefinition& Rule,
    const FGuid& InstanceId, const float Now, const float RemainingDistance, const float ArrivalDistance,
    const float BodyRadius, const bool bRunning)
{
    const FVector Direction = GoalDelta.GetSafeNormal2D();
    const float GaitScale = bRunning ? Rule.NavigationCurveRunScale : 1.0f;
    if (Direction.IsNearlyZero() || Rule.NavigationCurveAngle <= 0.0f || GaitScale <= 0.0f)
    {
        return Direction;
    }

    const float EndFade = FMath::SmoothStep(ArrivalDistance,
        ArrivalDistance + Rule.NavigationCurveFadeDistance, RemainingDistance);
    const float CornerFade = FMath::SmoothStep(BodyRadius * 2.0f, BodyRadius * 6.0f, static_cast<float>(GoalDelta.Size2D()));
    const float Strength = EndFade * CornerFade * GaitScale;
    if (Strength <= SMALL_NUMBER)
    {
        return Direction;
    }

    // 身份决定相位与周期 不随重算路径重置 不增加逐实体的随机数状态
    const uint32 Seed = GetTypeHash(InstanceId);
    const float Phase = (Seed & 0xffffu) * (2.0f * PI / 65536.0f);
    const float PeriodScale = 0.85f + (Seed >> 16) * (0.3f / 65535.0f);
    const float Time = Now * (2.0f * PI / (Rule.NavigationCurvePeriod * PeriodScale));
    const float Wave = 0.8f * FMath::Sin(Time + Phase) + 0.2f * FMath::Sin(Time * 0.53f + Phase * 1.7f);
    const float Angle = FMath::DegreesToRadians(Rule.NavigationCurveAngle) * Wave * Strength;
    float SinAngle;
    float CosAngle;
    FMath::SinCos(&SinAngle, &CosAngle, Angle);
    return FVector(Direction.X * CosAngle - Direction.Y * SinAngle,
        Direction.X * SinAngle + Direction.Y * CosAngle, 0.0f);
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
