#include "BBBWork/UBBBNexus/MonsterMass/Processors/MonsterDamageProcessor.h"

#include "MassExecutionContext.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Controller.h"
#include "BBBWork/UBBBNexus/MonsterMass/Entity/MonsterRuntimeData.h"

UMonsterDamageProcessor::UMonsterDamageProcessor()
    : MonsterQuery(*this)
{
    bAutoRegisterWithProcessingPhases = true;
    bRequiresGameThreadExecution = true;
    ExecutionFlags = static_cast<uint8>(EProcessorExecutionFlags::AllNetModes);
}

void UMonsterDamageProcessor::ConfigureQueries(const TSharedRef<FMassEntityManager>& EntityManager)
{
    MonsterQuery.AddRequirement<FMonsterHealthFragment>(EMassFragmentAccess::ReadWrite);
    MonsterQuery.AddRequirement<FMonsterDamageEventFragment>(EMassFragmentAccess::ReadWrite);
    MonsterQuery.AddRequirement<FMonsterStateFragment>(EMassFragmentAccess::ReadOnly);
    MonsterQuery.AddTagRequirement<FMonsterTag>(EMassFragmentPresence::All);
}

void UMonsterDamageProcessor::Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context)
{
    // 获取伤害处理使用的世界时间
    UWorld* World = Context.GetWorld();

    if (!ensureMsgf(World != nullptr, TEXT("[UBBBM]Monster damage requires a valid world")))
    {
        return;
    }

    // 逐帧消费实体积累的待处理伤害
    MonsterQuery.ForEachEntityChunk(Context, [](FMassExecutionContext& ChunkContext)
    {
        TArrayView<FMonsterHealthFragment> Healths = ChunkContext.GetMutableFragmentView<FMonsterHealthFragment>();
        TArrayView<FMonsterDamageEventFragment> DamageEvents = ChunkContext.GetMutableFragmentView<FMonsterDamageEventFragment>();
        const TConstArrayView<FMonsterStateFragment> States = ChunkContext.GetFragmentView<FMonsterStateFragment>();

        for (int32 Index = 0; Index < ChunkContext.GetNumEntities(); ++Index)
        {
            FMonsterDamageEventFragment& DamageEvent = DamageEvents[Index];
            FMonsterHealthFragment& Health = Healths[Index];

            // 已死亡实体丢弃尚未消费的请求 不触发复活或延长回收时间
            if (States[Index].State == EMonsterState::Dead || Health.CurrentHealth <= 0.0f)
            {
                DamageEvent.PendingRequests.Reset();
                continue;
            }

            for (const FMonsterDamageRequest& Request : DamageEvent.PendingRequests)
            {
                if (Health.CurrentHealth <= 0.0f)
                {
                    break;
                }

                Health.CurrentHealth = FMath::Max(Health.CurrentHealth - Request.Damage, 0.0f);
                DamageEvent.LastDamageRequest = Request;
                DamageEvent.bReceivedDamage = true;
                UE_LOG(LogTemp, Verbose, TEXT("[UBBBM]Damage consumed Entity=%d Damage=%.2f Health=%.2f Causer=%s Instigator=%s Hit=%s Direction=%s"),
                    ChunkContext.GetEntity(Index).Index, Request.Damage, Health.CurrentHealth,
                    *GetNameSafe(Request.DamageCauser.Get()), *GetNameSafe(Request.Instigator.Get()),
                    *Request.HitLocation.ToCompactString(), *Request.HitDirection.ToCompactString());
            }

            DamageEvent.PendingRequests.Reset();
        }
    });
}
