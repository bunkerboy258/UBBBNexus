#include "BBBWork/UBBBNexus/ProjectileMass/Processors/BBBProjectileLifetimeProcessor.h"

#include "MassExecutionContext.h"
#include "BBBWork/UBBBNexus/ProjectileMass/Entity/BBBProjectileRuntimeFragment.h"
#include "BBBWork/UBBBNexus/ProjectileMass/Entity/BBBProjectileTag.h"
#include "BBBWork/UBBBNexus/ProjectileMass/Processors/BBBProjectileCollisionProcessor.h"

UBBBProjectileLifetimeProcessor::UBBBProjectileLifetimeProcessor()
    : ProjectileQuery(*this)
{
    bAutoRegisterWithProcessingPhases = true;
    bRequiresGameThreadExecution = false;
    ProcessingPhase = EMassProcessingPhase::PostPhysics;
    ExecutionFlags = static_cast<uint8>(EProcessorExecutionFlags::AllNetModes);
    ExecutionOrder.ExecuteAfter.Add(UBBBProjectileCollisionProcessor::StaticClass()->GetFName());
}

void UBBBProjectileLifetimeProcessor::ConfigureQueries(const TSharedRef<FMassEntityManager>&)
{
    ProjectileQuery.AddRequirement<FBBBProjectileRuntimeFragment>(EMassFragmentAccess::ReadWrite);
    ProjectileQuery.AddTagRequirement<FBBBProjectileTag>(EMassFragmentPresence::All);
}

void UBBBProjectileLifetimeProcessor::Execute(FMassEntityManager&, FMassExecutionContext& Context)
{
    const float DeltaSeconds = FMath::Max(Context.GetDeltaTimeSeconds(), 0.0f);

    ProjectileQuery.ForEachEntityChunk(Context, [DeltaSeconds](FMassExecutionContext& ChunkContext)
    {
        TArrayView<FBBBProjectileRuntimeFragment> RuntimeFragments = ChunkContext.GetMutableFragmentView<FBBBProjectileRuntimeFragment>();

        for (int32 Index = 0; Index < ChunkContext.GetNumEntities(); ++Index)
        {
            FBBBProjectileRuntimeFragment& Runtime = RuntimeFragments[Index];

            if (Runtime.bPendingDestroy)
            {
                ChunkContext.Defer().DestroyEntity(ChunkContext.GetEntity(Index));
                continue;
            }

            Runtime.RemainingLifetimeSeconds -= DeltaSeconds;

            if (Runtime.RemainingLifetimeSeconds <= 0.0f)
            {
                ChunkContext.Defer().DestroyEntity(ChunkContext.GetEntity(Index));
            }
        }
    });
}
