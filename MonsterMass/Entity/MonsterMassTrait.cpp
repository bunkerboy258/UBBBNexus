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

    // 配置错误直接阻止构建 避免产生永远够不到目标或无法结束动作的实体
    const FMonsterHealthFragment& Health = RuntimeData->Health;
    const FMonsterCombatFragment& Combat = RuntimeData->Combat;
    const FMonsterMovementFragment& Movement = RuntimeData->Movement;
    const bool bValidHealth = FMath::IsFinite(Health.MaxHealth) && Health.MaxHealth > 0.0f
        && FMath::IsFinite(Health.CurrentHealth) && Health.CurrentHealth > 0.0f && Health.CurrentHealth <= Health.MaxHealth
        && FMath::IsFinite(Health.HurtDuration) && Health.HurtDuration > 0.0f
        && FMath::IsFinite(Health.DeathLifetime) && Health.DeathLifetime > 0.0f;
    const bool bValidCombat = FMath::IsFinite(Combat.AttackWindup) && Combat.AttackWindup > 0.0f
        && FMath::IsFinite(Combat.AttackRecovery) && Combat.AttackRecovery > 0.0f
        && FMath::IsFinite(Combat.AttackCooldown) && Combat.AttackCooldown >= 0.0f
        && FMath::IsFinite(Combat.AttackDamage) && Combat.AttackDamage >= 0.0f
        && FMath::IsFinite(Combat.AttackRange) && Combat.AttackRange > 0.0f
        && FMath::IsFinite(Combat.AnimationHitFraction) && Combat.AnimationHitFraction > 0.0f && Combat.AnimationHitFraction < 1.0f;
    const bool bValidMovement = FMath::IsFinite(Movement.StopRadius) && Movement.StopRadius >= 0.0f
        && Movement.StopRadius <= Combat.AttackRange && FMath::IsFinite(Movement.MoveSpeed) && Movement.MoveSpeed >= 0.0f;

    if (!ensureMsgf(bValidHealth && bValidCombat && bValidMovement, TEXT("[UBBBM]Invalid monster configuration Asset=%s Health=%d Combat=%d Movement=%d"),
        *GetPathNameSafe(RuntimeData), bValidHealth, bValidCombat, bValidMovement))
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
