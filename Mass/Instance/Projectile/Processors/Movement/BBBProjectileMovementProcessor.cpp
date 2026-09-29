#include "BBBWork/UBBBNexus/Mass/Instance/Projectile/Processors/Movement/BBBProjectileMovementProcessor.h"
#include "MassExecutionContext.h"
#include "MassCommonFragments.h"
#include "MassMovementFragments.h"
#include "BBBWork/UBBBNexus/Mass/Core/BBBMassProcessingGroups.h"

#include "BBBWork/UBBBNexus/Mass/Instance/Projectile/Fragments/Movement/BBBProjectileMotionFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Projectile/Fragments/Lifetime/BBBProjectileLifetimeFragment.h"
UBBBProjectileMovementProcessor::UBBBProjectileMovementProcessor()
    : EntityQuery(*this)
{
    bAutoRegisterWithProcessingPhases = true;
    bRequiresGameThreadExecution = false;
    ProcessingPhase = EMassProcessingPhase::FrameEnd;
    ExecutionFlags = static_cast<uint8>(EProcessorExecutionFlags::AllNetModes);
    ExecutionOrder.ExecuteInGroup = BBBMassProcessingGroups::Movement;
    ExecutionOrder.ExecuteAfter.Add(BBBMassProcessingGroups::Parse);
}

void UBBBProjectileMovementProcessor::ConfigureQueries(const TSharedRef<FMassEntityManager>&)
{
    EntityQuery.AddRequirement<FTransformFragment>(EMassFragmentAccess::ReadWrite);
    EntityQuery.AddRequirement<FMassVelocityFragment>(EMassFragmentAccess::ReadOnly);
    EntityQuery.AddRequirement<FBBBProjectileMotionFragment>(EMassFragmentAccess::ReadWrite);
    EntityQuery.AddRequirement<FBBBProjectileLifetimeFragment>(EMassFragmentAccess::ReadOnly);
}

void UBBBProjectileMovementProcessor::Execute(FMassEntityManager&, FMassExecutionContext& Context)
{
    const float Delta = Context.GetDeltaTimeSeconds();
    EntityQuery.ForEachEntityChunk(Context, [Delta](FMassExecutionContext& Chunk)
    {
        auto Transforms = Chunk.GetMutableFragmentView<FTransformFragment>();
        auto Motion = Chunk.GetMutableFragmentView<FBBBProjectileMotionFragment>();
        const auto Velocity = Chunk.GetFragmentView<FMassVelocityFragment>();
        const auto Life = Chunk.GetFragmentView<FBBBProjectileLifetimeFragment>();
        for (int32 Index = 0; Index < Chunk.GetNumEntities(); ++Index)
        {
            Motion[Index].PreviousLocation = Transforms[Index].GetTransform().GetLocation();
            if (Motion[Index].bInitialized && !Life[Index].bPendingDestroy)
            {
                Transforms[Index].GetMutableTransform().AddToTranslation(Velocity[Index].Value * FMath::Min(Delta, Life[Index].RemainingSeconds));
            }
        }
    });
}
