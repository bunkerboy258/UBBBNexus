#include "BBBWork/UBBBNexus/MonsterMass/MonsterPerceptionProcessor.h"

#include "GameFramework/Pawn.h"
#include "Kismet/GameplayStatics.h"
#include "MassCommonFragments.h"
#include "MassExecutionContext.h"
#include "BBBWork/UBBBNexus/MonsterMass/MonsterRuntimeData.h"

UMonsterPerceptionProcessor::UMonsterPerceptionProcessor()
    : MonsterQuery(*this)
{
    bAutoRegisterWithProcessingPhases = true;
    bRequiresGameThreadExecution = true;
    ExecutionFlags = static_cast<uint8>(EProcessorExecutionFlags::AllNetModes);
}

void UMonsterPerceptionProcessor::ConfigureQueries(const TSharedRef<FMassEntityManager>& EntityManager)
{
    MonsterQuery.AddRequirement<FTransformFragment>(EMassFragmentAccess::ReadOnly);
    MonsterQuery.AddRequirement<FMonsterPerceptionFragment>(EMassFragmentAccess::ReadOnly);
    MonsterQuery.AddRequirement<FMonsterTargetRequestFragment>(EMassFragmentAccess::ReadWrite);
    MonsterQuery.AddRequirement<FMonsterStateFragment>(EMassFragmentAccess::ReadWrite);
    MonsterQuery.AddTagRequirement<FMonsterTag>(EMassFragmentPresence::All);
}

void UMonsterPerceptionProcessor::Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context)
{
    UWorld* World = Context.GetWorld();

    if (!ensureMsgf(World != nullptr, TEXT("[UBBBM]Monster perception requires a valid world")))
    {
        return;
    }

    APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(World, 0);

    MonsterQuery.ForEachEntityChunk(Context, [PlayerPawn, World](FMassExecutionContext& ChunkContext)
    {
        const TConstArrayView<FTransformFragment> Transforms = ChunkContext.GetFragmentView<FTransformFragment>();
        const TConstArrayView<FMonsterPerceptionFragment> Perceptions = ChunkContext.GetFragmentView<FMonsterPerceptionFragment>();
        TArrayView<FMonsterTargetRequestFragment> Targets = ChunkContext.GetMutableFragmentView<FMonsterTargetRequestFragment>();
        TArrayView<FMonsterStateFragment> States = ChunkContext.GetMutableFragmentView<FMonsterStateFragment>();

        for (int32 Index = 0; Index < ChunkContext.GetNumEntities(); ++Index)
        {
            FMonsterTargetRequestFragment& Target = Targets[Index];
            FMonsterStateFragment& State = States[Index];

            const float SightRange = FMath::Max(Perceptions[Index].SightRange, 0.0f);
            const FVector MonsterLocation = Transforms[Index].GetTransform().GetLocation();

            if (!IsValid(PlayerPawn) || FVector::DistSquared2D(MonsterLocation, PlayerPawn->GetActorLocation()) > FMath::Square(SightRange))
            {
                Target.bHasTarget = false;

                if (State.State != EMonsterState::Dead)
                {
                    State.State = EMonsterState::Scout;
                    State.StateEnteredTime = World->GetTimeSeconds();
                }

                continue;
            }

            Target.TargetLocation = PlayerPawn->GetActorLocation();
            Target.LastSeenTime = World->GetTimeSeconds();
            Target.bHasTarget = true;
        }
    });
}
