#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Processors/Input/BBBMonsterParseProcessor.h"
#include "MassExecutionContext.h"
#include "MassCommonFragments.h"
#include "MassMovementFragments.h"
#include "BBBWork/UBBBNexus/Mass/Core/BBBMassProcessingGroups.h"

#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Health/BBBMonsterHealthInputFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Network/BBBMonsterNetworkInputFragment.h"
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
    EntityQuery.AddRequirement<FBBBMonsterHealthFragment>(EMassFragmentAccess::ReadOnly);
}

void UBBBMonsterParseProcessor::Execute(FMassEntityManager&, FMassExecutionContext& Context)
{
    EntityQuery.ForEachEntityChunk(Context, [](FMassExecutionContext& Chunk)
    {
        auto HealthInputs = Chunk.GetMutableFragmentView<FBBBMonsterHealthInputFragment>();
        auto NetworkInputs = Chunk.GetMutableFragmentView<FBBBMonsterNetworkInputFragment>();
        auto Damage = Chunk.GetMutableFragmentView<FBBBMonsterDamageFragment>();
        auto Network = Chunk.GetMutableFragmentView<FBBBMonsterNetworkFragment>();
        auto State = Chunk.GetMutableFragmentView<FBBBMonsterBehaviorFragment>();
        auto Transforms = Chunk.GetMutableFragmentView<FTransformFragment>();
        auto Velocities = Chunk.GetMutableFragmentView<FMassVelocityFragment>();
        const auto Health = Chunk.GetFragmentView<FBBBMonsterHealthFragment>();
        for (int32 Index = 0; Index < Chunk.GetNumEntities(); ++Index)
        {
            auto& Fact = NetworkInputs[Index].State;
            if (Fact.bActive)
            {
                Fact.bActive = false;
                if (Fact.Packet.IsValid() && Fact.Packet.CanApply(Network[Index]))
                {
                    Fact.Packet.Apply(Network[Index], Transforms[Index], Velocities[Index], State[Index], Damage[Index]);
                }
            }

            auto& Remote = HealthInputs[Index].RemoteHealth;
            if (Remote.bActive)
            {
                Remote.bActive = false;
                if (Remote.Packet.IsValid() && Remote.Packet.CanApply(Health[Index]))
                {
                    Remote.Packet.Apply(Damage[Index]);
                }
            }

            auto& Local = HealthInputs[Index].Damage;
            if (Local.bActive)
            {
                Local.bActive = false;
                if (Local.Packet.IsValid() && Local.Packet.CanApply(Health[Index]))
                {
                    Local.Packet.Apply(Damage[Index]);
                }
            }
        }
    });
}
