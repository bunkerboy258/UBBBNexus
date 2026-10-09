#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Config/BBBMonsterDefinition.h"

bool UBBBMonsterDefinition::IsValid() const
{
    return EntityConfig != nullptr
        && FMath::IsFinite(MaxHealth) && MaxHealth > 0.0f
        && FMath::IsFinite(DeathLifetime) && DeathLifetime > 0.0f
        && FMath::IsFinite(WalkSpeed) && WalkSpeed > 0.0f
        && FMath::IsFinite(RunSpeed) && RunSpeed > WalkSpeed
        && FMath::IsFinite(SprintSpeed) && SprintSpeed > RunSpeed
        && FMath::IsFinite(Acceleration) && Acceleration > 0.0f
        && FMath::IsFinite(Deceleration) && Deceleration > 0.0f
        && FMath::IsFinite(SprintDistance) && SprintDistance > 0.0f
        && FMath::IsFinite(BodyHitSpeedRatio) && BodyHitSpeedRatio >= 0.1f && BodyHitSpeedRatio <= 1.0f
        && FMath::IsFinite(ArmHitSpeedRatio) && ArmHitSpeedRatio >= 0.1f && ArmHitSpeedRatio <= 1.0f
        && FMath::IsFinite(LegHitSpeedRatio) && LegHitSpeedRatio >= 0.1f && LegHitSpeedRatio <= 1.0f
        && FMath::IsFinite(BodyHitSlowDuration) && BodyHitSlowDuration > 0.0f
        && FMath::IsFinite(ArmHitSlowDuration) && ArmHitSlowDuration > 0.0f
        && FMath::IsFinite(LegHitSlowDuration) && LegHitSlowDuration > 0.0f
        && FMath::IsFinite(HitSlowHoldDuration) && HitSlowHoldDuration >= 0.0f
        && FMath::IsFinite(HitStopDuration) && HitStopDuration >= 0.0f && HitStopDuration <= 0.2f
        && FMath::IsFinite(StaggerDuration) && StaggerDuration >= 0.5f && StaggerDuration <= 1.5f
        && FMath::IsFinite(CrawlLegDamageFraction) && CrawlLegDamageFraction > 0.0f && CrawlLegDamageFraction <= 1.0f
        && FMath::IsFinite(CrawlSpeed) && CrawlSpeed > 0.0f && CrawlSpeed <= WalkSpeed
        && FMath::IsFinite(CrawlCapsuleHalfHeight) && CrawlCapsuleHalfHeight >= CollisionRadius && CrawlCapsuleHalfHeight <= CapsuleHalfHeight
        && FMath::IsFinite(CrawlTransitionDuration) && CrawlTransitionDuration > 0.0f
        && FMath::IsFinite(CrawlMaxStepHeight) && CrawlMaxStepHeight >= 0.0f && CrawlMaxStepHeight < CrawlCapsuleHalfHeight
        && FMath::IsFinite(IdleDurationMin) && IdleDurationMin > 0.0f
        && FMath::IsFinite(IdleDurationMax) && IdleDurationMax >= IdleDurationMin
        && FMath::IsFinite(PatrolDurationMin) && PatrolDurationMin > 0.0f
        && FMath::IsFinite(PatrolDurationMax) && PatrolDurationMax >= PatrolDurationMin
        && FMath::IsFinite(AlertDuration) && AlertDuration > 0.0f
        && FMath::IsFinite(SightRange) && SightRange >= 0.0f
        && FMath::IsFinite(SightAngle) && SightAngle > 0.0f && SightAngle <= 180.0f
        && FMath::IsFinite(SightConfirmMin) && SightConfirmMin > 0.0f
        && FMath::IsFinite(SightConfirmMax) && SightConfirmMax >= SightConfirmMin
        && FMath::IsFinite(AwarenessDecayDuration) && AwarenessDecayDuration > 0.0f
        && FMath::IsFinite(TargetMemoryDuration) && TargetMemoryDuration > 0.0f
        && FMath::IsFinite(TargetLeashDistance) && TargetLeashDistance >= SightRange
        && FMath::IsFinite(TargetSwitchRatio) && TargetSwitchRatio > 0.0f && TargetSwitchRatio < 1.0f
        && FMath::IsFinite(TargetSwitchAdvantage) && TargetSwitchAdvantage >= 0.0f
        && FMath::IsFinite(TargetSwitchDuration) && TargetSwitchDuration >= 0.0f
        && FMath::IsFinite(AllyAlertRange) && AllyAlertRange >= 0.0f
        && FMath::IsFinite(AllyAlertCooldown) && AllyAlertCooldown > 0.0f
        && FMath::IsFinite(InvestigationAlertDuration) && InvestigationAlertDuration > 0.0f
        && FMath::IsFinite(NavigationStuckDuration) && NavigationStuckDuration > 0.0f
        && FMath::IsFinite(NavigationRetryDuration) && NavigationRetryDuration > 0.0f
        && FMath::IsFinite(EncircleRange) && EncircleRange > 0.0f
        && NavigationFailureLimit >= 1 && NavigationFailureLimit <= 10
        && FMath::IsFinite(CollisionRadius) && CollisionRadius > 0.0f
        && FMath::IsFinite(CapsuleHalfHeight) && CapsuleHalfHeight >= CollisionRadius
        && FMath::IsFinite(MaxStepHeight) && MaxStepHeight >= 0.0f && MaxStepHeight < CapsuleHalfHeight
        && FMath::IsFinite(MaxWalkableSlopeAngle) && MaxWalkableSlopeAngle >= 0.0f && MaxWalkableSlopeAngle <= 80.0f
        && FMath::IsFinite(PersonalSpaceRadius) && PersonalSpaceRadius >= 0.0f
        && FMath::IsFinite(NeighborSearchRadius) && NeighborSearchRadius >= PersonalSpaceRadius
        && FMath::IsFinite(AvoidanceWeight) && AvoidanceWeight >= 0.0f
        && FMath::IsFinite(AttackRange) && AttackRange > 0.0f
        && FMath::IsFinite(AttackDamage) && AttackDamage >= 0.0f
        && FMath::IsFinite(AttackCooldown) && AttackCooldown >= 0.0f
        && FMath::IsFinite(AttackWindup) && AttackWindup > 0.0f
        && FMath::IsFinite(AttackRecovery) && AttackRecovery > 0.0f
        && FMath::IsFinite(AnimationHitFraction) && AnimationHitFraction > 0.0f && AnimationHitFraction < 1.0f;
}
