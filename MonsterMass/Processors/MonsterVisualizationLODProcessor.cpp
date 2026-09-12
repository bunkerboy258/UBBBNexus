#include "BBBWork/UBBBNexus/MonsterMass/Processors/MonsterVisualizationLODProcessor.h"

#include "MassCommonTypes.h"
#include "BBBWork/UBBBNexus/MonsterMass/Entity/MonsterRuntimeData.h"

UMonsterVisualizationLODProcessor::UMonsterVisualizationLODProcessor()
{
    bAutoRegisterWithProcessingPhases = true;
    ExecutionFlags = static_cast<uint8>(EProcessorExecutionFlags::AllNetModes);
}

void UMonsterVisualizationLODProcessor::ConfigureQueries(const TSharedRef<FMassEntityManager>& EntityManager)
{
    // 为近处、远处和调试查询统一添加小怪标签
    Super::ConfigureQueries(EntityManager);

    CloseEntityQuery.AddTagRequirement<FMonsterTag>(EMassFragmentPresence::All);
    CloseEntityAdjustDistanceQuery.AddTagRequirement<FMonsterTag>(EMassFragmentPresence::All);
    FarEntityQuery.AddTagRequirement<FMonsterTag>(EMassFragmentPresence::All);
    DebugEntityQuery.AddTagRequirement<FMonsterTag>(EMassFragmentPresence::All);
    FilterTag = FMonsterTag::StaticStruct();
}
