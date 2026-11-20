#include "BBBWork/UBBBNexus/Mass/Instance/Projectile/Processors/Lifetime/BBBProjectileLifetimeProcessor.h"
#include "MassExecutionContext.h"
#include "MassCommonFragments.h"
#include "MassMovementFragments.h"
#include "BBBWork/UBBBNexus/Mass/Core/BBBMassSubsystem.h"
#include "BBBWork/UBBBNexus/Mass/Core/BBBMassProcessingGroups.h"

#include "BBBWork/UBBBNexus/Mass/Instance/Projectile/Fragments/Lifetime/BBBProjectileLifetimeFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Projectile/Fragments/Movement/BBBProjectileMotionFragment.h"

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
    EntityQuery.AddRequirement<FBBBProjectileMotionFragment>(EMassFragmentAccess::ReadOnly);
}

void UBBBProjectileLifetimeProcessor::Execute(FMassEntityManager&, FMassExecutionContext& Context)
{
    UWorld* World = Context.GetWorld();
    UBBBMassSubsystem* Mass = World->GetSubsystem<UBBBMassSubsystem>();
    const float Delta = Context.GetDeltaTimeSeconds();
    EntityQuery.ForEachEntityChunk(Context, [Delta, Mass](FMassExecutionContext& Chunk)
    {
        auto Lives = Chunk.GetMutableFragmentView<FBBBProjectileLifetimeFragment>();
        const auto Motion = Chunk.GetFragmentView<FBBBProjectileMotionFragment>();
        for (int32 Index = 0; Index < Chunk.GetNumEntities(); ++Index)
        {
            if (!Motion[Index].bInitialized)
            {
                continue;
            }

            Lives[Index].RemainingSeconds -= Delta;
            if (Lives[Index].FuseRemainingSeconds > 0.0f)
            {
                Lives[Index].FuseRemainingSeconds = FMath::Max(0.0f, Lives[Index].FuseRemainingSeconds - Delta);
            }
            if (Lives[Index].bPendingDestroy || Lives[Index].RemainingSeconds <= 0.0f)
            {
                Mass->ReleaseProjectile(Chunk.GetEntity(Index));
            }
        }
    });
}
