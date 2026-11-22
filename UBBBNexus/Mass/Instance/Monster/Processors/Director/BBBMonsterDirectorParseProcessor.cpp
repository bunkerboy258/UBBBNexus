#include "BBBMonsterDirectorParseProcessor.h"
#include "MassEntitySubsystem.h"
#include "MassExecutionContext.h"
#include "Engine/World.h"
#include "BBBWork/UBBBNexus/Mass/Core/BBBMassProcessingGroups.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Config/BBBMonsterDirectorDefinition.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Director/BBBMonsterDirectorInputFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Spawn/BBBMonsterSpawnInputFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Tags/BBBMonsterDirectorTag.h"

UBBBMonsterDirectorParseProcessor::UBBBMonsterDirectorParseProcessor()
    : DirectorQuery(*this)
{
    bAutoRegisterWithProcessingPhases = true;
    bRequiresGameThreadExecution = true;
    ProcessingPhase = EMassProcessingPhase::PrePhysics;
    ExecutionFlags = static_cast<uint8>(EProcessorExecutionFlags::Server | EProcessorExecutionFlags::Standalone);
    ExecutionOrder.ExecuteInGroup = BBBMassProcessingGroups::Parse;
}

void UBBBMonsterDirectorParseProcessor::ConfigureQueries(const TSharedRef<FMassEntityManager>&)
{
    DirectorQuery.AddTagRequirement<FBBBMonsterDirectorTag>(EMassFragmentPresence::All);
    DirectorQuery.AddRequirement<FBBBMonsterDirectorInputFragment>(EMassFragmentAccess::ReadWrite);
    DirectorQuery.AddRequirement<FBBBMonsterDirectorFragment>(EMassFragmentAccess::ReadWrite);
    DirectorQuery.AddRequirement<FBBBMonsterDirectorSpawnFragment>(EMassFragmentAccess::ReadWrite);
    DirectorQuery.AddRequirement<FBBBMonsterSpawnFragment>(EMassFragmentAccess::ReadWrite);
    DirectorQuery.AddRequirement<FBBBMonsterSpawnInputFragment>(EMassFragmentAccess::ReadWrite);
}

void UBBBMonsterDirectorParseProcessor::Execute(FMassEntityManager&, FMassExecutionContext& Context)
{
    DirectorQuery.ForEachEntityChunk(Context, [](FMassExecutionContext& Chunk)
    {
        auto Inputs = Chunk.GetMutableFragmentView<FBBBMonsterDirectorInputFragment>();
        auto States = Chunk.GetMutableFragmentView<FBBBMonsterDirectorFragment>();
        auto Spawns = Chunk.GetMutableFragmentView<FBBBMonsterDirectorSpawnFragment>();
        auto Jobs = Chunk.GetMutableFragmentView<FBBBMonsterSpawnFragment>();
        auto Requests = Chunk.GetMutableFragmentView<FBBBMonsterSpawnInputFragment>();
        for (int32 Index = 0; Index < Chunk.GetNumEntities(); ++Index)
        {
            auto& Input = Inputs[Index];
            auto& State = States[Index];
            auto& Spawn = Spawns[Index];
            auto& Job = Jobs[Index];
            if (Job.OwnerId == State.InstanceId && Job.Generation == State.Generation && Job.RequestId == Spawn.RequestId)
            {
                const int32 Delta = FMath::Max(0, Job.Created - Spawn.Accounted);
                if (Delta > 0)
                {
                    State.TotalSpawned += Delta;
                    Spawn.Accounted = Job.Created;
                    if (Spawn.Source == EBBBMonsterSpawnSource::EventWave)
                    {
                        for (auto& Event : State.ActiveEvents)
                        {
                            if (Event.EventId == Spawn.EventId)
                            {
                                Event.Spawned += Delta;
                                break;
                            }
                        }
                    }
                }
                if (!Job.bActive && !Job.bCreating && Spawn.Reserved > 0)
                {
                    Spawn.Reserved = 0;
                    State.Reason = Job.Reason;
                }
            }
            const auto Consume = [&State, &Spawn](auto& Slot)
            {
                if (!Slot.bActive)
                {
                    return;
                }
                Slot.bActive = false;
                if (Slot.Packet.IsValid() && Slot.Packet.CanApply(State, Spawn))
                {
                    Slot.Packet.Apply(State, Spawn);
                }
            };
            Consume(Input.Initialize);
            Consume(Input.Start);
            Consume(Input.SetDynamicHordePaused);
            Consume(Input.TriggerEventWave);
            Consume(Input.StopEventWave);
            Consume(Input.Stop);
            if (!State.bRunning)
            {
                Job.bActive = false;
                Requests[Index].Request.bActive = false;
            }
        }
    });
}

bool UBBBMonsterDirectorParseProcessor::Query(UWorld& World, const FMassEntityHandle Entity, FBBBMonsterDirectorStatus& Result)
{
    const UMassEntitySubsystem* Entities = World.GetSubsystem<UMassEntitySubsystem>();
    if (!Entities || !Entities->GetEntityManager().IsEntityValid(Entity))
    {
        return false;
    }
    const auto& Manager = Entities->GetEntityManager();
    const auto* State = Manager.GetFragmentDataPtr<FBBBMonsterDirectorFragment>(Entity);
    const auto* Spawn = Manager.GetFragmentDataPtr<FBBBMonsterDirectorSpawnFragment>(Entity);
    if (!State || !Spawn)
    {
        return false;
    }
    Result.InstanceId = State->InstanceId;
    Result.bRunning = State->bRunning;
    Result.bDynamicHordePaused = State->bDynamicHordePaused;
    Result.Alive = State->Alive;
    Result.TotalSpawned = State->TotalSpawned;
    Result.Pending = Spawn->Reserved;
    Result.ActivatedPopulationRegions = State->ActivatedPopulationRegionIds.Num();
    Result.TotalPopulationRegions = 0;
    for (const auto& Region : State->Regions)
    {
        if (Region.Role == EBBBMonsterSpawnRegionRole::Population)
        {
            ++Result.TotalPopulationRegions;
        }
    }
    const auto* Definition = State->Definition.Get();
    if (State->bRunning && Definition && Definition->bEnableDynamicHorde && !State->bDynamicHordePaused && State->NextDynamicHordeTime > 0.0)
    {
        Result.DynamicHordeCooldown = FMath::Max(0.0f, static_cast<float>(State->NextDynamicHordeTime - World.GetTimeSeconds()));
    }
    else
    {
        Result.DynamicHordeCooldown = -1.0f;
    }
    Result.ActiveEventIds.Reset();
    for (const auto& Event : State->ActiveEvents)
    {
        Result.ActiveEventIds.Add(Event.EventId);
    }
    Result.Reason = State->Reason;
    return true;
}
