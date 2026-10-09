#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Config/BBBMonsterDefinition.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Config/BBBMonsterVariationDefinition.h"

UBBBMonsterDefinition::UBBBMonsterDefinition()
{
    TorsoPart = {1.0f, 0.25f, 1.0f, true};
    HeadPart = {0.35f, 0.0f, 1.0f, true};
    LeftArmPart = {0.4f, 0.5f, 0.25f, false};
    RightArmPart = LeftArmPart;
    LeftLegPart = {0.35f, 0.35f, 0.3f, false};
    RightLegPart = LeftLegPart;
}

const FBBBMonsterBodyPartDefinition& UBBBMonsterDefinition::GetBodyPart(const EBBBMonsterHitRegion Part) const
{
    switch (Part)
    {
        case EBBBMonsterHitRegion::Head:
            return HeadPart;
        case EBBBMonsterHitRegion::LeftArm:
            return LeftArmPart;
        case EBBBMonsterHitRegion::RightArm:
            return RightArmPart;
        case EBBBMonsterHitRegion::LeftLeg:
            return LeftLegPart;
        case EBBBMonsterHitRegion::RightLeg:
            return RightLegPart;
        default:
            return TorsoPart;
    }
}

bool UBBBMonsterDefinition::IsValid() const
{
    return EntityConfig != nullptr
        && Variation != nullptr && Variation->IsValid()
        && FMath::IsFinite(MaxHealth) && MaxHealth > 0.0f
        && TorsoPart.IsValid() && HeadPart.IsValid() && LeftArmPart.IsValid() && RightArmPart.IsValid()
        && LeftLegPart.IsValid() && RightLegPart.IsValid()
        && FMath::IsFinite(HeavyHitFraction) && HeavyHitFraction > 0.0f && HeavyHitFraction <= 1.0f
        && FMath::IsFinite(SuppressionRecovery) && SuppressionRecovery > 0.0f
        && FMath::IsFinite(OneArmAttackRatio) && OneArmAttackRatio >= 0.0f && OneArmAttackRatio <= 1.0f
        && FMath::IsFinite(WalkTurnRate) && WalkTurnRate > 0.0f
        && FMath::IsFinite(RunTurnRate) && RunTurnRate > 0.0f
        && FMath::IsFinite(CrawlTurnRate) && CrawlTurnRate > 0.0f
        && FMath::IsFinite(CorpseLifetime) && CorpseLifetime >= 5.0f
        && FMath::IsFinite(DeathAnimationDuration) && DeathAnimationDuration > 0.0f && DeathAnimationDuration < CorpseLifetime
        && FMath::IsFinite(CorpseSimulationDuration) && CorpseSimulationDuration >= 0.5f && CorpseSimulationDuration < CorpseLifetime
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
