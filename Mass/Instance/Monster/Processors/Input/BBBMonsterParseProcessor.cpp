#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Processors/Input/BBBMonsterParseProcessor.h"
#include "MassExecutionContext.h"
#include "MassCommonFragments.h"
#include "MassMovementFragments.h"
#include "BBBWork/UBBBNexus/Mass/Core/BBBMassProcessingGroups.h"

#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Health/BBBMonsterHealthInputFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Network/BBBMonsterNetworkInputFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/HitReaction/BBBMonsterHitReactionInputFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Perception/BBBMonsterPerceptionInputFragment.h"
#include "Engine/World.h"
UBBBMonsterParseProcessor::UBBBMonsterParseProcessor()
    : EntityQuery(*this)
{
    bAutoRegisterWithProcessingPhases = true;
    bRequiresGameThreadExecution = true;
    ProcessingPhase = EMassProcessingPhase::PrePhysics;
    ExecutionFlags = static_cast<uint8>(EProcessorExecutionFlags::AllNetModes);
    ExecutionOrder.ExecuteInGroup = BBBMassProcessingGroups::Parse;
}

void UBBBMonsterParseProcessor::ConfigureQueries(const TSharedRef<FMassEntityManager>&)
{
    EntityQuery.AddRequirement<FBBBMonsterHealthInputFragment>(EMassFragmentAccess::ReadWrite);
    EntityQuery.AddRequirement<FBBBMonsterNetworkInputFragment>(EMassFragmentAccess::ReadWrite);
    EntityQuery.AddRequirement<FBBBMonsterDamageFragment>(EMassFragmentAccess::ReadWrite);
    EntityQuery.AddRequirement<FBBBMonsterNetworkFragment>(EMassFragmentAccess::ReadWrite);
    EntityQuery.AddRequirement<FBBBMonsterBehaviorFragment>(EMassFragmentAccess::ReadWrite);
    EntityQuery.AddRequirement<FTransformFragment>(EMassFragmentAccess::ReadWrite);
    EntityQuery.AddRequirement<FMassVelocityFragment>(EMassFragmentAccess::ReadWrite);
    EntityQuery.AddRequirement<FBBBMonsterHitReactionInputFragment>(EMassFragmentAccess::ReadWrite);
    EntityQuery.AddRequirement<FBBBMonsterHitReactionFragment>(EMassFragmentAccess::ReadWrite);
    EntityQuery.AddRequirement<FBBBMonsterHealthFragment>(EMassFragmentAccess::ReadOnly);
    EntityQuery.AddRequirement<FBBBMonsterPerceptionInputFragment>(EMassFragmentAccess::ReadWrite);
    EntityQuery.AddRequirement<FBBBMonsterStimulusFragment>(EMassFragmentAccess::ReadWrite);
}

void UBBBMonsterParseProcessor::Execute(FMassEntityManager&, FMassExecutionContext& Context)
{
    const UWorld* World = Context.GetWorld();
    const float Now = World ? World->GetTimeSeconds() : 0.0f;
    const bool bHost = World && World->GetNetMode() != NM_Client;
    EntityQuery.ForEachEntityChunk(Context, [Now, bHost](FMassExecutionContext& Chunk)
    {
        auto HealthInputs = Chunk.GetMutableFragmentView<FBBBMonsterHealthInputFragment>();
        auto NetworkInputs = Chunk.GetMutableFragmentView<FBBBMonsterNetworkInputFragment>();
        auto Damage = Chunk.GetMutableFragmentView<FBBBMonsterDamageFragment>();
        auto Network = Chunk.GetMutableFragmentView<FBBBMonsterNetworkFragment>();
        auto State = Chunk.GetMutableFragmentView<FBBBMonsterBehaviorFragment>();
        auto Transforms = Chunk.GetMutableFragmentView<FTransformFragment>();
        auto Velocities = Chunk.GetMutableFragmentView<FMassVelocityFragment>();
        auto HitInputs = Chunk.GetMutableFragmentView<FBBBMonsterHitReactionInputFragment>();
        auto Hits = Chunk.GetMutableFragmentView<FBBBMonsterHitReactionFragment>();
        const auto Health = Chunk.GetFragmentView<FBBBMonsterHealthFragment>();
        auto PerceptionInputs = Chunk.GetMutableFragmentView<FBBBMonsterPerceptionInputFragment>();
        auto Stimuli = Chunk.GetMutableFragmentView<FBBBMonsterStimulusFragment>();
        for (int32 Index = 0; Index < Chunk.GetNumEntities(); ++Index)
        {
            auto& Sound = PerceptionInputs[Index].Sound;
            if (Sound.bActive)
            {
                Sound.bActive = false;
                if (bHost && Sound.Packet.IsValid() && Sound.Packet.CanApply(Stimuli[Index]) &&
                    Sound.Packet.Time <= Now && Sound.Packet.Time + Sound.Packet.Duration > Now && Health[Index].CurrentHealth > 0.0f)
                {
                    Sound.Packet.Apply(Stimuli[Index]);
                }
            }

            auto& Hit = HitInputs[Index].Hit;
            if (Hit.bActive)
            {
                Hit.bActive = false;
                if (Hit.Packet.IsValid() && Hit.Packet.CanApply(Health[Index]))
                {
                    Hit.Packet.Apply(Hits[Index]);
                }
            }

            auto& Fact = NetworkInputs[Index].State;
            if (Fact.bActive)
            {
                Fact.bActive = false;
                if (Fact.Packet.IsValid() && Fact.Packet.CanApply(Network[Index]))
                {
                    Fact.Packet.Apply(Network[Index], Transforms[Index], Velocities[Index], State[Index]);
                }
            }

            auto& Remote = HealthInputs[Index].RemoteDamage;
            if (Remote.bActive)
            {
                Remote.bActive = false;
                if (Remote.Packet.IsValid() && Remote.Packet.CanApply(Network[Index]))
                {
                    Remote.Packet.Apply(Damage[Index]);
                }
            }

            auto& Local = HealthInputs[Index].Damage;
            if (Local.bActive)
            {
                Local.bActive = false;
                if (Local.Packet.IsValid() && Local.Packet.CanApply(Damage[Index]))
                {
                    Local.Packet.Apply(Damage[Index]);
                }
            }
        }
    });
}
