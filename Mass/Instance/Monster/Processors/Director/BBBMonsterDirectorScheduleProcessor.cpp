#include "BBBMonsterDirectorScheduleProcessor.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Pawn.h"
#include "MassExecutionContext.h"
#include "BBBWork/UBBBNexus/Mass/Core/BBBMassProcessingGroups.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Config/BBBMonsterDirectorDefinition.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Director/BBBMonsterDirectorFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Director/BBBMonsterDirectorSpawnFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Director/BBBMonsterSpawnOwnershipFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Health/BBBMonsterHealthFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Tags/BBBMonsterDirectorTag.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Tags/BBBMonsterTag.h"

UBBBMonsterDirectorScheduleProcessor::UBBBMonsterDirectorScheduleProcessor()
    : DirectorQuery(*this), MonsterQuery(*this)
{
    bAutoRegisterWithProcessingPhases = true;
    bRequiresGameThreadExecution = true;
    ProcessingPhase = EMassProcessingPhase::PrePhysics;
    ExecutionFlags = static_cast<uint8>(EProcessorExecutionFlags::Server | EProcessorExecutionFlags::Standalone);
    ExecutionOrder.ExecuteInGroup = BBBMassProcessingGroups::Decision;
    ExecutionOrder.ExecuteAfter.Add(BBBMassProcessingGroups::Parse);
    ExecutionOrder.ExecuteAfter.Add(TEXT("BBBMonsterDamageProcessor"));
}

void UBBBMonsterDirectorScheduleProcessor::ConfigureQueries(const TSharedRef<FMassEntityManager>&)
{
    DirectorQuery.AddTagRequirement<FBBBMonsterDirectorTag>(EMassFragmentPresence::All);
    DirectorQuery.AddRequirement<FBBBMonsterDirectorFragment>(EMassFragmentAccess::ReadWrite);
    DirectorQuery.AddRequirement<FBBBMonsterDirectorSpawnFragment>(EMassFragmentAccess::ReadWrite);
    MonsterQuery.AddTagRequirement<FBBBMonsterTag>(EMassFragmentPresence::All);
    MonsterQuery.AddRequirement<FBBBMonsterHealthFragment>(EMassFragmentAccess::ReadOnly);
    MonsterQuery.AddRequirement<FBBBMonsterSpawnOwnershipFragment>(EMassFragmentAccess::ReadOnly);
}

int32 UBBBMonsterDirectorScheduleProcessor::CalculateRequest(const FBBBMonsterDirectorStageDefinition& Stage,
    const int32 Spawned, const int32 Alive, const int32 WorldSlots)
{
    const int32 Remaining = FMath::Max(0, Stage.TotalCount - Spawned);
    const int32 LocalSlots = FMath::Max(0, Stage.MaxAlive - Alive);
    const int32 Desired = Stage.Mode == EBBBMonsterDirectorSpawnMode::Once ? Remaining : FMath::Min(Stage.BatchCount, Remaining);
    if (Stage.Mode != EBBBMonsterDirectorSpawnMode::Refill && FMath::Min(LocalSlots, WorldSlots) < Desired)
    {
        return 0;
    }
    return FMath::Max(0, FMath::Min3(Desired, LocalSlots, WorldSlots));
}

void UBBBMonsterDirectorScheduleProcessor::Execute(FMassEntityManager&, FMassExecutionContext& Context)
{
    UWorld* World = Context.GetWorld();
    const double Now = World->GetTimeSeconds();
    TMap<FGuid, FIntPoint> Counts;
    int32 WorldAlive = 0;
    MonsterQuery.ForEachEntityChunk(Context, [&Counts, &WorldAlive](FMassExecutionContext& Chunk)
    {
        const auto Health = Chunk.GetFragmentView<FBBBMonsterHealthFragment>();
        const auto Ownership = Chunk.GetFragmentView<FBBBMonsterSpawnOwnershipFragment>();
        for (int32 Index = 0; Index < Chunk.GetNumEntities(); ++Index)
        {
            const bool bAlive = Health[Index].CurrentHealth > 0.0f;
            WorldAlive += bAlive ? 1 : 0;
            if (Ownership[Index].DirectorId.IsValid())
            {
                auto& Count = Counts.FindOrAdd(Ownership[Index].DirectorId);
                Count.X += bAlive ? 1 : 0;
                ++Count.Y;
            }
        }
    });

    int32 Reserved = 0;
    int32 WorldLimit = MAX_int32;
    DirectorQuery.ForEachEntityChunk(Context, [&Reserved, &WorldLimit](FMassExecutionContext& Chunk)
    {
        const auto States = Chunk.GetFragmentView<FBBBMonsterDirectorFragment>();
        const auto Spawns = Chunk.GetFragmentView<FBBBMonsterDirectorSpawnFragment>();
        for (int32 Index = 0; Index < Chunk.GetNumEntities(); ++Index)
        {
            Reserved += Spawns[Index].Reserved;
            if (const auto* Definition = States[Index].Definition.Get())
            {
                WorldLimit = FMath::Min(WorldLimit, Definition->WorldMaxAlive);
            }
        }
    });

    DirectorQuery.ForEachEntityChunk(Context, [World, Now, &Counts, &Reserved, WorldAlive, WorldLimit](FMassExecutionContext& Chunk)
    {
        auto States = Chunk.GetMutableFragmentView<FBBBMonsterDirectorFragment>();
        auto Spawns = Chunk.GetMutableFragmentView<FBBBMonsterDirectorSpawnFragment>();
        for (int32 Index = 0; Index < Chunk.GetNumEntities(); ++Index)
        {
            auto& State = States[Index];
            auto& Spawn = Spawns[Index];
            const FIntPoint Count = Counts.FindRef(State.InstanceId);
            if (State.Alive != Count.X || State.EntityCount != Count.Y)
            {
                State.Alive = Count.X;
                State.EntityCount = Count.Y;
                ++State.Revision;
            }
            const auto* Definition = State.Definition.Get();
            if (!Definition || !Definition->Stages.IsValidIndex(State.StageIndex))
            {
                continue;
            }
            if (Spawn.Reserved > 0 && !Spawn.bCreating && Now - Spawn.StartedAt >= Definition->QueryTimeout)
            {
                Reserved -= Spawn.Reserved;
                Spawn.Reserved = 0;
                ++Spawn.RequestId;
                Spawn.bNeedsQuery = false;
                Spawn.bResultReady = false;
                Spawn.Locations.Reset();
                State.NextSpawnTime = Now + Definition->RetryInterval;
                State.Reason = TEXT("出生查询超时 已释放预留");
                ++State.Revision;
            }
            if (Now < State.NextUpdateTime)
            {
                continue;
            }
            State.NextUpdateTime = Now + Definition->UpdateInterval;
            if (State.Phase == EBBBMonsterDirectorPhase::Waiting && State.TotalSpawned == 0 && State.EntityCount > 0)
            {
                State.Phase = EBBBMonsterDirectorPhase::Stopped;
                State.Reason = TEXT("旧运行的小怪或尸体尚未回收 不能重新开始");
                ++State.Revision;
                continue;
            }
            const auto& Stage = Definition->Stages[State.StageIndex];
            if (State.Phase == EBBBMonsterDirectorPhase::Waiting)
            {
                bool bActivate = Stage.Activation == EBBBMonsterDirectorActivation::Automatic;
                if (Stage.Activation == EBBBMonsterDirectorActivation::PlayerEnter)
                {
                    for (FConstPlayerControllerIterator It = World->GetPlayerControllerIterator(); It; ++It)
                    {
                        const APawn* Pawn = It->Get() ? It->Get()->GetPawn() : nullptr;
                        if (!Pawn || !Pawn->CanBeDamaged())
                        {
                            continue;
                        }
                        for (const auto& Region : State.Regions)
                        {
                            bActivate |= Stage.RegionIds.Contains(Region.Id) && Region.Contains(Pawn->GetActorLocation());
                        }
                    }
                }
                if (!bActivate)
                {
                    continue;
                }
                State.Phase = EBBBMonsterDirectorPhase::Spawning;
                State.Reason = TEXT("阶段条件满足");
                ++State.Revision;
            }
            if (State.Phase == EBBBMonsterDirectorPhase::Spawning && State.StageSpawned >= Stage.TotalCount && Spawn.Reserved == 0)
            {
                State.Phase = EBBBMonsterDirectorPhase::Clearing;
                State.Reason = TEXT("本阶段额度已用完 等待清理");
                ++State.Revision;
            }
            if (State.Phase == EBBBMonsterDirectorPhase::Clearing && State.Alive == 0 && Spawn.Reserved == 0)
            {
                ++State.StageIndex;
                State.StageSpawned = 0;
                State.NextSpawnTime = Now;
                State.Phase = Definition->Stages.IsValidIndex(State.StageIndex) ? EBBBMonsterDirectorPhase::Waiting : EBBBMonsterDirectorPhase::Completed;
                State.Reason = State.Phase == EBBBMonsterDirectorPhase::Completed ? TEXT("全部阶段已完成") : TEXT("等待下一阶段");
                ++State.Revision;
                continue;
            }
            if (State.Phase != EBBBMonsterDirectorPhase::Spawning || Spawn.Reserved > 0 || Now < State.NextSpawnTime)
            {
                continue;
            }
            const int32 Request = CalculateRequest(Stage, State.StageSpawned, State.Alive, WorldLimit - WorldAlive - Reserved);
            if (Request == 0)
            {
                State.Reason = TEXT("等待存活或完整批次额度");
                State.NextSpawnTime = Now + Definition->RetryInterval;
                ++State.Revision;
                continue;
            }
            ++Spawn.RequestId;
            Spawn.Reserved = Request;
            Spawn.StartedAt = Now;
            Spawn.bNeedsQuery = true;
            Spawn.bResultReady = false;
            Spawn.bCreating = false;
            Spawn.Locations.Reset();
            Reserved += Request;
            State.Reason = TEXT("已预留数量 正在申请出生位置");
            ++State.Revision;
        }
    });
}
