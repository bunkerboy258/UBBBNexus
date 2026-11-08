#include "BBBMonsterDirectorParseProcessor.h"
#include "MassEntitySubsystem.h"
#include "MassExecutionContext.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Spawn/BBBMonsterSpawnInputFragment.h"
#include "Engine/World.h"
#include "BBBWork/UBBBNexus/Mass/Core/BBBMassProcessingGroups.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Config/BBBMonsterDirectorDefinition.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Director/BBBMonsterDirectorInputFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Tags/BBBMonsterDirectorTag.h"

UBBBMonsterDirectorParseProcessor::UBBBMonsterDirectorParseProcessor()
    : DirectorQuery(*this)
{
    bAutoRegisterWithProcessingPhases = true;
    bRequiresGameThreadExecution = true;
    ProcessingPhase = EMassProcessingPhase::PrePhysics;
    ExecutionFlags = static_cast<uint8>(EProcessorExecutionFlags::AllNetModes);
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
    const bool bHost = Context.GetWorld()->GetNetMode() != NM_Client;
    const double Now = Context.GetWorld()->GetTimeSeconds();
    DirectorQuery.ForEachEntityChunk(Context, [bHost, Now](FMassExecutionContext& Chunk)
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
            if (bHost && Job.OwnerId == State.InstanceId && Job.Generation == State.Generation
                && Job.StageIndex == State.StageIndex && Job.RequestId == Spawn.RequestId)
            {
                const int32 Delta = FMath::Max(0, Job.Created - Spawn.Accounted);
                State.StageSpawned += Delta;
                State.TotalSpawned += Delta;
                State.EntityCount += Delta;
                Spawn.Accounted = Job.Created;
                if (Delta > 0)
                {
                    ++State.Revision;
                }
                if (!Job.bActive && !Job.bCreating && Spawn.Reserved > 0)
                {
                    Spawn.Reserved = 0;
                    State.NextSpawnTime = Now + (Job.Created > 0
                        ? State.Definition->Stages[State.StageIndex].Interval : State.Definition->RetryInterval);
                    State.Reason = Job.Reason;
                    ++State.Revision;
                }
            }
            const auto Consume = [&State, &Spawn](auto& Slot, const bool bAllowed)
            {
                if (!Slot.bActive)
                {
                    return;
                }
                Slot.bActive = false;
                if (bAllowed && Slot.Packet.IsValid() && Slot.Packet.CanApply(State, Spawn))
                {
                    Slot.Packet.Apply(State, Spawn);
                }
            };
            Consume(Input.Initialize, bHost);
            Consume(Input.Start, bHost);
            Consume(Input.Resume, bHost);
            Consume(Input.Trigger, bHost);
            Consume(Input.Pause, bHost);
            Consume(Input.Stop, bHost);
            if (bHost && State.Phase != EBBBMonsterDirectorPhase::Spawning)
            {
                Job.bActive = false;
                Requests[Index].Request.bActive = false;
            }
            Consume(Input.State, !bHost);
        }
    });
}

bool UBBBMonsterDirectorParseProcessor::Query(UWorld& World, const FMassEntityHandle Entity, FBBBMonsterDirectorReplicationState& Result)
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
    Result.Generation = State->Generation;
    Result.Revision = State->Revision;
    Result.StageIndex = State->StageIndex;
    Result.Phase = State->Phase;
    Result.StageSpawned = State->StageSpawned;
    Result.TotalSpawned = State->TotalSpawned;
    Result.Alive = State->Alive;
    Result.Pending = Spawn->Reserved;
    Result.Reason = State->Reason;
    const auto* Definition = State->Definition.Get();
    if (Definition && Definition->Stages.IsValidIndex(State->StageIndex))
    {
        Result.StageId = Definition->Stages[State->StageIndex].Id;
    }
    return true;
}
