#include "BBBWork/UBBBNexus/MonsterMass/Entity/MonsterMassTrait.h"

#include "MassCommonFragments.h"
#include "MassMovementFragments.h"
#include "MassEntityTemplateRegistry.h"
#include "BBBWork/UBBBNexus/MonsterMass/Entity/MonsterRuntimeData.h"

void UMonsterMassTrait::BuildTemplate(FMassEntityTemplateBuildContext& BuildContext, const UWorld& World) const
{
    // 运行数据是构建实体模板的唯一配置来源
    if (!ensureMsgf(RuntimeData != nullptr, TEXT("[UBBBM]Monster trait requires MonsterRuntimeData")))
    {
        return;
    }

    // 将小怪身份标签和基础变换加入实体模板
    BuildContext.AddTag<FMonsterTag>();
    BuildContext.AddFragment<FTransformFragment>();
    BuildContext.AddFragment<FMassVelocityFragment>();

    // 将数据资产中的叶子配置复制为实体 Fragment
    BuildContext.AddFragment(FConstStructView::Make(RuntimeData->Health));
    BuildContext.AddFragment<FMonsterStateFragment>();
    BuildContext.AddFragment<FMonsterTargetRequestFragment>();
    BuildContext.AddFragment(FConstStructView::Make(RuntimeData->Movement));
    BuildContext.AddFragment(FConstStructView::Make(RuntimeData->Perception));
    BuildContext.AddFragment(FConstStructView::Make(RuntimeData->Combat));
    BuildContext.AddFragment(FConstStructView::Make(RuntimeData->Avoidance));

    // 添加事件和表现快照供后续处理器消费
    BuildContext.AddFragment<FMonsterDamageEventFragment>();
    BuildContext.AddFragment<FMonsterDeathEventFragment>();
    BuildContext.AddFragment<FMonsterPresentationStateFragment>();
}
