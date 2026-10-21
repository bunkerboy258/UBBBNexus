#include "BBBMonsterSpawnerTrait.h"
#include "MassEntityTemplateRegistry.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Spawn/BBBMonsterSpawnInputFragment.h"

void UBBBMonsterSpawnerTrait::BuildTemplate(FMassEntityTemplateBuildContext& BuildContext, const UWorld& World) const
{
    BuildContext.AddFragment<FBBBMonsterSpawnFragment>();
    BuildContext.AddFragment<FBBBMonsterSpawnInputFragment>();
}
