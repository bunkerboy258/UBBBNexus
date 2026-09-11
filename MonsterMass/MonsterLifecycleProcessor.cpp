#include "BBBWork/UBBBNexus/MonsterMass/MonsterLifecycleProcessor.h"

#include "MassExecutionContext.h"
#include "BBBWork/UBBBNexus/MonsterMass/MonsterRuntimeData.h"

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
    UWorld* World = Context.GetWorld();

    if (!ensureMsgf(World != nullptr, TEXT("[UBBBM]Monster lifecycle requires a valid world")))
    {
        return;
    }

    const float WorldTime = World->GetTimeSeconds();

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
                State.State = EMonsterState::Chase;
                State.StateEnteredTime = WorldTime;
            }

            if (State.State == EMonsterState::Dead && DeathEvent.DestroyAtTime >= 0.0f && WorldTime >= DeathEvent.DestroyAtTime)
            {
                ChunkContext.Defer().DestroyEntity(ChunkContext.GetEntity(Index));
            }
        }
    });
}
