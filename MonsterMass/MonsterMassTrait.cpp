#include "BBBWork/UBBBNexus/MonsterMass/MonsterMassTrait.h"

#include "MassCommonFragments.h"
#include "MassMovementFragments.h"
#include "MassEntityTemplateRegistry.h"
#include "BBBWork/UBBBNexus/MonsterMass/MonsterRuntimeData.h"

void UMonsterMassTrait::BuildTemplate(FMassEntityTemplateBuildContext& BuildContext, const UWorld& World) const
{
    if (!ensureMsgf(RuntimeData != nullptr, TEXT("[UBBBM]Monster trait requires MonsterRuntimeData")))
    {
        return;
    }

    BuildContext.AddTag<FMonsterTag>();
    BuildContext.AddFragment<FTransformFragment>();
    BuildContext.AddFragment<FMassVelocityFragment>();
    BuildContext.AddFragment(FConstStructView::Make(RuntimeData->Health));
    BuildContext.AddFragment<FMonsterStateFragment>();
    BuildContext.AddFragment<FMonsterTargetRequestFragment>();
    BuildContext.AddFragment(FConstStructView::Make(RuntimeData->Movement));
    BuildContext.AddFragment(FConstStructView::Make(RuntimeData->Perception));
    BuildContext.AddFragment(FConstStructView::Make(RuntimeData->Combat));
    BuildContext.AddFragment(FConstStructView::Make(RuntimeData->Avoidance));
    BuildContext.AddFragment<FMonsterDamageEventFragment>();
    BuildContext.AddFragment<FMonsterDeathEventFragment>();
    BuildContext.AddFragment<FMonsterPresentationStateFragment>();
}
