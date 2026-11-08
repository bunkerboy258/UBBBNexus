#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Processors/Presentation/BBBMonsterVisualizationLODProcessor.h"
#include "BBBWork/UBBBNexus/Mass/Core/BBBMassProcessingGroups.h"

#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Tags/BBBMonsterTag.h"
#include "MassCommonTypes.h"

UBBBMonsterVisualizationLODProcessor::UBBBMonsterVisualizationLODProcessor()
{
    bAutoRegisterWithProcessingPhases = true;
    ProcessingPhase = EMassProcessingPhase::PrePhysics;
    ExecutionFlags = static_cast<uint8>(EProcessorExecutionFlags::AllNetModes);
}

void UBBBMonsterVisualizationLODProcessor::ConfigureQueries(const TSharedRef<FMassEntityManager>& EntityManager)
{
    // 为近处 远处和调试查询统一添加小怪标签
    Super::ConfigureQueries(EntityManager);

    CloseEntityQuery.AddTagRequirement<FBBBMonsterTag>(EMassFragmentPresence::All);
    CloseEntityAdjustDistanceQuery.AddTagRequirement<FBBBMonsterTag>(EMassFragmentPresence::All);
    FarEntityQuery.AddTagRequirement<FBBBMonsterTag>(EMassFragmentPresence::All);
    DebugEntityQuery.AddTagRequirement<FBBBMonsterTag>(EMassFragmentPresence::All);
    // 记录筛选标签供可视化查询使用
    FilterTag = FBBBMonsterTag::StaticStruct();
}
