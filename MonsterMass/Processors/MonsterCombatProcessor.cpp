#include "BBBWork/UBBBNexus/MonsterMass/Processors/MonsterCombatProcessor.h"

#include "GameFramework/Pawn.h"
#include "Kismet/GameplayStatics.h"
#include "MassCommonFragments.h"
#include "MassExecutionContext.h"
#include "BBBWork/UBBBNexus/MonsterMass/Processors/MonsterNavigationProcessor.h"
#include "BBBWork/UBBBNexus/MonsterMass/Entity/MonsterRuntimeData.h"

UMonsterCombatProcessor::UMonsterCombatProcessor()
    : MonsterQuery(*this)
{
    bAutoRegisterWithProcessingPhases = true;
    bRequiresGameThreadExecution = true;
    ExecutionFlags = static_cast<uint8>(EProcessorExecutionFlags::AllNetModes);
    ExecutionOrder.ExecuteAfter.Add(UMonsterNavigationProcessor::StaticClass()->GetFName());
}

void UMonsterCombatProcessor::ConfigureQueries(const TSharedRef<FMassEntityManager>& EntityManager)
{
    MonsterQuery.AddRequirement<FTransformFragment>(EMassFragmentAccess::ReadOnly);
    MonsterQuery.AddRequirement<FMonsterTargetRequestFragment>(EMassFragmentAccess::ReadOnly);
    MonsterQuery.AddRequirement<FMonsterCombatFragment>(EMassFragmentAccess::ReadWrite);
    MonsterQuery.AddRequirement<FMonsterStateFragment>(EMassFragmentAccess::ReadWrite);
    MonsterQuery.AddTagRequirement<FMonsterTag>(EMassFragmentPresence::All);
}

void UMonsterCombatProcessor::Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context)
{
    // 获取攻击判定使用的世界
    UWorld* World = Context.GetWorld();

    if (!ensureMsgf(World != nullptr, TEXT("[UBBBM]Monster combat requires a valid world")))
    {
        return;
    }

    APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(World, 0);

    // 没有玩家时不执行任何攻击判定
    if (!IsValid(PlayerPawn))
    {
        return;
    }

    // 使用同一时间值处理当前批次
    const float WorldTime = World->GetTimeSeconds();

    MonsterQuery.ForEachEntityChunk(Context, [PlayerPawn, WorldTime, this](FMassExecutionContext& ChunkContext)
    {
        // 只处理已经进入攻击状态且冷却完成的实体
        const TConstArrayView<FTransformFragment> Transforms = ChunkContext.GetFragmentView<FTransformFragment>();
        const TConstArrayView<FMonsterTargetRequestFragment> Targets = ChunkContext.GetFragmentView<FMonsterTargetRequestFragment>();
        TArrayView<FMonsterCombatFragment> Combats = ChunkContext.GetMutableFragmentView<FMonsterCombatFragment>();
        TArrayView<FMonsterStateFragment> States = ChunkContext.GetMutableFragmentView<FMonsterStateFragment>();

        for (int32 Index = 0; Index < ChunkContext.GetNumEntities(); ++Index)
        {
            const FMonsterTargetRequestFragment& Target = Targets[Index];
            FMonsterCombatFragment& Combat = Combats[Index];
            FMonsterStateFragment& State = States[Index];

            if (!Target.bHasTarget || State.State != EMonsterState::Attack || WorldTime < Combat.NextAttackTime)
            {
                continue;
            }

            const FVector MonsterLocation = Transforms[Index].GetTransform().GetLocation();
            const float AttackRange = FMath::Max(Combat.AttackRange, 0.0f);

            if (FVector::DistSquared2D(MonsterLocation, PlayerPawn->GetActorLocation()) > FMath::Square(AttackRange))
            {
                State.State = EMonsterState::Chase;
                continue;
            }

            // 伤害通过 UE 标准接口发送给玩家
            UGameplayStatics::ApplyDamage(PlayerPawn, FMath::Max(Combat.AttackDamage, 0.0f), nullptr, nullptr, nullptr);
            Combat.NextAttackTime = WorldTime + FMath::Max(Combat.AttackCooldown, 0.0f);
            State.StateEnteredTime = WorldTime;

            UE_LOG(
                LogTemp,
                Log,
                TEXT("[UBBBM]Monster attack Target=%s Damage=%.1f"),
                *GetNameSafe(PlayerPawn),
                Combat.AttackDamage);
        }
    });
}
