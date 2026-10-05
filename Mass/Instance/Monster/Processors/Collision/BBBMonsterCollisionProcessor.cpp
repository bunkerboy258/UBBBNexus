#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Processors/Collision/BBBMonsterCollisionProcessor.h"

#include "MassExecutionContext.h"
#include "MassCommonFragments.h"
#include "BBBWork/UBBBNexus/Mass/Core/BBBMassSubsystem.h"
#include "BBBWork/UBBBNexus/Mass/Core/BBBMassProcessingGroups.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Movement/BBBMonsterAvoidanceFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Health/BBBMonsterHealthFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Processors/Movement/BBBMonsterAvoidanceProcessor.h"

UBBBMonsterCollisionProcessor::UBBBMonsterCollisionProcessor()
    : EntityQuery(*this)
{
    bAutoRegisterWithProcessingPhases = true;
    QueryBasedPruning = EMassQueryBasedPruning::Never;
    bRequiresGameThreadExecution = true;
    ProcessingPhase = EMassProcessingPhase::PrePhysics;
    ExecutionFlags = static_cast<uint8>(EProcessorExecutionFlags::AllNetModes);
    ExecutionOrder.ExecuteInGroup = BBBMassProcessingGroups::Collision;
    ExecutionOrder.ExecuteAfter.Add(BBBMassProcessingGroups::Movement);
}

void UBBBMonsterCollisionProcessor::ConfigureQueries(const TSharedRef<FMassEntityManager>&)
{
    EntityQuery.AddRequirement<FTransformFragment>(EMassFragmentAccess::ReadOnly);
    EntityQuery.AddRequirement<FBBBMonsterAvoidanceFragment>(EMassFragmentAccess::ReadOnly);
    EntityQuery.AddRequirement<FBBBMonsterHealthFragment>(EMassFragmentAccess::ReadOnly);
}

void UBBBMonsterCollisionProcessor::Execute(FMassEntityManager&, FMassExecutionContext& Context)
{
    UBBBMassSubsystem* Mass = Context.GetWorld()->GetSubsystem<UBBBMassSubsystem>();
    Mass->BeginCollisionFrame();
    EntityQuery.ForEachEntityChunk(Context, [Mass](FMassExecutionContext& Chunk)
    {
        const auto Transforms = Chunk.GetFragmentView<FTransformFragment>();
        const auto Avoidance = Chunk.GetFragmentView<FBBBMonsterAvoidanceFragment>();
        const auto Health = Chunk.GetFragmentView<FBBBMonsterHealthFragment>();
        for (int32 Index = 0; Index < Chunk.GetNumEntities(); ++Index)
        {
            if (Health[Index].CurrentHealth <= 0.0f)
            {
                continue;
            }

            FBBBMassCollisionBody Body;
            Body.Entity = Chunk.GetEntity(Index);
            Body.Center = Transforms[Index].GetTransform().GetLocation();
            Body.Radius = Avoidance[Index].CollisionRadius;
            Body.Surface = SurfaceType2;
            Mass->AddCollisionBody(Body);
        }
    });
}
