#include "BBBWork/UBBBNexus/Mass/Instance/Projectile/Config/BBBProjectileDefinition.h"

#include "MassEntityConfigAsset.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Projectile/Traits/BBBProjectileTrait.h"

bool UBBBProjectileDefinition::IsValid() const
{
    const UBBBProjectileTrait* ProjectileTrait = nullptr;

    if (EntityConfig != nullptr)
    {
        ProjectileTrait = Cast<const UBBBProjectileTrait>(EntityConfig->FindTrait(UBBBProjectileTrait::StaticClass()));
    }

    return ProjectileTrait != nullptr
        && ProjectilesPerShot >= 1 && ProjectilesPerShot <= 64
        && FMath::IsFinite(SpreadHalfAngleDegrees) && SpreadHalfAngleDegrees >= 0.0f && SpreadHalfAngleDegrees <= 90.0f
        && FMath::IsFinite(GravityScale) && GravityScale >= 0.0f
        && FMath::IsFinite(ExplosionRadiusCm) && ExplosionRadiusCm >= 0.0f
        && FMath::IsFinite(FuseSeconds) && FuseSeconds >= 0.0f && FuseSeconds <= MaximumLifetimeSeconds
        && (FuseSeconds == 0.0f || ExplosionRadiusCm > 0.0f)
        && (ExplosionRadiusCm == 0.0f || (MaximumPenetrations == 0 && (bDetonateOnImpact || FuseSeconds > 0.0f)))
        && FMath::IsFinite(BounceRestitution) && BounceRestitution >= 0.0f && BounceRestitution <= 1.0f
        && !MeshRelativeTransform.ContainsNaN()
        && FMath::IsFinite(InitialSpeedCmPerSecond)
        && InitialSpeedCmPerSecond > 0.0f
        && FMath::IsFinite(MaximumLifetimeSeconds)
        && MaximumLifetimeSeconds > 0.0f
        && FMath::IsFinite(CollisionRadiusCm)
        && CollisionRadiusCm >= 0.0f
        && FMath::IsFinite(BaseDamage)
        && BaseDamage >= 0.0f
        && FMath::IsFinite(DurableDamage) && DurableDamage >= 0.0f
        && MaximumPenetrations >= 0
        && MaximumPenetrations <= 32
        && FMath::IsFinite(PenetrationDamageMultiplier)
        && PenetrationDamageMultiplier >= 0.0f
        && PenetrationDamageMultiplier <= 1.0f
        && (Mesh != nullptr || (PresentationChannel != nullptr && PresentationSystem != nullptr))
        && ((PresentationChannel != nullptr) == (PresentationSystem != nullptr))
        && FMath::IsFinite(TracerLengthCm)
        && TracerLengthCm > 0.0f
        && FMath::IsFinite(TracerWidthCm)
        && TracerWidthCm > 0.0f
        && FMath::IsFinite(TracerColor.R)
        && FMath::IsFinite(TracerColor.G)
        && FMath::IsFinite(TracerColor.B)
        && FMath::IsFinite(TracerColor.A);
}
