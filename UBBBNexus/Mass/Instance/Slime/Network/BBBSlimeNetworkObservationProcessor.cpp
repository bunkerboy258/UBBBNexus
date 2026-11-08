#include "BBBSlimeNetworkObservationProcessor.h"
#include "MassExecutionContext.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "BBBWork/UBBBNexus/Mass/Core/BBBMassProcessingGroups.h"
#include "BBBWork/UBBBNexus/Mass/Network/BBBMassNetworkActor.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Slime/Fragments/BBBSlimeFragment.h"

UBBBSlimeNetworkObservationProcessor::UBBBSlimeNetworkObservationProcessor() : SlimeQuery(*this)
{
    bAutoRegisterWithProcessingPhases = true;
    bRequiresGameThreadExecution = true;
    QueryBasedPruning = EMassQueryBasedPruning::Never;
    ProcessingPhase = EMassProcessingPhase::PrePhysics;
    ExecutionFlags = static_cast<uint8>(EProcessorExecutionFlags::Server | EProcessorExecutionFlags::Standalone);
    ExecutionOrder.ExecuteInGroup = BBBMassProcessingGroups::Network;
    ExecutionOrder.ExecuteAfter.Add(BBBMassProcessingGroups::Movement);
}

void UBBBSlimeNetworkObservationProcessor::ConfigureQueries(const TSharedRef<FMassEntityManager>&)
{
    SlimeQuery.AddRequirement<FBBBSlimeFragment>(EMassFragmentAccess::ReadOnly);
}

void UBBBSlimeNetworkObservationProcessor::Execute(FMassEntityManager&, FMassExecutionContext& Context)
{
    UWorld* World = Context.GetWorld();
    if (!World || (World->GetNetMode() == NM_Standalone && !World->IsRecordingReplay())
        || World->GetTimeSeconds() < NextPublishTime)
    {
        return;
    }
    NextPublishTime = World->GetTimeSeconds() + 0.1f;
    ABBBMassNetworkActor* Transport = nullptr;
    for (TActorIterator<ABBBMassNetworkActor> It(World); It; ++It)
    {
        Transport = *It;
        break;
    }
    if (!Transport)
    {
        Transport = World->SpawnActor<ABBBMassNetworkActor>();
    }
    if (!Transport)
    {
        return;
    }
    TArray<FBBBSlimeFact> States;
    SlimeQuery.ForEachEntityChunk(Context, [&](FMassExecutionContext& Chunk)
    {
        for (const auto& State : Chunk.GetFragmentView<FBBBSlimeFragment>())
        {
            if (State.Fact.IsValid() && States.Num() < 128 && World->GetTimeSeconds() < State.Fact.EndsAt)
            {
                States.Add(State.Fact);
            }
        }
    });
    States.Sort([](const auto& A, const auto& B) { return A.InstanceId < B.InstanceId; });
    Transport->PublishSlimes(States);
}
