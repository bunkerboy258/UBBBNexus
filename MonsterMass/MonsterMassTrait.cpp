#include "BBBWork/UBBBNexus/MonsterMass/MonsterMassTrait.h"

#include "MassCommonFragments.h"
#include "MassEntityTemplateRegistry.h"
#include "BBBWork/UBBBNexus/MonsterMass/MonsterMassFragments.h"

void UMonsterMassTrait::BuildTemplate(FMassEntityTemplateBuildContext& BuildContext, const UWorld& World) const
{
    BuildContext.AddTag<FMonsterTag>();
    BuildContext.AddFragment<FTransformFragment>();
    BuildContext.AddFragment<FMonsterHealthFragment>();
    BuildContext.AddFragment<FMonsterStateFragment>();
    BuildContext.AddFragment<FMonsterTargetFragment>();
    BuildContext.AddFragment<FMonsterMovementFragment>();
}
