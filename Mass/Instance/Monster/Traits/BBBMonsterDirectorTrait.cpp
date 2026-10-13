#include "BBBMonsterDirectorTrait.h"
#include "MassEntityTemplateRegistry.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Director/BBBMonsterDirectorInputFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Tags/BBBMonsterDirectorTag.h"

void UBBBMonsterDirectorTrait::BuildTemplate(FMassEntityTemplateBuildContext& BuildContext, const UWorld& World) const
{
    BuildContext.AddTag<FBBBMonsterDirectorTag>();
    BuildContext.AddFragment<FBBBMonsterDirectorFragment>();
    BuildContext.AddFragment<FBBBMonsterDirectorSpawnFragment>();
    BuildContext.AddFragment<FBBBMonsterDirectorInputFragment>();
}
