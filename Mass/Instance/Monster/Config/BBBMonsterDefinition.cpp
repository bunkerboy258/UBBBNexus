#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Config/BBBMonsterDefinition.h"

bool UBBBMonsterDefinition::IsValid() const
{
    return EntityConfig != nullptr
        && FMath::IsFinite(MaxHealth) && MaxHealth > 0.0f
        && FMath::IsFinite(HurtDuration) && HurtDuration > 0.0f
        && FMath::IsFinite(DeathLifetime) && DeathLifetime > 0.0f
        && FMath::IsFinite(WalkSpeed) && WalkSpeed > 0.0f
        && FMath::IsFinite(RunSpeed) && RunSpeed > WalkSpeed
        && FMath::IsFinite(SprintSpeed) && SprintSpeed > RunSpeed
        && FMath::IsFinite(Acceleration) && Acceleration > 0.0f
        && FMath::IsFinite(Deceleration) && Deceleration > 0.0f
        && FMath::IsFinite(SprintDistance) && SprintDistance > 0.0f
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
