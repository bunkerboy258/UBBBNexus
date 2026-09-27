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
        && FMath::IsFinite(InitialSpeedCmPerSecond)
        && InitialSpeedCmPerSecond > 0.0f
        && FMath::IsFinite(MaximumLifetimeSeconds)
        && MaximumLifetimeSeconds > 0.0f
        && FMath::IsFinite(CollisionRadiusCm)
        && CollisionRadiusCm >= 0.0f
        && FMath::IsFinite(BaseDamage)
        && BaseDamage >= 0.0f
        && MaximumPenetrations >= 0
        && MaximumPenetrations <= 32
        && FMath::IsFinite(PenetrationDamageMultiplier)
        && PenetrationDamageMultiplier >= 0.0f
        && PenetrationDamageMultiplier <= 1.0f;
}
