#include "BBBWork/UBBBNexus/Mass/Instance/Projectile/Processors/Lifetime/BBBProjectileLifetimeProcessor.h"
#include "MassExecutionContext.h"
#include "MassCommonFragments.h"
#include "MassMovementFragments.h"
#include "BBBWork/UBBBNexus/Mass/Core/BBBMassProcessingGroups.h"

#include "BBBWork/UBBBNexus/Mass/Instance/Projectile/Fragments/Lifetime/BBBProjectileLifetimeFragment.h"
UBBBProjectileLifetimeProcessor::UBBBProjectileLifetimeProcessor()
    : EntityQuery(*this)
{
    bAutoRegisterWithProcessingPhases = true;
    bRequiresGameThreadExecution = true;
    ProcessingPhase = EMassProcessingPhase::FrameEnd;
    ExecutionFlags = static_cast<uint8>(EProcessorExecutionFlags::AllNetModes);
    ExecutionOrder.ExecuteInGroup = BBBMassProcessingGroups::Lifetime;
    ExecutionOrder.ExecuteAfter.Add(BBBMassProcessingGroups::Presentation);
}

void UBBBProjectileLifetimeProcessor::ConfigureQueries(const TSharedRef<FMassEntityManager>&)
{
    EntityQuery.AddRequirement<FBBBProjectileLifetimeFragment>(EMassFragmentAccess::ReadWrite);
}

void UBBBProjectileLifetimeProcessor::Execute(FMassEntityManager&, FMassExecutionContext& Context)
{
    const float Delta = Context.GetDeltaTimeSeconds();
    EntityQuery.ForEachEntityChunk(Context, [Delta](FMassExecutionContext& Chunk)
    {
        auto Lives = Chunk.GetMutableFragmentView<FBBBProjectileLifetimeFragment>();
        for (int32 Index = 0; Index < Chunk.GetNumEntities(); ++Index)
        {
            Lives[Index].RemainingSeconds -= Delta;
            if (Lives[Index].FuseRemainingSeconds > 0.0f)
            {
                Lives[Index].FuseRemainingSeconds = FMath::Max(0.0f, Lives[Index].FuseRemainingSeconds - Delta);
            }
            if (Lives[Index].bPendingDestroy || Lives[Index].RemainingSeconds <= 0.0f)
            {
                Chunk.Defer().DestroyEntity(Chunk.GetEntity(Index));
            }
        }
    });
}
