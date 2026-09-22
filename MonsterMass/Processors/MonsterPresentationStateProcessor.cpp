#include "BBBWork/UBBBNexus/MonsterMass/Processors/MonsterPresentationStateProcessor.h"

#include "MassExecutionContext.h"
#include "Engine/World.h"
#include "BBBWork/UBBBNexus/MonsterMass/Processors/MonsterCombatProcessor.h"
#include "MassMovementFragments.h"
#include "BBBWork/UBBBNexus/MonsterMass/Entity/MonsterRuntimeData.h"

UMonsterPresentationStateProcessor::UMonsterPresentationStateProcessor()
    : MonsterQuery(*this)
{
    bAutoRegisterWithProcessingPhases = true;
    bRequiresGameThreadExecution = true;
    ExecutionOrder.ExecuteAfter.Add(UMonsterCombatProcessor::StaticClass()->GetFName());
    ExecutionFlags = static_cast<uint8>(EProcessorExecutionFlags::AllNetModes);
}

void UMonsterPresentationStateProcessor::ConfigureQueries(const TSharedRef<FMassEntityManager>& EntityManager)
{
    MonsterQuery.AddRequirement<FMonsterCombatFragment>(EMassFragmentAccess::ReadOnly);
    MonsterQuery.AddRequirement<FMonsterHealthFragment>(EMassFragmentAccess::ReadOnly);
    MonsterQuery.AddRequirement<FMassVelocityFragment>(EMassFragmentAccess::ReadOnly);
    MonsterQuery.AddRequirement<FMonsterStateFragment>(EMassFragmentAccess::ReadOnly);
    MonsterQuery.AddRequirement<FMonsterPresentationStateFragment>(EMassFragmentAccess::ReadWrite);
    MonsterQuery.AddTagRequirement<FMonsterTag>(EMassFragmentPresence::All);
}

void UMonsterPresentationStateProcessor::Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context)
{
    if (!ensureMsgf(Context.GetWorld() != nullptr, TEXT("[UBBBM]Presentation snapshot requires a valid world")))
    {
        return;
    }

    const float WorldTime = Context.GetWorld()->GetTimeSeconds();
    MonsterQuery.ForEachEntityChunk(Context, [WorldTime](FMassExecutionContext& ChunkContext)
    {
        // 把权威状态转换为表现层可安全读取的快照
        const TConstArrayView<FMassVelocityFragment> Velocities = ChunkContext.GetFragmentView<FMassVelocityFragment>();
        const TConstArrayView<FMonsterStateFragment> States = ChunkContext.GetFragmentView<FMonsterStateFragment>();
        TArrayView<FMonsterPresentationStateFragment> PresentationStates = ChunkContext.GetMutableFragmentView<FMonsterPresentationStateFragment>();

        const auto Combats = ChunkContext.GetFragmentView<FMonsterCombatFragment>();
        const auto Healths = ChunkContext.GetFragmentView<FMonsterHealthFragment>();

        // 逐个实体复制状态快照数据
        for (int32 Index = 0; Index < ChunkContext.GetNumEntities(); ++Index)
        {
            FMonsterPresentationStateFragment& PresentationState = PresentationStates[Index];
            PresentationState.State = States[Index].State;
            PresentationState.Speed = Velocities[Index].Value.Size2D();
            PresentationState.StateEnteredTime = States[Index].StateEnteredTime;
            PresentationState.ActionId = States[Index].ActionId;
            PresentationState.ActionProgress = 0.0f;
            const float Elapsed = FMath::Max(WorldTime - States[Index].StateEnteredTime, 0.0f);

            // 分段映射让动画命中帧与逻辑前摇结束严格对齐
            if (States[Index].State == EMonsterState::Attack)
            {
                const FMonsterCombatFragment& Combat = Combats[Index];
                PresentationState.ActionProgress = Elapsed <= Combat.AttackWindup
                    ? Combat.AnimationHitFraction * Elapsed / Combat.AttackWindup
                    : Combat.AnimationHitFraction + (1.0f - Combat.AnimationHitFraction) * (Elapsed - Combat.AttackWindup) / Combat.AttackRecovery;
            }

            if (States[Index].State == EMonsterState::Hurt)
            {
                PresentationState.ActionProgress = Elapsed / Healths[Index].HurtDuration;
            }

            if (States[Index].State == EMonsterState::Dead)
            {
                PresentationState.ActionProgress = Elapsed / Healths[Index].DeathLifetime;
            }

            PresentationState.ActionProgress = FMath::Clamp(PresentationState.ActionProgress, 0.0f, 1.0f);
        }
    });
}
