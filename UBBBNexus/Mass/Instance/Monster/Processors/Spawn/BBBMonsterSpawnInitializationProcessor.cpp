#include "BBBMonsterSpawnInitializationProcessor.h"
#include "MassCommonFragments.h"
#include "MassExecutionContext.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/PlayerState.h"
#include "GameFramework/Pawn.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Network/BBBMonsterNetworkFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Config/BBBMonsterDefinition.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Config/BBBMonsterPresentationDefinition.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Presentation/BBBMonsterBloodPresentationSubsystem.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Presentation/BBBMonsterGibPresentationSubsystem.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Spawn/BBBMonsterVariationFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Spawn/BBBMonsterSpawnData.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Perception/BBBMonsterPerceptionFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Director/BBBMonsterSpawnOwnershipFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Tags/BBBMonsterTag.h"

namespace
{
    int32 FindNearestAlivePlayerId(UWorld* World, const FVector& Location)
    {
        if (!World)
        {
            return INDEX_NONE;
        }
        int32 BestPlayerId = INDEX_NONE;
        double BestDistSq = TNumericLimits<double>::Max();
        for (FConstPlayerControllerIterator It = World->GetPlayerControllerIterator(); It; ++It)
        {
            const APlayerController* Controller = It->Get();
            const APawn* Pawn = Controller ? Controller->GetPawn() : nullptr;
            const APlayerState* PlayerState = Controller ? Controller->GetPlayerState<APlayerState>() : nullptr;
            if (!Pawn || !Pawn->CanBeDamaged() || !PlayerState || PlayerState->GetPlayerId() < 0)
            {
                continue;
            }
            const double DistSq = FVector::DistSquared(Location, Pawn->GetActorLocation());
            if (DistSq < BestDistSq)
            {
                BestDistSq = DistSq;
                BestPlayerId = PlayerState->GetPlayerId();
            }
        }
        return BestPlayerId;
    }
}

UBBBMonsterSpawnInitializationProcessor::UBBBMonsterSpawnInitializationProcessor()
    : SpawnQuery(*this)
{
    bAutoRegisterWithProcessingPhases = false;
    bRequiresGameThreadExecution = true;
}

void UBBBMonsterSpawnInitializationProcessor::ConfigureQueries(const TSharedRef<FMassEntityManager>&)
{
    SpawnQuery.AddTagRequirement<FBBBMonsterTag>(EMassFragmentPresence::All);
    SpawnQuery.AddRequirement<FTransformFragment>(EMassFragmentAccess::ReadWrite);
    SpawnQuery.AddRequirement<FBBBMonsterNetworkFragment>(EMassFragmentAccess::ReadOnly);
    SpawnQuery.AddRequirement<FBBBMonsterSpawnOwnershipFragment>(EMassFragmentAccess::ReadWrite);
    SpawnQuery.AddRequirement<FBBBMonsterVariationFragment>(EMassFragmentAccess::ReadWrite);
    SpawnQuery.AddRequirement<FBBBMonsterPerceptionFragment>(EMassFragmentAccess::ReadWrite);
}

void UBBBMonsterSpawnInitializationProcessor::Execute(FMassEntityManager&, FMassExecutionContext& Context)
{
    const auto& Data = Context.GetAuxData().Get<FBBBMonsterSpawnData>();
    UWorld* World = Context.GetWorld();
    int32 Offset = 0;
    SpawnQuery.ForEachEntityChunk(Context, [World, &Data, &Offset](FMassExecutionContext& Chunk)
    {
        const auto Rules = Chunk.GetFragmentView<FBBBMonsterNetworkFragment>();
        if (World && World->GetNetMode() != NM_DedicatedServer)
        {
            for (const auto& Rule : Rules)
            {
                if (Rule.Definition.IsValid() && Rule.Definition->Presentation)
                {
                    auto* Settings = Rule.Definition->Presentation.Get();
                    World->GetSubsystem<UBBBMonsterGibPresentationSubsystem>()->Prepare(*Settings);
                    if (Settings->Blood)
                    {
                        World->GetSubsystem<UBBBMonsterBloodPresentationSubsystem>()->Prepare(*Settings->Blood);
                    }
                }
            }
        }
        auto Transforms = Chunk.GetMutableFragmentView<FTransformFragment>();
        auto Owners = Chunk.GetMutableFragmentView<FBBBMonsterSpawnOwnershipFragment>();
        auto Variation = Chunk.GetMutableFragmentView<FBBBMonsterVariationFragment>();
        auto Perception = Chunk.GetMutableFragmentView<FBBBMonsterPerceptionFragment>();
        for (int32 Index = 0; Index < Chunk.GetNumEntities(); ++Index)
        {
            if (!ensureMsgf(Data.Transforms.IsValidIndex(Offset), TEXT("导演出生数据与批量实体数量不一致")))
            {
                return;
            }
            const FTransform SpawnTransform = Data.Transforms[Offset];
            Variation[Index].Seed = Data.Seeds.IsValidIndex(Offset) ? Data.Seeds[Offset] : 0;
            Transforms[Index].GetMutableTransform() = SpawnTransform;
            ++Offset;
            Owners[Index].DirectorId = Data.DirectorId;
            Owners[Index].Generation = Data.Generation;
            Owners[Index].Source = Data.Source;
            Owners[Index].EventId = Data.EventId;
            if (Data.InitialTargetPlayerId >= 0)
            {
                Perception[Index].InitialTargetPlayerId = Data.InitialTargetPlayerId;
            }
            else if (Data.bAlertOnSpawn)
            {
                Perception[Index].InitialTargetPlayerId = FindNearestAlivePlayerId(World, SpawnTransform.GetLocation());
            }
            else
            {
                Perception[Index].InitialTargetPlayerId = INDEX_NONE;
            }
        }
    });
}
