#include "BBBWork/UBBBNexus/Mass/Instance/Projectile/Processors/Presentation/BBBProjectilePresentationProcessor.h"

#include "MassExecutionContext.h"
#include "MassCommonFragments.h"
#include "BBBWork/UBBBNexus/Mass/Core/BBBMassProcessingGroups.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Projectile/Fragments/Movement/BBBProjectileMotionFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Projectile/Fragments/Presentation/BBBProjectilePresentationFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Projectile/Presentation/BBBProjectilePresentation.h"
UBBBProjectilePresentationProcessor::UBBBProjectilePresentationProcessor()
    : EntityQuery(*this)
{
    bAutoRegisterWithProcessingPhases = true;
    bRequiresGameThreadExecution = true;
    ProcessingPhase = EMassProcessingPhase::PostPhysics;
    ExecutionFlags = static_cast<uint8>(EProcessorExecutionFlags::AllNetModes);
    ExecutionOrder.ExecuteInGroup = BBBMassProcessingGroups::Presentation;
    ExecutionOrder.ExecuteAfter.Add(BBBMassProcessingGroups::Collision);
}

void UBBBProjectilePresentationProcessor::ConfigureQueries(const TSharedRef<FMassEntityManager>&)
{
    EntityQuery.AddRequirement<FTransformFragment>(EMassFragmentAccess::ReadOnly);
    EntityQuery.AddRequirement<FBBBProjectileMotionFragment>(EMassFragmentAccess::ReadOnly);
    EntityQuery.AddRequirement<FBBBProjectilePresentationFragment>(EMassFragmentAccess::ReadOnly);
}

void UBBBProjectilePresentationProcessor::Execute(FMassEntityManager&, FMassExecutionContext& Context)
{
    UWorld* World = Context.GetWorld();
    if (World->GetNetMode() == NM_DedicatedServer)
    {
        return;
    }

    EntityQuery.ForEachEntityChunk(Context, [World](FMassExecutionContext& Chunk)
    {
        const auto Transforms = Chunk.GetFragmentView<FTransformFragment>();
        const auto Motion = Chunk.GetFragmentView<FBBBProjectileMotionFragment>();
        const auto Presentation = Chunk.GetFragmentView<FBBBProjectilePresentationFragment>();
        FBBBProjectilePresentation::Publish(*World, Transforms, Motion, Presentation);
    });
}
