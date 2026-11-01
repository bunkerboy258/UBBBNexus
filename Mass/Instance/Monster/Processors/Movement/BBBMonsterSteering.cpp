#include "BBBMonsterSteering.h"

#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Movement/BBBMonsterMovementFragment.h"

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
