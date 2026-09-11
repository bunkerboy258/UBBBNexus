#include "BBBWork/UBBBNexus/MonsterMass/MonsterVisualizationLODProcessor.h"

#include "MassCommonTypes.h"
#include "BBBWork/UBBBNexus/MonsterMass/MonsterRuntimeData.h"

UMonsterVisualizationLODProcessor::UMonsterVisualizationLODProcessor()
{
    bAutoRegisterWithProcessingPhases = true;
    ExecutionFlags = static_cast<uint8>(EProcessorExecutionFlags::AllNetModes);
}

void UMonsterVisualizationLODProcessor::ConfigureQueries(const TSharedRef<FMassEntityManager>& EntityManager)
{
    Super::ConfigureQueries(EntityManager);

    CloseEntityQuery.AddTagRequirement<FMonsterTag>(EMassFragmentPresence::All);
    CloseEntityAdjustDistanceQuery.AddTagRequirement<FMonsterTag>(EMassFragmentPresence::All);
    FarEntityQuery.AddTagRequirement<FMonsterTag>(EMassFragmentPresence::All);
    DebugEntityQuery.AddTagRequirement<FMonsterTag>(EMassFragmentPresence::All);
    FilterTag = FMonsterTag::StaticStruct();
}
