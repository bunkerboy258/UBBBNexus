#include "BBBWork/UBBBNexus/MonsterMass/Processors/MonsterCombatProcessor.h"

#include "GameFramework/Pawn.h"
#include "MassActorSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "MassCommonFragments.h"
#include "MassExecutionContext.h"
#include "BBBWork/UBBBNexus/MonsterMass/Processors/MonsterAvoidanceProcessor.h"
#include "BBBWork/UBBBNexus/MonsterMass/Entity/MonsterRuntimeData.h"

UMonsterCombatProcessor::UMonsterCombatProcessor()
    : MonsterQuery(*this)
{
    bAutoRegisterWithProcessingPhases = true;
    bRequiresGameThreadExecution = true;
    ExecutionFlags = static_cast<uint8>(EProcessorExecutionFlags::AllNetModes);
    ExecutionOrder.ExecuteAfter.Add(UMonsterAvoidanceProcessor::StaticClass()->GetFName());
}

void UMonsterCombatProcessor::ConfigureQueries(const TSharedRef<FMassEntityManager>& EntityManager)
{
    MonsterQuery.AddRequirement<FTransformFragment>(EMassFragmentAccess::ReadOnly);
    MonsterQuery.AddRequirement<FMassActorFragment>(EMassFragmentAccess::ReadOnly, EMassFragmentPresence::Optional);
    MonsterQuery.AddRequirement<FMonsterCombatFragment>(EMassFragmentAccess::ReadWrite);
    MonsterQuery.AddRequirement<FMonsterStateFragment>(EMassFragmentAccess::ReadOnly);
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

    // 使用同一时间值处理当前批次
    const float WorldTime = World->GetTimeSeconds();

    MonsterQuery.ForEachEntityChunk(Context, [WorldTime](FMassExecutionContext& ChunkContext)
    {
        // 只处理已经进入攻击状态且冷却完成的实体
        const auto Transforms = ChunkContext.GetFragmentView<FTransformFragment>();
        const auto Actors = ChunkContext.GetFragmentView<FMassActorFragment>();
        const auto States = ChunkContext.GetFragmentView<FMonsterStateFragment>();
        auto Combats = ChunkContext.GetMutableFragmentView<FMonsterCombatFragment>();

        for (int32 Index = 0; Index < ChunkContext.GetNumEntities(); ++Index)
        {
            FMonsterCombatFragment& Combat = Combats[Index];

            if (States[Index].State != EMonsterState::Attack)
            {
                continue;
            }

            const float Elapsed = FMath::Max(WorldTime - States[Index].StateEnteredTime, 0.0f);
            Combat.bAttackFinished = Elapsed >= Combat.AttackWindup + Combat.AttackRecovery;

            if (Combat.bHitAttempted || Elapsed < Combat.AttackWindup)
            {
                continue;
            }

            // 先标记已判定再调用外部伤害回调 未命中也消耗本轮机会
            Combat.bHitAttempted = true;
            AActor* PlayerPawn = Combat.AttackTarget.Get();

            // 没有玩家时不执行任何攻击判定
            if (!IsValid(PlayerPawn))
            {
                UE_LOG(LogTemp, Verbose, TEXT("[UBBBM]Attack miss AttackId=%u Reason=InvalidTarget"), Combat.AttackId);
                continue;
            }

            const FVector MonsterLocation = Transforms[Index].GetTransform().GetLocation();

            if (FVector::DistSquared(MonsterLocation, PlayerPawn->GetActorLocation()) > FMath::Square(Combat.AttackRange))
            {
                UE_LOG(LogTemp, Verbose, TEXT("[UBBBM]Attack miss AttackId=%u Reason=OutOfRange"), Combat.AttackId);
                continue;
            }

            AActor* DamageCauser = Actors.IsEmpty() ? nullptr : const_cast<AActor*>(Actors[Index].Get());
            APawn* CauserPawn = Cast<APawn>(DamageCauser);
            AController* Instigator = CauserPawn != nullptr ? CauserPawn->GetController() : nullptr;

            // 伤害通过 UE 标准接口发送给玩家
            const float AppliedDamage = UGameplayStatics::ApplyDamage(PlayerPawn, Combat.AttackDamage, Instigator, DamageCauser, nullptr);
            UE_LOG(LogTemp, Verbose, TEXT("[UBBBM]Monster attack Target=%s AttackId=%u Requested=%.1f Applied=%.1f"),
                *GetNameSafe(PlayerPawn), Combat.AttackId, Combat.AttackDamage, AppliedDamage);
        }
    });
}
