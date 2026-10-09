#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Processors/Spawn/BBBMonsterInitializationProcessor.h"

#include "Engine/World.h"
#include "MassExecutionContext.h"
#include "MassCommandBuffer.h"
#include "BBBWork/UBBBNexus/Mass/Core/BBBMassProcessingGroups.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Config/BBBMonsterDefinition.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Config/BBBMonsterVariationDefinition.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Spawn/BBBMonsterVariationFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Movement/BBBMonsterMovementFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Behavior/BBBMonsterBehaviorFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Network/BBBMonsterNetworkFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Tags/BBBMonsterInitializationPendingTag.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Tags/BBBMonsterTag.h"

namespace
{
    uint32 MixBirthBits(uint32 Value)
    {
        Value ^= Value >> 16;
        Value *= 0x7feb352du;
        Value ^= Value >> 15;
        Value *= 0x846ca68bu;
        return Value ^ (Value >> 16);
    }

    float BirthSample(const uint32 Seed, const uint32 Channel)
    {
        return static_cast<float>(MixBirthBits(Seed ^ Channel) >> 8) * (1.0f / 16777216.0f);
    }
}

UBBBMonsterInitializationProcessor::UBBBMonsterInitializationProcessor()
    : PendingQuery(*this)
{
    bAutoRegisterWithProcessingPhases = true;
    bRequiresGameThreadExecution = true;
    ProcessingPhase = EMassProcessingPhase::PrePhysics;
    ExecutionFlags = static_cast<uint8>(EProcessorExecutionFlags::AllNetModes);
    ExecutionOrder.ExecuteInGroup = BBBMassProcessingGroups::Initialization;
    ExecutionOrder.ExecuteAfter.Add(BBBMassProcessingGroups::Parse);
    ExecutionOrder.ExecuteBefore.Add(BBBMassProcessingGroups::Decision);
}

bool UBBBMonsterInitializationProcessor::InitializeBirth(const FGuid& Identity, const UBBBMonsterDefinition& Definition,
    FBBBMonsterVariationFragment& Variation, FBBBMonsterMovementFragment& Movement, FBBBMonsterBehaviorFragment& Behavior)
{
    const UBBBMonsterVariationDefinition* Settings = Definition.Variation;
    if (!Identity.IsValid() || !Settings || !Settings->IsValid())
    {
        return false;
    }

    Variation.Seed = (MixBirthBits(Identity.A ^ MixBirthBits(Identity.B) ^ MixBirthBits(Identity.C) ^ MixBirthBits(Identity.D)) & 0x7fffffffu) | 1u;
    Variation.Infection = static_cast<uint8>(BirthSample(Variation.Seed, 0x1f83d9abu) * 256.0f);
    const float Infection = static_cast<float>(Variation.Infection) / 255.0f;
    const float RunnerChance = FMath::Lerp(Settings->LowInfectionRunnerChance, Settings->HighInfectionRunnerChance, Infection);
    Movement.MaxGait = BirthSample(Variation.Seed, 0x5be0cd19u) < RunnerChance ? EBBBMonsterGait::Sprint : EBBBMonsterGait::Walk;
    const float SpeedNoise = BirthSample(Variation.Seed, 0xa54ff53au) + BirthSample(Variation.Seed, 0x510e527fu) - 1.0f;
    Variation.SpeedScale = 1.0f + SpeedNoise * Settings->SpeedVariation;
    Variation.PhaseOffset = BirthSample(Variation.Seed, 0x9b05688cu);
    Variation.LocomotionStyle = static_cast<uint8>(BirthSample(Variation.Seed, 0x3c6ef372u) * 3.0f);
    Movement.WalkSpeed = Definition.WalkSpeed * Variation.SpeedScale;
    Movement.RunSpeed = Definition.RunSpeed * Variation.SpeedScale;
    Movement.SprintSpeed = Definition.SprintSpeed * Variation.SpeedScale;
    Movement.Acceleration = Definition.Acceleration * (1.0f + (BirthSample(Variation.Seed, 0xbb67ae85u) * 2.0f - 1.0f) * Settings->AccelerationVariation);
    Behavior.AlertDuration = Definition.AlertDuration * (1.0f + (BirthSample(Variation.Seed, 0x6a09e667u) * 2.0f - 1.0f) * Settings->AlertVariation);
    return true;
}

void UBBBMonsterInitializationProcessor::ConfigureQueries(const TSharedRef<FMassEntityManager>& EntityManager)
{
    PendingQuery.AddTagRequirement<FBBBMonsterTag>(EMassFragmentPresence::All);
    PendingQuery.AddTagRequirement<FBBBMonsterInitializationPendingTag>(EMassFragmentPresence::All);
    PendingQuery.AddRequirement<FBBBMonsterNetworkFragment>(EMassFragmentAccess::ReadWrite);
    PendingQuery.AddRequirement<FBBBMonsterVariationFragment>(EMassFragmentAccess::ReadWrite);
    PendingQuery.AddRequirement<FBBBMonsterMovementFragment>(EMassFragmentAccess::ReadWrite);
    PendingQuery.AddRequirement<FBBBMonsterBehaviorFragment>(EMassFragmentAccess::ReadWrite);
}

void UBBBMonsterInitializationProcessor::Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context)
{
    const bool bHost = Context.GetWorld()->GetNetMode() != NM_Client;
    PendingQuery.ForEachEntityChunk(Context, [bHost](FMassExecutionContext& Chunk)
    {
        auto Network = Chunk.GetMutableFragmentView<FBBBMonsterNetworkFragment>();
        auto Variation = Chunk.GetMutableFragmentView<FBBBMonsterVariationFragment>();
        auto Movement = Chunk.GetMutableFragmentView<FBBBMonsterMovementFragment>();
        auto Behavior = Chunk.GetMutableFragmentView<FBBBMonsterBehaviorFragment>();
        for (int32 Index = 0; Index < Chunk.GetNumEntities(); ++Index)
        {
            if (!Network[Index].InstanceId.IsValid() && bHost)
            {
                Network[Index].InstanceId = FGuid::NewGuid();
            }
            if (!Network[Index].InstanceId.IsValid())
            {
                continue;
            }
            const UBBBMonsterDefinition* Definition = Network[Index].Definition.Get();
            if (!ensureMsgf(Definition && InitializeBirth(Network[Index].InstanceId, *Definition, Variation[Index], Movement[Index], Behavior[Index]),
                TEXT("[BBBMonsterVariation]Invalid birth configuration Entity=%d"), Chunk.GetEntity(Index).Index))
            {
                continue;
            }
            Chunk.Defer().RemoveTag<FBBBMonsterInitializationPendingTag>(Chunk.GetEntity(Index));
        }
    });
}
