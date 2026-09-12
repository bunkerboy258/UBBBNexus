#include "BBBWork/UBBBNexus/MonsterMass/Processors/MonsterPresentationStateProcessor.h"

#include "MassExecutionContext.h"
#include "MassMovementFragments.h"
#include "BBBWork/UBBBNexus/MonsterMass/Entity/MonsterRuntimeData.h"

UMonsterPresentationStateProcessor::UMonsterPresentationStateProcessor()
    : MonsterQuery(*this)
{
    bAutoRegisterWithProcessingPhases = true;
    bRequiresGameThreadExecution = true;
    ExecutionFlags = static_cast<uint8>(EProcessorExecutionFlags::AllNetModes);
}

void UMonsterPresentationStateProcessor::ConfigureQueries(const TSharedRef<FMassEntityManager>& EntityManager)
{
    MonsterQuery.AddRequirement<FMassVelocityFragment>(EMassFragmentAccess::ReadOnly);
    MonsterQuery.AddRequirement<FMonsterStateFragment>(EMassFragmentAccess::ReadOnly);
    MonsterQuery.AddRequirement<FMonsterPresentationStateFragment>(EMassFragmentAccess::ReadWrite);
    MonsterQuery.AddTagRequirement<FMonsterTag>(EMassFragmentPresence::All);
}

void UMonsterPresentationStateProcessor::Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context)
{
    MonsterQuery.ForEachEntityChunk(Context, [](FMassExecutionContext& ChunkContext)
    {
        // 把权威状态转换为表现层可安全读取的快照
        const TConstArrayView<FMassVelocityFragment> Velocities = ChunkContext.GetFragmentView<FMassVelocityFragment>();
        const TConstArrayView<FMonsterStateFragment> States = ChunkContext.GetFragmentView<FMonsterStateFragment>();
        TArrayView<FMonsterPresentationStateFragment> PresentationStates = ChunkContext.GetMutableFragmentView<FMonsterPresentationStateFragment>();

        for (int32 Index = 0; Index < ChunkContext.GetNumEntities(); ++Index)
        {
            FMonsterPresentationStateFragment& PresentationState = PresentationStates[Index];
            PresentationState.State = States[Index].State;
            PresentationState.Speed = Velocities[Index].Value.Size2D();
            PresentationState.StateEnteredTime = States[Index].StateEnteredTime;
        }
    });
}
