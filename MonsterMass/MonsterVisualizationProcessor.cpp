#include "BBBWork/UBBBNexus/MonsterMass/MonsterVisualizationProcessor.h"

#include "MassCommonTypes.h"
#include "BBBWork/UBBBNexus/MonsterMass/MonsterRuntimeData.h"

UMonsterVisualizationProcessor::UMonsterVisualizationProcessor()
{
    bAutoRegisterWithProcessingPhases = true;
    bRequiresGameThreadExecution = true;
    ExecutionFlags = static_cast<uint8>(EProcessorExecutionFlags::AllNetModes);
}

void UMonsterVisualizationProcessor::ConfigureQueries(const TSharedRef<FMassEntityManager>& EntityManager)
{
    Super::ConfigureQueries(EntityManager);

    EntityQuery.AddTagRequirement<FMonsterTag>(EMassFragmentPresence::All);
}
