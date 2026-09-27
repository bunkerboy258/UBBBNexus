#include "BBBWork/UBBBNexus/ProjectileMass/Processors/BBBProjectileMovementProcessor.h"

#include "MassCommonFragments.h"
#include "MassExecutionContext.h"
#include "MassMovementFragments.h"
#include "BBBWork/UBBBNexus/ProjectileMass/Entity/BBBProjectileRuntimeFragment.h"
#include "BBBWork/UBBBNexus/ProjectileMass/Entity/BBBProjectileTag.h"

UBBBProjectileMovementProcessor::UBBBProjectileMovementProcessor()
    : ProjectileQuery(*this)
{
    bAutoRegisterWithProcessingPhases = true;
    bRequiresGameThreadExecution = false;
    ProcessingPhase = EMassProcessingPhase::PostPhysics;
    ExecutionFlags = static_cast<uint8>(EProcessorExecutionFlags::AllNetModes);
}

void UBBBProjectileMovementProcessor::ConfigureQueries(const TSharedRef<FMassEntityManager>&)
{
    ProjectileQuery.AddRequirement<FTransformFragment>(EMassFragmentAccess::ReadWrite);
    ProjectileQuery.AddRequirement<FMassVelocityFragment>(EMassFragmentAccess::ReadOnly);
    ProjectileQuery.AddRequirement<FBBBProjectileRuntimeFragment>(EMassFragmentAccess::ReadWrite);
    ProjectileQuery.AddTagRequirement<FBBBProjectileTag>(EMassFragmentPresence::All);
}

void UBBBProjectileMovementProcessor::Execute(FMassEntityManager&, FMassExecutionContext& Context)
{
    const float DeltaSeconds = FMath::Max(Context.GetDeltaTimeSeconds(), 0.0f);

    ProjectileQuery.ForEachEntityChunk(Context, [DeltaSeconds](FMassExecutionContext& ChunkContext)
    {
        TArrayView<FTransformFragment> Transforms = ChunkContext.GetMutableFragmentView<FTransformFragment>();
        const TConstArrayView<FMassVelocityFragment> Velocities = ChunkContext.GetFragmentView<FMassVelocityFragment>();
        TArrayView<FBBBProjectileRuntimeFragment> RuntimeFragments = ChunkContext.GetMutableFragmentView<FBBBProjectileRuntimeFragment>();

        for (int32 Index = 0; Index < ChunkContext.GetNumEntities(); ++Index)
        {
            FBBBProjectileRuntimeFragment& Runtime = RuntimeFragments[Index];

            if (Runtime.bPendingDestroy)
            {
                continue;
            }

            FTransform& Transform = Transforms[Index].GetMutableTransform();
            Runtime.PreviousLocation = Transform.GetLocation();
            Transform.SetLocation(Runtime.PreviousLocation + Velocities[Index].Value * DeltaSeconds);
        }
    });
}
