#include "BBBMonsterDirectorNetworkObservationProcessor.h"
#include "MassExecutionContext.h"
#include "BBBWork/UBBBNexus/Mass/Core/BBBMassProcessingGroups.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Director/BBBMonsterDirectorFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Director/BBBMonsterDirectorSpawnFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Presentation/BBBMonsterDirectorPresentationActor.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Network/BBBMonsterDirectorNetworkComponent.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Processors/Director/BBBMonsterDirectorParseProcessor.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Tags/BBBMonsterDirectorTag.h"

UBBBMonsterDirectorNetworkObservationProcessor::UBBBMonsterDirectorNetworkObservationProcessor()
    : DirectorQuery(*this)
{
    bAutoRegisterWithProcessingPhases = true;
    bRequiresGameThreadExecution = true;
    ProcessingPhase = EMassProcessingPhase::PrePhysics;
    ExecutionFlags = static_cast<uint8>(EProcessorExecutionFlags::Server | EProcessorExecutionFlags::Standalone);
    ExecutionOrder.ExecuteInGroup = BBBMassProcessingGroups::Network;
    ExecutionOrder.ExecuteAfter.Add(BBBMassProcessingGroups::Decision);
}

void UBBBMonsterDirectorNetworkObservationProcessor::ConfigureQueries(const TSharedRef<FMassEntityManager>&)
{
    DirectorQuery.AddTagRequirement<FBBBMonsterDirectorTag>(EMassFragmentPresence::All);
    DirectorQuery.AddRequirement<FBBBMonsterDirectorFragment>(EMassFragmentAccess::ReadOnly);
    DirectorQuery.AddRequirement<FBBBMonsterDirectorSpawnFragment>(EMassFragmentAccess::ReadOnly);
}

void UBBBMonsterDirectorNetworkObservationProcessor::Execute(FMassEntityManager&, FMassExecutionContext& Context)
{
    DirectorQuery.ForEachEntityChunk(Context, [](FMassExecutionContext& Chunk)
    {
        const auto States = Chunk.GetFragmentView<FBBBMonsterDirectorFragment>();
        for (int32 Index = 0; Index < Chunk.GetNumEntities(); ++Index)
        {
            auto* Presentation = States[Index].Presentation.Get();
            if (Presentation)
            {
                Presentation->GetDirectorNetwork()->Publish(Presentation->ReadState());
            }
        }
    });
}
