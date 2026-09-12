#include "BBBWork/UBBBNexus/MonsterMass/Processors/MonsterVisualizationProcessor.h"

#include "MassCommonTypes.h"
#include "BBBWork/UBBBNexus/MonsterMass/Entity/MonsterRuntimeData.h"

UMonsterVisualizationProcessor::UMonsterVisualizationProcessor()
{
    bAutoRegisterWithProcessingPhases = true;
    bRequiresGameThreadExecution = true;
    ExecutionFlags = static_cast<uint8>(EProcessorExecutionFlags::AllNetModes);
}

void UMonsterVisualizationProcessor::ConfigureQueries(const TSharedRef<FMassEntityManager>& EntityManager)
{
    // 保留引擎表现查询后，仅筛选小怪实体
    Super::ConfigureQueries(EntityManager);

    EntityQuery.AddTagRequirement<FMonsterTag>(EMassFragmentPresence::All);
}
