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
