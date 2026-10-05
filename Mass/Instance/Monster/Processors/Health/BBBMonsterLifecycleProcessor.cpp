#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Processors/Health/BBBMonsterLifecycleProcessor.h"
#include "BBBWork/UBBBNexus/Mass/Core/BBBMassProcessingGroups.h"

#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Behavior/BBBMonsterBehavior.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Tags/BBBMonsterTag.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Behavior/BBBMonsterBehaviorFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Health/BBBMonsterDeathFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Network/BBBMonsterNetworkFragment.h"
#include "MassExecutionContext.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Processors/Presentation/BBBMonsterPresentationProcessor.h"

UBBBMonsterLifecycleProcessor::UBBBMonsterLifecycleProcessor()
    : MonsterQuery(*this)
{
    bAutoRegisterWithProcessingPhases = true;
    ProcessingPhase = EMassProcessingPhase::PrePhysics;
    ExecutionOrder.ExecuteInGroup = BBBMassProcessingGroups::Lifetime;
    bRequiresGameThreadExecution = true;
    ExecutionOrder.ExecuteAfter.Add(UBBBMonsterPresentationProcessor::StaticClass()->GetFName());
    ExecutionOrder.ExecuteAfter.Add(BBBMassProcessingGroups::Network);
    ExecutionFlags = static_cast<uint8>(EProcessorExecutionFlags::AllNetModes);
}

void UBBBMonsterLifecycleProcessor::ConfigureQueries(const TSharedRef<FMassEntityManager>& EntityManager)
{
    MonsterQuery.AddRequirement<FBBBMonsterDeathFragment>(EMassFragmentAccess::ReadOnly);
    MonsterQuery.AddRequirement<FBBBMonsterBehaviorFragment>(EMassFragmentAccess::ReadOnly);
    MonsterQuery.AddRequirement<FBBBMonsterNetworkFragment>(EMassFragmentAccess::ReadOnly);
    MonsterQuery.AddTagRequirement<FBBBMonsterTag>(EMassFragmentPresence::All);
}

void UBBBMonsterLifecycleProcessor::Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context)
{
    // 获取生命周期处理使用的世界时间
    UWorld* World = Context.GetWorld();

    if (!ensureMsgf(World != nullptr, TEXT("[UBBBM]Monster lifecycle requires a valid world")))
    {
        return;
    }

    const float WorldTime = World->GetTimeSeconds();
    const bool bStandalone = World->GetNetMode() == NM_Standalone;

    // 维护受伤硬直恢复和死亡实体延迟回收
    MonsterQuery.ForEachEntityChunk(Context, [WorldTime, bStandalone](FMassExecutionContext& ChunkContext)
    {
        TConstArrayView<FBBBMonsterDeathFragment> DeathEvents = ChunkContext.GetFragmentView<FBBBMonsterDeathFragment>();
        TConstArrayView<FBBBMonsterBehaviorFragment> States = ChunkContext.GetFragmentView<FBBBMonsterBehaviorFragment>();
        const auto Network = ChunkContext.GetFragmentView<FBBBMonsterNetworkFragment>();

        for (int32 Index = 0; Index < ChunkContext.GetNumEntities(); ++Index)
        {
            const FBBBMonsterDeathFragment& DeathEvent = DeathEvents[Index];
            const FBBBMonsterBehaviorFragment& State = States[Index];

            if (State.State == EBBBMonsterBehavior::Dead && DeathEvent.DestroyAtTime >= 0.0f
                && WorldTime >= DeathEvent.DestroyAtTime && (bStandalone || Network[Index].bDamageSubmitted))
            {
                // 使用延迟命令安全销毁当前遍历中的实体
                ChunkContext.Defer().DestroyEntity(ChunkContext.GetEntity(Index));
            }
        }
    });
}
