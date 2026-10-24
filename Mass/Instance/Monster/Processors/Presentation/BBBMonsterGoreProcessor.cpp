#include "BBBMonsterGoreProcessor.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Config/BBBMonsterPresentationDefinition.h"

#include "BBBWork/UBBBNexus/Mass/Core/BBBMassProcessingGroups.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Config/BBBMonsterDefinition.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Presentation/BBBMonsterGoreFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Health/BBBMonsterHealthFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/HitReaction/BBBMonsterHitReactionFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Network/BBBMonsterNetworkFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Tags/BBBMonsterInitializationPendingTag.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Processors/Health/BBBMonsterDamageProcessor.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Processors/Network/BBBMonsterNetworkObservationProcessor.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Presentation/BBBMonsterGorePresentationComponent.h"
#include "MassActorSubsystem.h"
#include "MassExecutionContext.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"

UBBBMonsterGoreProcessor::UBBBMonsterGoreProcessor() : Query(*this)
{
    bAutoRegisterWithProcessingPhases = true;
    bRequiresGameThreadExecution = true;
    ProcessingPhase = EMassProcessingPhase::PrePhysics;
    ExecutionFlags = static_cast<uint8>(EProcessorExecutionFlags::AllNetModes);
    ExecutionOrder.ExecuteInGroup = BBBMassProcessingGroups::Damage;
    ExecutionOrder.ExecuteAfter.Add(UBBBMonsterDamageProcessor::StaticClass()->GetFName());
    ExecutionOrder.ExecuteBefore.Add(UBBBMonsterNetworkObservationProcessor::StaticClass()->GetFName());
}

void UBBBMonsterGoreProcessor::ConfigureQueries(const TSharedRef<FMassEntityManager>&)
{
    Query.AddRequirement<FBBBMonsterGoreFragment>(EMassFragmentAccess::ReadWrite);
    Query.AddRequirement<FBBBMonsterHealthFragment>(EMassFragmentAccess::ReadOnly);
    Query.AddRequirement<FBBBMonsterHitReactionFragment>(EMassFragmentAccess::ReadOnly);
    Query.AddRequirement<FBBBMonsterNetworkFragment>(EMassFragmentAccess::ReadOnly);
    Query.AddRequirement<FMassActorFragment>(EMassFragmentAccess::ReadOnly);
    Query.AddTagRequirement<FBBBMonsterInitializationPendingTag>(EMassFragmentPresence::None);
}

void UBBBMonsterGoreProcessor::Execute(FMassEntityManager&, FMassExecutionContext& Context)
{
    const float Now = Context.GetWorld()->GetTimeSeconds();
    const bool bAuthority = Context.GetWorld()->GetNetMode() != NM_Client;
    Query.ForEachEntityChunk(Context, [Now, bAuthority](FMassExecutionContext& Chunk)
    {
        auto Results = Chunk.GetMutableFragmentView<FBBBMonsterGoreFragment>();
        const auto Health = Chunk.GetFragmentView<FBBBMonsterHealthFragment>();
        const auto Hits = Chunk.GetFragmentView<FBBBMonsterHitReactionFragment>();
        const auto Networks = Chunk.GetFragmentView<FBBBMonsterNetworkFragment>();
        const auto Actors = Chunk.GetFragmentView<FMassActorFragment>();
        for (int32 Index = 0; Index < Chunk.GetNumEntities(); ++Index)
        {
            const auto* Definition = Networks[Index].Definition.Get();
            if (!Definition || !Definition->Presentation || !Definition->Presentation->Gore)
            {
                continue;
            }
            auto* Actor = Actors[Index].Get();
            auto* Mesh = Actor ? Actor->FindComponentByClass<USkeletalMeshComponent>() : nullptr;
            UBBBMonsterGorePresentationComponent::UpdateWounds(Results[Index], *Definition->Presentation->Gore,
                Health[Index].DestroyedParts, Hits[Index], Mesh, Now, bAuthority);
        }
    });
}
