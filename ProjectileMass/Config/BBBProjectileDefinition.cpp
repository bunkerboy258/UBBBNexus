#include "BBBWork/UBBBNexus/ProjectileMass/Config/BBBProjectileDefinition.h"

#include "BBBWork/UBBBNexus/ProjectileMass/Entity/BBBProjectileMassConfigAsset.h"
#include "BBBWork/UBBBNexus/ProjectileMass/Entity/BBBProjectileMassTrait.h"

bool UBBBProjectileDefinition::IsValid() const
{
    const UBBBProjectileMassTrait* ProjectileTrait = nullptr;

    if (EntityConfig != nullptr)
    {
        ProjectileTrait = Cast<const UBBBProjectileMassTrait>(EntityConfig->FindTrait(UBBBProjectileMassTrait::StaticClass()));
    }

    return ProjectileTrait != nullptr
        && ProjectileTrait->StaticMeshInstanceDesc.IsValid()
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
