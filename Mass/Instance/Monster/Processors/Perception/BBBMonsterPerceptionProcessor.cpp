#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Processors/Perception/BBBMonsterPerceptionProcessor.h"
#include "BBBWork/UBBBNexus/Mass/Core/BBBMassProcessingGroups.h"

#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Tags/BBBMonsterTag.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Perception/BBBMonsterPerceptionFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Perception/BBBMonsterTargetFragment.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "MassCommonFragments.h"
#include "MassExecutionContext.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Processors/Health/BBBMonsterDamageProcessor.h"

UBBBMonsterPerceptionProcessor::UBBBMonsterPerceptionProcessor()
    : MonsterQuery(*this)
{
    bAutoRegisterWithProcessingPhases = true;
    ProcessingPhase = EMassProcessingPhase::PrePhysics;
    ExecutionOrder.ExecuteInGroup = BBBMassProcessingGroups::Decision;
    bRequiresGameThreadExecution = true;
    ExecutionOrder.ExecuteAfter.Add(UBBBMonsterDamageProcessor::StaticClass()->GetFName());
    ExecutionFlags = static_cast<uint8>(EProcessorExecutionFlags::Server | EProcessorExecutionFlags::Standalone);
}

void UBBBMonsterPerceptionProcessor::ConfigureQueries(const TSharedRef<FMassEntityManager>& EntityManager)
{
    MonsterQuery.AddRequirement<FTransformFragment>(EMassFragmentAccess::ReadOnly);
    MonsterQuery.AddRequirement<FBBBMonsterPerceptionFragment>(EMassFragmentAccess::ReadOnly);
    MonsterQuery.AddRequirement<FBBBMonsterTargetFragment>(EMassFragmentAccess::ReadWrite);
    MonsterQuery.AddTagRequirement<FBBBMonsterTag>(EMassFragmentPresence::All);
}

void UBBBMonsterPerceptionProcessor::Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context)
{
    UWorld* World = Context.GetWorld();

    if (!ensureMsgf(World != nullptr, TEXT("[UBBBM]Monster perception requires a valid world")))
    {
        return;
    }

    // 获取当前世界中的玩家目标
    TArray<APawn*, TInlineAllocator<8>> Players;
    for (FConstPlayerControllerIterator It = World->GetPlayerControllerIterator(); It; ++It)
    {
        if (It->IsValid() && It->Get()->GetPawn() != nullptr)
        {
            Players.Add(It->Get()->GetPawn());
        }
    }

    // 每帧根据视野范围更新目标请求
    MonsterQuery.ForEachEntityChunk(Context, [&Players, World](FMassExecutionContext& ChunkContext)
    {
        const TConstArrayView<FTransformFragment> Transforms = ChunkContext.GetFragmentView<FTransformFragment>();
        const TConstArrayView<FBBBMonsterPerceptionFragment> Perceptions = ChunkContext.GetFragmentView<FBBBMonsterPerceptionFragment>();
        TArrayView<FBBBMonsterTargetFragment> Targets = ChunkContext.GetMutableFragmentView<FBBBMonsterTargetFragment>();

        for (int32 Index = 0; Index < ChunkContext.GetNumEntities(); ++Index)
        {
            FBBBMonsterTargetFragment& Target = Targets[Index];

            const float SightRange = FMath::Max(Perceptions[Index].SightRange, 0.0f);
            const FVector MonsterLocation = Transforms[Index].GetTransform().GetLocation();
            APawn* PlayerPawn = nullptr;
            float BestDistance = FMath::Square(SightRange);
            for (APawn* Candidate : Players)
            {
                if (!IsValid(Candidate) || !Candidate->CanBeDamaged())
                {
                    continue;
                }
                const float Distance = FVector::DistSquared2D(MonsterLocation, Candidate->GetActorLocation());
                if (Distance <= BestDistance)
                {
                    BestDistance = Distance;
                    PlayerPawn = Candidate;
                }
            }

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
