#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Processors/Behavior/BBBMonsterBehaviorProcessor.h"
#include "BBBWork/UBBBNexus/Mass/Core/BBBMassProcessingGroups.h"

#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Behavior/BBBMonsterBehavior.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Tags/BBBMonsterTag.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Health/BBBMonsterHealthFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Behavior/BBBMonsterBehaviorFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Movement/BBBMonsterNavigationFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Combat/BBBMonsterCombatFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Perception/BBBMonsterTargetFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Health/BBBMonsterDeathFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Health/BBBMonsterDamageFragment.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "MassCommonFragments.h"
#include "MassExecutionContext.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Movement/BBBMonsterGroundFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Movement/BBBMonsterMobilityFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Network/BBBMonsterNetworkFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Config/BBBMonsterDefinition.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Processors/Perception/BBBMonsterPerceptionProcessor.h"

UBBBMonsterBehaviorProcessor::UBBBMonsterBehaviorProcessor()
    : MonsterQuery(*this)
{
    bAutoRegisterWithProcessingPhases = true;
    ProcessingPhase = EMassProcessingPhase::PrePhysics;
    ExecutionOrder.ExecuteInGroup = BBBMassProcessingGroups::Decision;
    ExecutionOrder.ExecuteAfter.Add(TEXT("BBBMonsterDamageProcessor"));
    bRequiresGameThreadExecution = true;
    ExecutionFlags = static_cast<uint8>(EProcessorExecutionFlags::AllNetModes);
    ExecutionOrder.ExecuteAfter.Add(UBBBMonsterPerceptionProcessor::StaticClass()->GetFName());
}

void UBBBMonsterBehaviorProcessor::ConfigureQueries(const TSharedRef<FMassEntityManager>& EntityManager)
{
    MonsterQuery.AddRequirement<FTransformFragment>(EMassFragmentAccess::ReadWrite);
    MonsterQuery.AddRequirement<FBBBMonsterHealthFragment>(EMassFragmentAccess::ReadOnly);
    MonsterQuery.AddRequirement<FBBBMonsterNavigationFragment>(EMassFragmentAccess::ReadOnly);
    MonsterQuery.AddRequirement<FBBBMonsterGroundFragment>(EMassFragmentAccess::ReadOnly);
    MonsterQuery.AddRequirement<FBBBMonsterMobilityFragment>(EMassFragmentAccess::ReadOnly);
    MonsterQuery.AddRequirement<FBBBMonsterNetworkFragment>(EMassFragmentAccess::ReadOnly);
    MonsterQuery.AddRequirement<FBBBMonsterTargetFragment>(EMassFragmentAccess::ReadOnly);
    MonsterQuery.AddRequirement<FBBBMonsterDamageFragment>(EMassFragmentAccess::ReadWrite);
    MonsterQuery.AddRequirement<FBBBMonsterDeathFragment>(EMassFragmentAccess::ReadWrite);
    MonsterQuery.AddRequirement<FBBBMonsterCombatFragment>(EMassFragmentAccess::ReadWrite);
    MonsterQuery.AddRequirement<FBBBMonsterBehaviorFragment>(EMassFragmentAccess::ReadWrite);
    MonsterQuery.AddTagRequirement<FBBBMonsterTag>(EMassFragmentPresence::All);
}

void UBBBMonsterBehaviorProcessor::Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context)
{
    UWorld* World = Context.GetWorld();

    if (!ensureMsgf(World != nullptr, TEXT("[UBBBM]Monster state requires a valid world")))
    {
        return;
    }

    const float WorldTime = World->GetTimeSeconds();
    const bool bRemote = World->GetNetMode() == NM_Client;

    // 维护受伤硬直恢复和死亡实体延迟回收
    MonsterQuery.ForEachEntityChunk(Context, [WorldTime, bRemote](FMassExecutionContext& ChunkContext)
    {
        auto Transforms = ChunkContext.GetMutableFragmentView<FTransformFragment>();
        const auto Healths = ChunkContext.GetFragmentView<FBBBMonsterHealthFragment>();
        const auto Navigation = ChunkContext.GetFragmentView<FBBBMonsterNavigationFragment>();
        const auto Grounds = ChunkContext.GetFragmentView<FBBBMonsterGroundFragment>();
        const auto Mobility = ChunkContext.GetFragmentView<FBBBMonsterMobilityFragment>();
        const auto Network = ChunkContext.GetFragmentView<FBBBMonsterNetworkFragment>();
        const auto Targets = ChunkContext.GetFragmentView<FBBBMonsterTargetFragment>();
        auto DamageEvents = ChunkContext.GetMutableFragmentView<FBBBMonsterDamageFragment>();
        auto DeathEvents = ChunkContext.GetMutableFragmentView<FBBBMonsterDeathFragment>();
        auto Combats = ChunkContext.GetMutableFragmentView<FBBBMonsterCombatFragment>();
        auto States = ChunkContext.GetMutableFragmentView<FBBBMonsterBehaviorFragment>();

        for (int32 Index = 0; Index < ChunkContext.GetNumEntities(); ++Index)
        {
            FBBBMonsterBehaviorFragment& State = States[Index];
            FBBBMonsterCombatFragment& Combat = Combats[Index];
            FBBBMonsterDamageFragment& DamageEvent = DamageEvents[Index];
            const FBBBMonsterHealthFragment& Health = Healths[Index];

            const auto EnterState = [&State, WorldTime](const EBBBMonsterBehavior NewState, const bool bRestart = false)
            {
                if (State.State != NewState || bRestart)
                {
                    State.State = NewState;
                    State.StateEnteredTime = WorldTime;
                    ++State.ActionId;
                    State.StateEndsAtTime = 0.0f;
                    if (NewState == EBBBMonsterBehavior::Idle)
                    {
                        State.StateEndsAtTime = WorldTime + State.Random.FRandRange(State.IdleDurationMin, State.IdleDurationMax);
                    }
                    if (NewState == EBBBMonsterBehavior::Patrol)
                    {
                        State.StateEndsAtTime = WorldTime + State.Random.FRandRange(State.PatrolDurationMin, State.PatrolDurationMax);
                    }
                    if (NewState == EBBBMonsterBehavior::Alert)
                    {
                        State.StateEndsAtTime = WorldTime + State.AlertDuration;
                    }
                }
            };

            if (!bRemote && State.StateEndsAtTime < 0.0f)
            {
                const FMassEntityHandle Entity = ChunkContext.GetEntity(Index);
                State.Random.Initialize(static_cast<int32>(HashCombine(GetTypeHash(Entity.Index), GetTypeHash(Entity.SerialNumber))));
                EnterState(EBBBMonsterBehavior::Idle, true);
            }

            // 先扣除累计伤害 再根据剩余生命切换状态
            if (State.State == EBBBMonsterBehavior::Dead || Health.CurrentHealth <= 0.0f)
            {
                // 生命归零后进入死亡状态并延迟回收
                if (DeathEvents[Index].DestroyAtTime <= 0.0f)
                {
                    EnterState(EBBBMonsterBehavior::Dead);
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
            }

            if (bRemote)
            {
                continue;
            }

            const auto* Settings = Network[Index].Definition.Get();
            if (Mobility[Index].bCrawling && Settings && WorldTime < Mobility[Index].CrawlStartedAt + Settings->CrawlTransitionDuration)
            {
                Combat.AttackTarget.Reset();
                Combat.bHitAttempted = true;
                Combat.bAttackFinished = true;
                EnterState(EBBBMonsterBehavior::Chase);
                continue;
            }

            if (State.State == EBBBMonsterBehavior::Attack && !Grounds[Index].bGrounded)
            {
                Combat.AttackTarget.Reset();
                Combat.bHitAttempted = true;
                Combat.bAttackFinished = true;
            }

            if (State.State == EBBBMonsterBehavior::Attack &&
                (!Combat.AttackTarget.IsValid() || !Combat.AttackTarget->CanBeDamaged()))
            {
                Combat.AttackTarget.Reset();
                Combat.bHitAttempted = true;
                Combat.bAttackFinished = true;
            }

            // 战斗处理器先完成唯一命中判定 下一帧才允许结束攻击 防止长帧漏判
            if (State.State == EBBBMonsterBehavior::Attack && !Combat.bAttackFinished)
            {
                continue;
            }

            const FBBBMonsterTargetFragment& Target = Targets[Index];

            if (!Target.bHasTarget || !Target.TargetActor.IsValid() || !Target.TargetActor->CanBeDamaged())
            {
                if (State.bHadTarget)
                {
                    State.bHadTarget = false;
                    EnterState(EBBBMonsterBehavior::Alert, true);
                    continue;
                }
                if (State.State == EBBBMonsterBehavior::Alert && WorldTime < State.StateEndsAtTime)
                {
                    continue;
                }
                if (State.State == EBBBMonsterBehavior::Patrol)
                {
                    if (WorldTime >= State.StateEndsAtTime ||
                        (Navigation[Index].ActionId == State.ActionId && Navigation[Index].bReachedDestination))
                    {
                        EnterState(EBBBMonsterBehavior::Idle);
                    }
                    continue;
                }
                if (State.State == EBBBMonsterBehavior::Idle)
                {
                    if (WorldTime >= State.StateEndsAtTime)
                    {
                        EnterState(EBBBMonsterBehavior::Patrol);
                    }
                    continue;
                }
                EnterState(EBBBMonsterBehavior::Idle);
                continue;
            }

            if (!State.bHadTarget)
            {
                State.bHadTarget = true;
                EnterState(EBBBMonsterBehavior::Alert, true);
                continue;
            }
            if (State.State == EBBBMonsterBehavior::Alert && WorldTime < State.StateEndsAtTime)
            {
                continue;
            }

            const bool bInRange = FVector::DistSquared(Transforms[Index].GetTransform().GetLocation(), Target.TargetLocation) <= FMath::Square(Combat.AttackRange);

            if (Grounds[Index].bGrounded && bInRange && WorldTime >= Combat.NextAttackTime)
            {
                FTransform& Transform = Transforms[Index].GetMutableTransform();
                const FVector Facing = (Target.TargetLocation - Transform.GetLocation()).GetSafeNormal2D();
                if (!Facing.IsNearlyZero())
                {
                    Transform.SetRotation(Facing.Rotation().Quaternion());
                }
                EnterState(EBBBMonsterBehavior::Attack, true);
                ++Combat.AttackId;
                Combat.AttackTarget = Target.TargetActor;
                Combat.bHitAttempted = false;
                Combat.bAttackFinished = false;
                Combat.NextAttackTime = WorldTime + FMath::Max(Combat.AttackCooldown, Combat.AttackWindup + Combat.AttackRecovery);
                UE_LOG(LogTemp, Verbose, TEXT("[UBBBM]Attack begin Entity=%d AttackId=%u"), ChunkContext.GetEntity(Index).Index, Combat.AttackId);
                continue;
            }

            EnterState(EBBBMonsterBehavior::Chase);
        }
    });
}
