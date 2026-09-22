#include "BBBWork/UBBBNexus/MonsterMass/Processors/MonsterStateProcessor.h"

#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "MassCommonFragments.h"
#include "MassExecutionContext.h"
#include "BBBWork/UBBBNexus/MonsterMass/Entity/MonsterRuntimeData.h"
#include "BBBWork/UBBBNexus/MonsterMass/Processors/MonsterPerceptionProcessor.h"

UMonsterStateProcessor::UMonsterStateProcessor()
    : MonsterQuery(*this)
{
    bAutoRegisterWithProcessingPhases = true;
    bRequiresGameThreadExecution = true;
    ExecutionFlags = static_cast<uint8>(EProcessorExecutionFlags::AllNetModes);
    ExecutionOrder.ExecuteAfter.Add(UMonsterPerceptionProcessor::StaticClass()->GetFName());
}

void UMonsterStateProcessor::ConfigureQueries(const TSharedRef<FMassEntityManager>& EntityManager)
{
    MonsterQuery.AddRequirement<FTransformFragment>(EMassFragmentAccess::ReadOnly);
    MonsterQuery.AddRequirement<FMonsterHealthFragment>(EMassFragmentAccess::ReadOnly);
    MonsterQuery.AddRequirement<FMonsterMovementFragment>(EMassFragmentAccess::ReadOnly);
    MonsterQuery.AddRequirement<FMonsterTargetRequestFragment>(EMassFragmentAccess::ReadOnly);
    MonsterQuery.AddRequirement<FMonsterDamageEventFragment>(EMassFragmentAccess::ReadWrite);
    MonsterQuery.AddRequirement<FMonsterDeathEventFragment>(EMassFragmentAccess::ReadWrite);
    MonsterQuery.AddRequirement<FMonsterCombatFragment>(EMassFragmentAccess::ReadWrite);
    MonsterQuery.AddRequirement<FMonsterStateFragment>(EMassFragmentAccess::ReadWrite);
    MonsterQuery.AddTagRequirement<FMonsterTag>(EMassFragmentPresence::All);
}

void UMonsterStateProcessor::Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context)
{
    UWorld* World = Context.GetWorld();

    if (!ensureMsgf(World != nullptr, TEXT("[UBBBM]Monster state requires a valid world")))
    {
        return;
    }

    const float WorldTime = World->GetTimeSeconds();

    // 维护受伤硬直恢复和死亡实体延迟回收
    MonsterQuery.ForEachEntityChunk(Context, [WorldTime](FMassExecutionContext& ChunkContext)
    {
        const auto Transforms = ChunkContext.GetFragmentView<FTransformFragment>();
        const auto Healths = ChunkContext.GetFragmentView<FMonsterHealthFragment>();
        const auto Movements = ChunkContext.GetFragmentView<FMonsterMovementFragment>();
        const auto Targets = ChunkContext.GetFragmentView<FMonsterTargetRequestFragment>();
        auto DamageEvents = ChunkContext.GetMutableFragmentView<FMonsterDamageEventFragment>();
        auto DeathEvents = ChunkContext.GetMutableFragmentView<FMonsterDeathEventFragment>();
        auto Combats = ChunkContext.GetMutableFragmentView<FMonsterCombatFragment>();
        auto States = ChunkContext.GetMutableFragmentView<FMonsterStateFragment>();

        for (int32 Index = 0; Index < ChunkContext.GetNumEntities(); ++Index)
        {
            FMonsterStateFragment& State = States[Index];
            FMonsterCombatFragment& Combat = Combats[Index];
            FMonsterDamageEventFragment& DamageEvent = DamageEvents[Index];
            const FMonsterHealthFragment& Health = Healths[Index];

            const auto EnterState = [&State, WorldTime](const EMonsterState NewState, const bool bRestart = false)
            {
                if (State.State != NewState || bRestart)
                {
                    State.State = NewState;
                    State.StateEnteredTime = WorldTime;
                    ++State.ActionId;
                }
            };

            // 先扣除累计伤害，再根据剩余生命切换状态
            if (State.State == EMonsterState::Dead || Health.CurrentHealth <= 0.0f)
            {
                // 生命归零后进入死亡状态并延迟回收
                if (State.State != EMonsterState::Dead)
                {
                    EnterState(EMonsterState::Dead);
                    DeathEvents[Index].DestroyAtTime = WorldTime + Health.DeathLifetime;
                    UE_LOG(LogTemp, Log, TEXT("[UBBBM]Monster dead Entity=%d DestroyAt=%.3f"), ChunkContext.GetEntity(Index).Index, DeathEvents[Index].DestroyAtTime);
                }

                Combat.AttackTarget.Reset();
                Combat.bHitAttempted = true;
                Combat.bAttackFinished = true;
                DamageEvent.bReceivedDamage = false;
                continue;
            }

            if (DamageEvent.bReceivedDamage)
            {
                DamageEvent.bReceivedDamage = false;
                Combat.AttackTarget.Reset();
                Combat.bHitAttempted = true;
                Combat.bAttackFinished = true;
                EnterState(EMonsterState::Hurt, true);
                continue;
            }

            if (State.State == EMonsterState::Hurt && WorldTime < State.StateEnteredTime + Health.HurtDuration)
            {
                continue;
            }

            // 战斗处理器先完成唯一命中判定 下一帧才允许结束攻击 防止长帧漏判
            if (State.State == EMonsterState::Attack && !Combat.bAttackFinished)
            {
                continue;
            }

            // 受伤硬直结束后恢复追击状态
            const FMonsterTargetRequestFragment& Target = Targets[Index];

            if (!Target.bHasTarget || !Target.TargetActor.IsValid())
            {
                EnterState(EMonsterState::Scout);
                continue;
            }

            const float StartRange = FMath::Min(Movements[Index].StopRadius, Combat.AttackRange);
            const bool bInRange = FVector::DistSquared2D(Transforms[Index].GetTransform().GetLocation(), Target.TargetLocation) <= FMath::Square(StartRange + KINDA_SMALL_NUMBER);

            if (bInRange && WorldTime >= Combat.NextAttackTime)
            {
                EnterState(EMonsterState::Attack, true);
                ++Combat.AttackId;
                Combat.AttackTarget = Target.TargetActor;
                Combat.bHitAttempted = false;
                Combat.bAttackFinished = false;
                Combat.NextAttackTime = WorldTime + FMath::Max(Combat.AttackCooldown, Combat.AttackWindup + Combat.AttackRecovery);
                UE_LOG(LogTemp, Verbose, TEXT("[UBBBM]Attack begin Entity=%d AttackId=%u"), ChunkContext.GetEntity(Index).Index, Combat.AttackId);
                continue;
            }

            EnterState(bInRange ? EMonsterState::Idle : EMonsterState::Chase);
        }
    });
}
