#include "BBBWork/UBBBNexus/Mass/Instance/Projectile/Processors/Movement/BBBProjectileMovementProcessor.h"
#include "MassExecutionContext.h"
#include "MassCommonFragments.h"
#include "MassMovementFragments.h"
#include "Engine/World.h"
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
    EntityQuery.AddRequirement<FMassVelocityFragment>(EMassFragmentAccess::ReadWrite);
    EntityQuery.AddRequirement<FBBBProjectileMotionFragment>(EMassFragmentAccess::ReadWrite);
    EntityQuery.AddRequirement<FBBBProjectileLifetimeFragment>(EMassFragmentAccess::ReadOnly);
}

void UBBBProjectileMovementProcessor::Execute(FMassEntityManager&, FMassExecutionContext& Context)
{
    const float Delta = Context.GetDeltaTimeSeconds();
    const float GravityZ = Context.GetWorld()->GetGravityZ();
    EntityQuery.ForEachEntityChunk(Context, [Delta, GravityZ](FMassExecutionContext& Chunk)
    {
        auto Transforms = Chunk.GetMutableFragmentView<FTransformFragment>();
        auto Motion = Chunk.GetMutableFragmentView<FBBBProjectileMotionFragment>();
        auto Velocity = Chunk.GetMutableFragmentView<FMassVelocityFragment>();
        const auto Life = Chunk.GetFragmentView<FBBBProjectileLifetimeFragment>();
        for (int32 Index = 0; Index < Chunk.GetNumEntities(); ++Index)
        {
            Motion[Index].PreviousLocation = Transforms[Index].GetTransform().GetLocation();
            if (Motion[Index].bInitialized && !Life[Index].bPendingDestroy && !Motion[Index].bResting)
            {
                float Step = FMath::Min(Delta, Life[Index].RemainingSeconds);
                if (Life[Index].FuseRemainingSeconds > 0.0f)
                {
                    Step = FMath::Min(Step, Life[Index].FuseRemainingSeconds);
                }

                const FVector Acceleration(0.0, 0.0, GravityZ * Motion[Index].GravityScale);
                Transforms[Index].GetMutableTransform().AddToTranslation(Velocity[Index].Value * Step + Acceleration * (0.5f * Step * Step));
                Velocity[Index].Value += Acceleration * Step;
                if (!Velocity[Index].Value.IsNearlyZero())
                {
                    Transforms[Index].GetMutableTransform().SetRotation(Velocity[Index].Value.ToOrientationQuat());
                }
            }
        }
    });
}
