#include "BBBMonsterSpawnInitializationProcessor.h"
#include "MassCommonFragments.h"
#include "MassExecutionContext.h"
#include "Engine/World.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Network/BBBMonsterNetworkFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Config/BBBMonsterDefinition.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Config/BBBMonsterPresentationDefinition.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Presentation/BBBMonsterBloodPresentationSubsystem.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Presentation/BBBMonsterGibPresentationSubsystem.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Spawn/BBBMonsterVariationFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Spawn/BBBMonsterSpawnData.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Director/BBBMonsterSpawnOwnershipFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Tags/BBBMonsterTag.h"

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
}

void UBBBMonsterSpawnInitializationProcessor::Execute(FMassEntityManager&, FMassExecutionContext& Context)
{
    const auto& Data = Context.GetAuxData().Get<FBBBMonsterSpawnData>();
    int32 Offset = 0;
    SpawnQuery.ForEachEntityChunk(Context, [&Data, &Offset](FMassExecutionContext& Chunk)
    {
        const auto Rules = Chunk.GetFragmentView<FBBBMonsterNetworkFragment>();
        if (Chunk.GetWorld()->GetNetMode() != NM_DedicatedServer)
        {
            for (const auto &Rule : Rules)
            {
                if (Rule.Definition.IsValid() && Rule.Definition->Presentation)
                {
                    auto *Settings = Rule.Definition->Presentation.Get();
                    Chunk.GetWorld()->GetSubsystem<UBBBMonsterGibPresentationSubsystem>()->Prepare(*Settings);
                    if (Settings->Blood)
                    {
                        Chunk.GetWorld()->GetSubsystem<UBBBMonsterBloodPresentationSubsystem>()->Prepare(*Settings->Blood);
                    }
                }
            }
        }
        auto Transforms = Chunk.GetMutableFragmentView<FTransformFragment>();
        auto Owners = Chunk.GetMutableFragmentView<FBBBMonsterSpawnOwnershipFragment>();
        auto Variation = Chunk.GetMutableFragmentView<FBBBMonsterVariationFragment>();
        for (int32 Index = 0; Index < Chunk.GetNumEntities(); ++Index)
        {
            if (!ensureMsgf(Data.Transforms.IsValidIndex(Offset), TEXT("导演出生数据与批量实体数量不一致")))
            {
                return;
            }
            Variation[Index].Seed = Data.Seeds.IsValidIndex(Offset) ? Data.Seeds[Offset] : 0;
            Transforms[Index].GetMutableTransform() = Data.Transforms[Offset++];
            Owners[Index].DirectorId = Data.DirectorId;
            Owners[Index].Generation = Data.Generation;
            Owners[Index].StageIndex = Data.StageIndex;
        }
    });
}
