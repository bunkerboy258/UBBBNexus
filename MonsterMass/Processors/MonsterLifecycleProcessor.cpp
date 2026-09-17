#include "BBBWork/UBBBNexus/MonsterMass/Processors/MonsterLifecycleProcessor.h"

#include "MassExecutionContext.h"
#include "BBBWork/UBBBNexus/MonsterMass/Entity/MonsterRuntimeData.h"

UMonsterLifecycleProcessor::UMonsterLifecycleProcessor()
    : MonsterQuery(*this)
{
    bAutoRegisterWithProcessingPhases = true;
    bRequiresGameThreadExecution = true;
    ExecutionFlags = static_cast<uint8>(EProcessorExecutionFlags::AllNetModes);
}

void UMonsterLifecycleProcessor::ConfigureQueries(const TSharedRef<FMassEntityManager>& EntityManager)
{
    MonsterQuery.AddRequirement<FMonsterDeathEventFragment>(EMassFragmentAccess::ReadWrite);
    MonsterQuery.AddRequirement<FMonsterStateFragment>(EMassFragmentAccess::ReadWrite);
    MonsterQuery.AddTagRequirement<FMonsterTag>(EMassFragmentPresence::All);
}

void UMonsterLifecycleProcessor::Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context)
{
    // 获取生命周期处理使用的世界时间
    UWorld* World = Context.GetWorld();

    if (!ensureMsgf(World != nullptr, TEXT("[UBBBM]Monster lifecycle requires a valid world")))
    {
        return;
    }

    const float WorldTime = World->GetTimeSeconds();

    // 维护受伤硬直恢复和死亡实体延迟回收
    MonsterQuery.ForEachEntityChunk(Context, [WorldTime](FMassExecutionContext& ChunkContext)
    {
        TArrayView<FMonsterDeathEventFragment> DeathEvents = ChunkContext.GetMutableFragmentView<FMonsterDeathEventFragment>();
        TArrayView<FMonsterStateFragment> States = ChunkContext.GetMutableFragmentView<FMonsterStateFragment>();

        for (int32 Index = 0; Index < ChunkContext.GetNumEntities(); ++Index)
        {
            FMonsterDeathEventFragment& DeathEvent = DeathEvents[Index];
            FMonsterStateFragment& State = States[Index];

            if (State.State == EMonsterState::Hurt && WorldTime >= State.StateEnteredTime + 0.2f)
            {
                // 受伤硬直结束后恢复追击状态
                State.State = EMonsterState::Chase;
                State.StateEnteredTime = WorldTime;
            }

            if (State.State == EMonsterState::Dead && DeathEvent.DestroyAtTime >= 0.0f && WorldTime >= DeathEvent.DestroyAtTime)
            {
                // 使用延迟命令安全销毁当前遍历中的实体
                ChunkContext.Defer().DestroyEntity(ChunkContext.GetEntity(Index));
            }
        }
    });
}
