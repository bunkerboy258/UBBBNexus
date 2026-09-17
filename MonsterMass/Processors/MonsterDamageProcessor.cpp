#include "BBBWork/UBBBNexus/MonsterMass/Processors/MonsterDamageProcessor.h"

#include "MassExecutionContext.h"
#include "BBBWork/UBBBNexus/MonsterMass/Entity/MonsterRuntimeData.h"

UMonsterDamageProcessor::UMonsterDamageProcessor()
    : MonsterQuery(*this)
{
    bAutoRegisterWithProcessingPhases = true;
    bRequiresGameThreadExecution = true;
    ExecutionFlags = static_cast<uint8>(EProcessorExecutionFlags::AllNetModes);
}

void UMonsterDamageProcessor::ConfigureQueries(const TSharedRef<FMassEntityManager>& EntityManager)
{
    MonsterQuery.AddRequirement<FMonsterHealthFragment>(EMassFragmentAccess::ReadWrite);
    MonsterQuery.AddRequirement<FMonsterDamageEventFragment>(EMassFragmentAccess::ReadWrite);
    MonsterQuery.AddRequirement<FMonsterDeathEventFragment>(EMassFragmentAccess::ReadWrite);
    MonsterQuery.AddRequirement<FMonsterStateFragment>(EMassFragmentAccess::ReadWrite);
    MonsterQuery.AddTagRequirement<FMonsterTag>(EMassFragmentPresence::All);
}

void UMonsterDamageProcessor::Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context)
{
    // 获取伤害处理使用的世界时间
    UWorld* World = Context.GetWorld();

    if (!ensureMsgf(World != nullptr, TEXT("[UBBBM]Monster damage requires a valid world")))
    {
        return;
    }

    const float WorldTime = World->GetTimeSeconds();

    // 逐帧消费实体积累的待处理伤害
    MonsterQuery.ForEachEntityChunk(Context, [WorldTime](FMassExecutionContext& ChunkContext)
    {
        TArrayView<FMonsterHealthFragment> Healths = ChunkContext.GetMutableFragmentView<FMonsterHealthFragment>();
        TArrayView<FMonsterDamageEventFragment> DamageEvents = ChunkContext.GetMutableFragmentView<FMonsterDamageEventFragment>();
        TArrayView<FMonsterDeathEventFragment> DeathEvents = ChunkContext.GetMutableFragmentView<FMonsterDeathEventFragment>();
        TArrayView<FMonsterStateFragment> States = ChunkContext.GetMutableFragmentView<FMonsterStateFragment>();

        for (int32 Index = 0; Index < ChunkContext.GetNumEntities(); ++Index)
        {
            FMonsterDamageEventFragment& DamageEvent = DamageEvents[Index];

            if (DamageEvent.PendingDamage <= 0.0f)
            {
                continue;
            }

            // 先扣除累计伤害，再根据剩余生命切换状态
            FMonsterHealthFragment& Health = Healths[Index];
            FMonsterDeathEventFragment& DeathEvent = DeathEvents[Index];
            FMonsterStateFragment& State = States[Index];

            Health.CurrentHealth = FMath::Clamp(Health.CurrentHealth - DamageEvent.PendingDamage, 0.0f, Health.MaxHealth);
            DamageEvent.PendingDamage = 0.0f;
            State.StateEnteredTime = WorldTime;

            if (Health.CurrentHealth > 0.0f)
            {
                State.State = EMonsterState::Hurt;
                continue;
            }

            // 生命归零后进入死亡状态并延迟回收
            State.State = EMonsterState::Dead;
            DeathEvent.DestroyAtTime = WorldTime + 3.0f;
        }
    });
}
