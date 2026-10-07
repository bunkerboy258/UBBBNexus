#include "BBBMonsterHitReactionProcessor.h"

#include "BBBWork/UBBBNexus/Mass/Core/BBBMassProcessingGroups.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/HitReaction/BBBMonsterHitReactionFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Presentation/BBBMonsterPresentationActor.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Presentation/BBBMonsterPresentationComponent.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Presentation/BBBMonsterBloodPresentationSubsystem.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Processors/Presentation/BBBMonsterPresentationProcessor.h"
#include "MassActorSubsystem.h"
#include "MassExecutionContext.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Network/BBBMonsterNetworkFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Config/BBBMonsterDefinition.h"

UBBBMonsterHitReactionProcessor::UBBBMonsterHitReactionProcessor()
    : EntityQuery(*this)
{
    bAutoRegisterWithProcessingPhases = true;
    bRequiresGameThreadExecution = true;
    ProcessingPhase = EMassProcessingPhase::PrePhysics;
    ExecutionFlags = static_cast<uint8>(EProcessorExecutionFlags::AllNetModes);
    ExecutionOrder.ExecuteInGroup = BBBMassProcessingGroups::Presentation;
    ExecutionOrder.ExecuteAfter.Add(BBBMassProcessingGroups::Decision);
    ExecutionOrder.ExecuteBefore.Add(UBBBMonsterPresentationProcessor::StaticClass()->GetFName());
}

void UBBBMonsterHitReactionProcessor::ConfigureQueries(const TSharedRef<FMassEntityManager>&)
{
    EntityQuery.AddRequirement<FBBBMonsterHitReactionFragment>(EMassFragmentAccess::ReadWrite);
    EntityQuery.AddRequirement<FBBBMonsterNetworkFragment>(EMassFragmentAccess::ReadOnly);
}

void UBBBMonsterHitReactionProcessor::Execute(FMassEntityManager&, FMassExecutionContext& Context)
{
    TMap<const UBBBMonsterBloodPresentationDefinition*, TArray<FBBBMonsterBloodImpact>> Batches;
    EntityQuery.ForEachEntityChunk(Context, [&Batches](FMassExecutionContext& Chunk)
    {
        auto Hits = Chunk.GetMutableFragmentView<FBBBMonsterHitReactionFragment>();
        const auto Definitions = Chunk.GetFragmentView<FBBBMonsterNetworkFragment>();
        for (int32 Index = 0; Index < Chunk.GetNumEntities(); ++Index)
        {
            auto& Hit = Hits[Index];
            Hit.Age = FMath::Min(Hit.Age + Chunk.GetDeltaTimeSeconds(), 10.0f);
            if (Hit.Serial == Hit.PublishedSerial)
            {
                continue;
            }
            Hit.PublishedSerial = Hit.Serial;
            const auto* Definition = Definitions[Index].Definition.Get();
            const auto* Settings = Definition ? Definition->BloodPresentation.Get() : nullptr;
            if (Hit.Age <= 0.2f && Settings)
            {
                Batches.FindOrAdd(Settings).Add({Hit.Position, Hit.Direction, Hit.Normal, Hit.Region});
            }
        }
    });
    auto* Presentation = Context.GetWorld()->GetSubsystem<UBBBMonsterBloodPresentationSubsystem>();
    for (const auto& Batch : Batches)
    {
        Presentation->Publish(*Batch.Key, Batch.Value);
    }
}
