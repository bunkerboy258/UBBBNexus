#include "BBBWork/UBBBNexus/ProjectileMass/Entity/BBBProjectileMassTrait.h"

#include "MassCommonFragments.h"
#include "MassEntityTemplateRegistry.h"
#include "MassMovementFragments.h"
#include "BBBWork/UBBBNexus/ProjectileMass/Entity/BBBProjectileRuntimeFragment.h"
#include "BBBWork/UBBBNexus/ProjectileMass/Entity/BBBProjectileTag.h"

UBBBProjectileMassTrait::UBBBProjectileMassTrait()
{
    Params.LODRepresentation[EMassLOD::High] = EMassRepresentationType::StaticMeshInstance;
    Params.LODRepresentation[EMassLOD::Medium] = EMassRepresentationType::StaticMeshInstance;
    Params.LODRepresentation[EMassLOD::Low] = EMassRepresentationType::StaticMeshInstance;
    Params.LODRepresentation[EMassLOD::Off] = EMassRepresentationType::None;
}

void UBBBProjectileMassTrait::BuildTemplate(FMassEntityTemplateBuildContext& BuildContext, const UWorld& World) const
{
    BuildContext.AddTag<FBBBProjectileTag>();
    BuildContext.AddTag<FMassCustomMovementTag>();
    BuildContext.AddFragment<FTransformFragment>();
    BuildContext.AddFragment<FMassVelocityFragment>();
    BuildContext.AddFragment<FBBBProjectileRuntimeFragment>();

    Super::BuildTemplate(BuildContext, World);
}
