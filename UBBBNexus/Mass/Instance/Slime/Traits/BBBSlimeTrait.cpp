#include "BBBSlimeTrait.h"
#include "MassEntityTemplateRegistry.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Slime/Fragments/BBBSlimeInputFragment.h"

void UBBBSlimeTrait::BuildTemplate(FMassEntityTemplateBuildContext& BuildContext, const UWorld&) const
{
    BuildContext.AddFragment<FBBBSlimeFragment>();
    BuildContext.AddFragment<FBBBSlimeInputFragment>();
}
