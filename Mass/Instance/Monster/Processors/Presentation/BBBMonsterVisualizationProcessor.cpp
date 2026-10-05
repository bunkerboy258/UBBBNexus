#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Processors/Presentation/BBBMonsterVisualizationProcessor.h"
#include "BBBWork/UBBBNexus/Mass/Core/BBBMassProcessingGroups.h"

#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Tags/BBBMonsterTag.h"
#include "MassCommonTypes.h"

UBBBMonsterVisualizationProcessor::UBBBMonsterVisualizationProcessor()
{
    bAutoRegisterWithProcessingPhases = true;
    ProcessingPhase = EMassProcessingPhase::PrePhysics;
    bRequiresGameThreadExecution = true;
    ExecutionFlags = static_cast<uint8>(EProcessorExecutionFlags::AllNetModes);
}

void UBBBMonsterVisualizationProcessor::ConfigureQueries(const TSharedRef<FMassEntityManager>& EntityManager)
{
    // 保留引擎表现查询后 仅筛选小怪实体
    Super::ConfigureQueries(EntityManager);

    // 让可视化查询只处理小怪实体
    EntityQuery.AddTagRequirement<FBBBMonsterTag>(EMassFragmentPresence::All);
}
