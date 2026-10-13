#include "BBBMonsterDirectorSpawnInitializationProcessor.h"
#include "MassCommonFragments.h"
#include "MassExecutionContext.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Director/BBBMonsterDirectorSpawnData.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Director/BBBMonsterSpawnOwnershipFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Tags/BBBMonsterTag.h"

UBBBMonsterDirectorSpawnInitializationProcessor::UBBBMonsterDirectorSpawnInitializationProcessor()
    : SpawnQuery(*this)
{
    bAutoRegisterWithProcessingPhases = false;
    bRequiresGameThreadExecution = true;
}

void UBBBMonsterDirectorSpawnInitializationProcessor::ConfigureQueries(const TSharedRef<FMassEntityManager>&)
{
    SpawnQuery.AddTagRequirement<FBBBMonsterTag>(EMassFragmentPresence::All);
    SpawnQuery.AddRequirement<FTransformFragment>(EMassFragmentAccess::ReadWrite);
    SpawnQuery.AddRequirement<FBBBMonsterSpawnOwnershipFragment>(EMassFragmentAccess::ReadWrite);
}

void UBBBMonsterDirectorSpawnInitializationProcessor::Execute(FMassEntityManager&, FMassExecutionContext& Context)
{
    const auto& Data = Context.GetAuxData().Get<FBBBMonsterDirectorSpawnData>();
    int32 Offset = 0;
    SpawnQuery.ForEachEntityChunk(Context, [&Data, &Offset](FMassExecutionContext& Chunk)
    {
        auto Transforms = Chunk.GetMutableFragmentView<FTransformFragment>();
        auto Owners = Chunk.GetMutableFragmentView<FBBBMonsterSpawnOwnershipFragment>();
        for (int32 Index = 0; Index < Chunk.GetNumEntities(); ++Index)
        {
            if (!ensureMsgf(Data.Transforms.IsValidIndex(Offset), TEXT("导演出生数据与批量实体数量不一致")))
            {
                return;
            }
            Transforms[Index].GetMutableTransform() = Data.Transforms[Offset++];
            Owners[Index].DirectorId = Data.DirectorId;
            Owners[Index].Generation = Data.Generation;
            Owners[Index].StageIndex = Data.StageIndex;
        }
    });
}
