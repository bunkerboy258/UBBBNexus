#include "BBBMonsterHitReactionProcessor.h"

#include "BBBWork/UBBBNexus/Mass/Core/BBBMassProcessingGroups.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/HitReaction/BBBMonsterHitReactionFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Presentation/BBBMonsterPresentationActor.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Presentation/BBBMonsterPresentationComponent.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Presentation/BBBMonsterBloodPresentationSubsystem.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Processors/Presentation/BBBMonsterPresentationProcessor.h"
#include "MassActorSubsystem.h"
#include "MassExecutionContext.h"

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
    EntityQuery.AddRequirement<FMassActorFragment>(EMassFragmentAccess::ReadOnly);
}

void UBBBMonsterHitReactionProcessor::Execute(FMassEntityManager&, FMassExecutionContext& Context)
{
    TMap<const UBBBMonsterBloodPresentationDefinition*, TArray<FBBBMonsterBloodImpact>> Batches;
    EntityQuery.ForEachEntityChunk(Context, [&Batches](FMassExecutionContext& Chunk)
    {
        auto Hits = Chunk.GetMutableFragmentView<FBBBMonsterHitReactionFragment>();
        const auto Actors = Chunk.GetFragmentView<FMassActorFragment>();
        for (int32 Index = 0; Index < Chunk.GetNumEntities(); ++Index)
        {
            auto& Hit = Hits[Index];
            Hit.Age = FMath::Min(Hit.Age + Chunk.GetDeltaTimeSeconds(), 10.0f);
            if (Hit.Serial == Hit.PublishedSerial)
            {
                continue;
            }
            Hit.PublishedSerial = Hit.Serial;
            const auto* Actor = Cast<ABBBMonsterPresentationActor>(Actors[Index].Get());
            const auto* Settings = Actor ? Actor->GetMonsterPresentation()->GetBloodPresentation() : nullptr;
            if (Settings)
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
