#include "BBBWork/UBBBNexus/Mass/Instance/Projectile/Traits/BBBProjectileTrait.h"

#include "MassEntityTemplateRegistry.h"
#include "MassCommonFragments.h"
#include "MassMovementFragments.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Projectile/Tags/BBBProjectileTag.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Projectile/Fragments/Spawn/BBBProjectileSpawnInputFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Projectile/Fragments/Movement/BBBProjectileMotionFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Projectile/Fragments/Collision/BBBProjectileCollisionFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Projectile/Fragments/Lifetime/BBBProjectileLifetimeFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Projectile/Fragments/Presentation/BBBProjectilePresentationFragment.h"

void UBBBProjectileTrait::BuildTemplate(FMassEntityTemplateBuildContext& BuildContext, const UWorld& World) const
{
    BuildContext.AddTag<FBBBProjectileTag>();
    BuildContext.AddTag<FMassCustomMovementTag>();
    BuildContext.AddFragment<FTransformFragment>();
    BuildContext.AddFragment<FMassVelocityFragment>();
    BuildContext.AddFragment<FBBBProjectileSpawnInputFragment>();
    BuildContext.AddFragment<FBBBProjectileMotionFragment>();
    BuildContext.AddFragment<FBBBProjectileCollisionFragment>();
    BuildContext.AddFragment<FBBBProjectileLifetimeFragment>();
    BuildContext.AddFragment<FBBBProjectilePresentationFragment>();
}
