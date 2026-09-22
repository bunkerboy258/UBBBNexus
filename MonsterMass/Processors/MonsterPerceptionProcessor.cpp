#include "BBBWork/UBBBNexus/MonsterMass/Processors/MonsterPerceptionProcessor.h"

#include "GameFramework/Pawn.h"
#include "Kismet/GameplayStatics.h"
#include "MassCommonFragments.h"
#include "MassExecutionContext.h"
#include "BBBWork/UBBBNexus/MonsterMass/Processors/MonsterDamageProcessor.h"
#include "BBBWork/UBBBNexus/MonsterMass/Entity/MonsterRuntimeData.h"

UMonsterPerceptionProcessor::UMonsterPerceptionProcessor()
    : MonsterQuery(*this)
{
    bAutoRegisterWithProcessingPhases = true;
    bRequiresGameThreadExecution = true;
    ExecutionOrder.ExecuteAfter.Add(UMonsterDamageProcessor::StaticClass()->GetFName());
    ExecutionFlags = static_cast<uint8>(EProcessorExecutionFlags::AllNetModes);
}

void UMonsterPerceptionProcessor::ConfigureQueries(const TSharedRef<FMassEntityManager>& EntityManager)
{
    MonsterQuery.AddRequirement<FTransformFragment>(EMassFragmentAccess::ReadOnly);
    MonsterQuery.AddRequirement<FMonsterPerceptionFragment>(EMassFragmentAccess::ReadOnly);
    MonsterQuery.AddRequirement<FMonsterTargetRequestFragment>(EMassFragmentAccess::ReadWrite);
    MonsterQuery.AddTagRequirement<FMonsterTag>(EMassFragmentPresence::All);
}

void UMonsterPerceptionProcessor::Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context)
{
    UWorld* World = Context.GetWorld();

    if (!ensureMsgf(World != nullptr, TEXT("[UBBBM]Monster perception requires a valid world")))
    {
        return;
    }

    // 获取当前世界中的玩家目标
    APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(World, 0);

    // 每帧根据视野范围更新目标请求
    MonsterQuery.ForEachEntityChunk(Context, [PlayerPawn, World](FMassExecutionContext& ChunkContext)
    {
        const TConstArrayView<FTransformFragment> Transforms = ChunkContext.GetFragmentView<FTransformFragment>();
        const TConstArrayView<FMonsterPerceptionFragment> Perceptions = ChunkContext.GetFragmentView<FMonsterPerceptionFragment>();
        TArrayView<FMonsterTargetRequestFragment> Targets = ChunkContext.GetMutableFragmentView<FMonsterTargetRequestFragment>();

        for (int32 Index = 0; Index < ChunkContext.GetNumEntities(); ++Index)
        {
            FMonsterTargetRequestFragment& Target = Targets[Index];

            const float SightRange = FMath::Max(Perceptions[Index].SightRange, 0.0f);
            const FVector MonsterLocation = Transforms[Index].GetTransform().GetLocation();

            // 玩家无效或超出范围时清除目标并进入侦察状态
            if (!IsValid(PlayerPawn) || FVector::DistSquared2D(MonsterLocation, PlayerPawn->GetActorLocation()) > FMath::Square(SightRange))
            {
                Target.bHasTarget = false;

                Target.TargetActor.Reset();
                continue;
            }

            // 记录玩家当前位置供导航处理器使用
            Target.TargetLocation = PlayerPawn->GetActorLocation();
            Target.LastSeenTime = World->GetTimeSeconds();
            Target.bHasTarget = true;
            Target.TargetActor = PlayerPawn;
        }
    });
}
