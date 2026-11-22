#include "BBBMonsterDirectorScheduleProcessor.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Pawn.h"
#include "MassCommonFragments.h"
#include "MassExecutionContext.h"
#include "MassCommandBuffer.h"
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "BBBWork/UBBBNexus/Mass/Core/BBBMassSubsystem.h"
#include "BBBWork/UBBBNexus/Mass/Core/BBBMassProcessingGroups.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Config/BBBMonsterDirectorDefinition.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Config/BBBMonsterSpawnDefinition.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Director/BBBMonsterDirectorFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Director/BBBMonsterDirectorSpawnFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Director/BBBMonsterSpawnOwnershipFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Health/BBBMonsterHealthFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Spawn/BBBMonsterSpawnInputFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Processors/Spawn/BBBMonsterSpawnProcessor.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Tags/BBBMonsterDirectorTag.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Tags/BBBMonsterTag.h"

namespace
{
    struct FPlayerObserver
    {
        FVector Location = FVector::ZeroVector;
        FVector ViewLocation = FVector::ZeroVector;
        FVector ViewDirection = FVector::ForwardVector;
    };

    void CollectPlayers(UWorld& World, TArray<FPlayerObserver>& OutPlayers)
    {
        OutPlayers.Reset();
        for (FConstPlayerControllerIterator It = World.GetPlayerControllerIterator(); It; ++It)
        {
            const APlayerController* Controller = It->Get();
            const APawn* Pawn = Controller ? Controller->GetPawn() : nullptr;
            if (!Pawn || !Pawn->CanBeDamaged())
            {
                continue;
            }
            if (const auto* Character = Cast<ABBBCharacter>(Pawn); Character && Character->GetLifePhase() == EBBBCharacterLifePhase::Dead)
            {
                continue;
            }
            FPlayerObserver Observer;
            Observer.Location = Pawn->GetActorLocation();
            FRotator ViewRotation;
            Controller->GetPlayerViewPoint(Observer.ViewLocation, ViewRotation);
            Observer.ViewDirection = ViewRotation.Vector();
            OutPlayers.Add(Observer);
        }
    }

    int32 RequestPriority(const EBBBMonsterSpawnSource Source)
    {
        switch (Source)
        {
        case EBBBMonsterSpawnSource::EventWave:
            return 3;
        case EBBBMonsterSpawnSource::DynamicHorde:
            return 2;
        case EBBBMonsterSpawnSource::Population:
            return 1;
        default:
            return 0;
        }
    }

    void BuildPlayerRingRegions(const TArray<FPlayerObserver>& Players, const float Radius, const float HalfHeight,
        TArray<FBBBMonsterSpawnRegionDefinition>& OutRegions)
    {
        OutRegions.Reset();
        for (int32 Index = 0; Index < Players.Num(); ++Index)
        {
            FBBBMonsterSpawnRegionDefinition Region;
            Region.Id = FName(*FString::Printf(TEXT("DynamicPlayer_%d"), Index));
            Region.Role = EBBBMonsterSpawnRegionRole::Event;
            Region.Shape = EBBBMonsterSpawnRegionShape::Cylinder;
            Region.Transform = FTransform(Players[Index].Location);
            Region.Extent = FVector(Radius, Radius, HalfHeight);
            OutRegions.Add(Region);
        }
    }
}

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
    DirectorQuery.AddRequirement<FBBBMonsterSpawnFragment>(EMassFragmentAccess::ReadWrite);
    MonsterQuery.AddTagRequirement<FBBBMonsterTag>(EMassFragmentPresence::All);
    MonsterQuery.AddRequirement<FTransformFragment>(EMassFragmentAccess::ReadOnly);
    MonsterQuery.AddRequirement<FBBBMonsterHealthFragment>(EMassFragmentAccess::ReadOnly);
    MonsterQuery.AddRequirement<FBBBMonsterSpawnOwnershipFragment>(EMassFragmentAccess::ReadOnly);
}

int32 UBBBMonsterDirectorScheduleProcessor::CalculateEventRequest(const FBBBMonsterDirectorEventWaveDefinition& Wave,
    const int32 Spawned, const int32 EventAlive, const int32 WorldSlots)
{
    const int32 EventSlots = FMath::Max(0, Wave.MaxAlive - EventAlive);
    const int32 Remaining = Wave.TotalCount > 0 ? FMath::Max(0, Wave.TotalCount - Spawned) : Wave.BatchCount;
    const int32 Desired = FMath::Min(Wave.BatchCount, Remaining);
    return FMath::Min(512, FMath::Max(0, FMath::Min3(Desired, EventSlots, WorldSlots)));
}

void UBBBMonsterDirectorScheduleProcessor::Execute(FMassEntityManager&, FMassExecutionContext& Context)
{
    UWorld* World = Context.GetWorld();
    const double Now = World->GetTimeSeconds();
    TArray<FPlayerObserver> Players;
    CollectPlayers(*World, Players);
    const int32 PlayerCount = Players.Num();

    int32 Reserved = 0;
    int32 WorldLimit = MAX_int32;
    TMap<FGuid, float> DespawnDistances;
    DirectorQuery.ForEachEntityChunk(Context, [&Reserved, &WorldLimit, &DespawnDistances](FMassExecutionContext& Chunk)
    {
        const auto States = Chunk.GetFragmentView<FBBBMonsterDirectorFragment>();
        const auto Spawns = Chunk.GetFragmentView<FBBBMonsterDirectorSpawnFragment>();
        for (int32 Index = 0; Index < Chunk.GetNumEntities(); ++Index)
        {
            Reserved += FMath::Max(0, Spawns[Index].Reserved - Spawns[Index].Accounted);
            if (const auto* Definition = States[Index].Definition.Get(); Definition && States[Index].bRunning)
            {
                WorldLimit = FMath::Min(WorldLimit, Definition->WorldMaxAlive);
                if (Definition->DespawnDistance > 0.0f && States[Index].InstanceId.IsValid())
                {
                    DespawnDistances.Add(States[Index].InstanceId, Definition->DespawnDistance);
                }
            }
        }
    });

    TMap<FGuid, int32> DirectorAlive;
    TMap<TPair<FGuid, FName>, int32> EventAlive;
    int32 WorldAlive = 0;
    FCollisionQueryParams VisibilityParams(SCENE_QUERY_STAT(BBBMonsterDirectorDespawn), false);
    MonsterQuery.ForEachEntityChunk(Context, [World, &Players, &DespawnDistances, &DirectorAlive, &EventAlive, &WorldAlive, &VisibilityParams](FMassExecutionContext& Chunk)
    {
        const auto Transforms = Chunk.GetFragmentView<FTransformFragment>();
        const auto Health = Chunk.GetFragmentView<FBBBMonsterHealthFragment>();
        const auto Ownership = Chunk.GetFragmentView<FBBBMonsterSpawnOwnershipFragment>();
        for (int32 Index = 0; Index < Chunk.GetNumEntities(); ++Index)
        {
            if (Health[Index].CurrentHealth <= 0.0f)
            {
                continue;
            }
            const FGuid OwnerId = Ownership[Index].DirectorId;
            if (!Players.IsEmpty() && OwnerId.IsValid())
            {
                if (const float* DespawnDist = DespawnDistances.Find(OwnerId); DespawnDist && *DespawnDist > 0.0f)
                {
                    const FVector Location = Transforms[Index].GetTransform().GetLocation();
                    const double DespawnDistSq = FMath::Square(static_cast<double>(*DespawnDist));
                    bool bTooFarFromAll = true;
                    bool bVisibleToAny = false;
                    for (const auto& Player : Players)
                    {
                        if (FVector::DistSquared(Location, Player.Location) <= DespawnDistSq)
                        {
                            bTooFarFromAll = false;
                            break;
                        }
                        if (FVector::DotProduct((Location - Player.ViewLocation).GetSafeNormal(), Player.ViewDirection) >= 0.45f)
                        {
                            FHitResult Hit;
                            if (!World->LineTraceSingleByChannel(Hit, Player.ViewLocation, Location, ECC_Visibility, VisibilityParams))
                            {
                                bVisibleToAny = true;
                                break;
                            }
                        }
                    }
                    if (bTooFarFromAll && !bVisibleToAny)
                    {
                        Chunk.Defer().DestroyEntity(Chunk.GetEntity(Index));
                        continue;
                    }
                }
            }
            ++WorldAlive;
            if (OwnerId.IsValid())
            {
                ++DirectorAlive.FindOrAdd(OwnerId);
                if (Ownership[Index].Source == EBBBMonsterSpawnSource::EventWave && !Ownership[Index].EventId.IsNone())
                {
                    ++EventAlive.FindOrAdd(TPair<FGuid, FName>(OwnerId, Ownership[Index].EventId));
                }
            }
        }
    });

    DirectorQuery.ForEachEntityChunk(Context, [World, Now, PlayerCount, &Players, &DirectorAlive, &EventAlive, &Reserved, WorldAlive, WorldLimit](FMassExecutionContext& Chunk)
    {
        auto States = Chunk.GetMutableFragmentView<FBBBMonsterDirectorFragment>();
        auto Spawns = Chunk.GetMutableFragmentView<FBBBMonsterDirectorSpawnFragment>();
        auto Jobs = Chunk.GetMutableFragmentView<FBBBMonsterSpawnFragment>();
        for (int32 Index = 0; Index < Chunk.GetNumEntities(); ++Index)
        {
            auto& State = States[Index];
            auto& Spawn = Spawns[Index];
            auto& Job = Jobs[Index];
            State.Alive = DirectorAlive.FindRef(State.InstanceId);

            const auto* Definition = State.Definition.Get();
            if (!Definition || !State.bRunning)
            {
                continue;
            }
            if (Spawn.Reserved > 0 && !Job.bCreating && Now - Spawn.StartedAt >= Definition->QueryTimeout)
            {
                Reserved -= FMath::Max(0, Spawn.Reserved - Spawn.Accounted);
                Spawn.Reserved = 0;
                ++Spawn.RequestId;
                Job.bActive = false;
                State.Reason = TEXT("出生查询超时 已释放预留");
            }
            if (Now < State.NextUpdateTime)
            {
                continue;
            }
            State.NextUpdateTime = Now + Definition->UpdateInterval;

            const double Scale = (Definition->AdditionalPlayerScale > 0.0f && PlayerCount > 1)
                ? (1.0 + static_cast<double>(Definition->AdditionalPlayerScale) * (PlayerCount - 1)) : 1.0;

            // 1. 固有僵尸通道：玩家靠近未激活的固有区时一次性入队
            for (const auto& Region : State.Regions)
            {
                if (Region.Role != EBBBMonsterSpawnRegionRole::Population || State.ActivatedPopulationRegionIds.Contains(Region.Id))
                {
                    continue;
                }
                UBBBMonsterSpawnDefinition* PopDef = Region.SpawnDefinitionOverride.IsValid()
                    ? Region.SpawnDefinitionOverride.Get() : Definition->PopulationSpawnDefinition.Get();
                if (!PopDef)
                {
                    continue;
                }
                const double Radius = Region.ActivationRadius > 0.0f ? Region.ActivationRadius : Definition->PopulationActivationRadius;
                bool bTrigger = Players.IsEmpty() && PopDef->bAllowWithoutObservers;
                for (const auto& Player : Players)
                {
                    if (Region.DistanceToPoint(Player.Location) <= Radius)
                    {
                        bTrigger = true;
                        break;
                    }
                }
                if (!bTrigger)
                {
                    continue;
                }
                State.ActivatedPopulationRegionIds.Add(Region.Id);
                FBBBMonsterDirectorQueuedRequest Req;
                Req.Source = EBBBMonsterSpawnSource::Population;
                Req.Definition = PopDef;
                Req.Regions.Add(Region);
                Req.Count = FMath::Clamp(Region.PopulationCount, 1, 512);
                Req.bAlertOnSpawn = false;
                State.Queue.Add(MoveTemp(Req));
            }

            // 2. 预制事件波次通道
            for (int32 EventIdx = State.ActiveEvents.Num() - 1; EventIdx >= 0; --EventIdx)
            {
                auto& Event = State.ActiveEvents[EventIdx];
                const auto* WaveDef = Definition->FindEventWave(Event.EventId);
                if (!WaveDef || !WaveDef->SpawnDefinition)
                {
                    State.ActiveEvents.RemoveAt(EventIdx);
                    continue;
                }
                auto ScaledWave = *WaveDef;
                if (Scale > 1.0)
                {
                    ScaledWave.MaxAlive = FMath::RoundToInt32(FMath::Min(Scale * ScaledWave.MaxAlive, static_cast<double>(WorldLimit)));
                    ScaledWave.BatchCount = FMath::RoundToInt32(FMath::Min(Scale * ScaledWave.BatchCount, static_cast<double>(ScaledWave.MaxAlive)));
                    if (ScaledWave.TotalCount > 0)
                    {
                        ScaledWave.TotalCount = FMath::RoundToInt32(Scale * ScaledWave.TotalCount);
                    }
                }
                if (ScaledWave.TotalCount > 0 && Event.Spawned >= ScaledWave.TotalCount)
                {
                    State.ActiveEvents.RemoveAt(EventIdx);
                    continue;
                }
                const bool bInFlight = (Spawn.Reserved > 0 && Spawn.Source == EBBBMonsterSpawnSource::EventWave && Spawn.EventId == Event.EventId)
                    || State.Queue.ContainsByPredicate([Id = Event.EventId](const FBBBMonsterDirectorQueuedRequest& Item)
                    {
                        return Item.Source == EBBBMonsterSpawnSource::EventWave && Item.EventId == Id;
                    });
                if (bInFlight || Now < Event.NextSpawnTime)
                {
                    continue;
                }
                const int32 CurrentEventAlive = EventAlive.FindRef(TPair<FGuid, FName>(State.InstanceId, Event.EventId));
                const int32 WorldSlots = FMath::Max(0, WorldLimit - WorldAlive - Reserved);
                const int32 RequestCount = CalculateEventRequest(ScaledWave, Event.Spawned, CurrentEventAlive, WorldSlots);
                if (RequestCount <= 0)
                {
                    Event.NextSpawnTime = Now + Definition->RetryInterval;
                    continue;
                }
                TArray<FBBBMonsterSpawnRegionDefinition> TargetRegions;
                if (!WaveDef->RegionIds.IsEmpty())
                {
                    for (const auto& Region : State.Regions)
                    {
                        if (WaveDef->RegionIds.Contains(Region.Id))
                        {
                            TargetRegions.Add(Region);
                        }
                    }
                }
                else if (!Players.IsEmpty())
                {
                    const float Radius = WaveDef->SpawnDefinition->MaxPlayerDistance > 0.0f
                        ? WaveDef->SpawnDefinition->MaxPlayerDistance : Definition->DynamicHordeSampleRadius;
                    BuildPlayerRingRegions(Players, Radius, Definition->DynamicHordeSampleHalfHeight, TargetRegions);
                }
                if (TargetRegions.IsEmpty())
                {
                    Event.NextSpawnTime = Now + Definition->RetryInterval;
                    continue;
                }
                FBBBMonsterDirectorQueuedRequest Req;
                Req.Source = EBBBMonsterSpawnSource::EventWave;
                Req.EventId = Event.EventId;
                Req.Definition = WaveDef->SpawnDefinition;
                Req.Regions = MoveTemp(TargetRegions);
                Req.Count = RequestCount;
                Req.InitialTargetPlayerId = Event.InitialTargetPlayerId;
                Req.bAlertOnSpawn = true;
                State.Queue.Add(MoveTemp(Req));
                Event.NextSpawnTime = Now + WaveDef->Interval;
            }

            // 3. 动态波次通道
            if (Definition->bEnableDynamicHorde && Definition->DynamicHordeSpawnDefinition && !State.bDynamicHordePaused)
            {
                const bool bPausedByEvent = Definition->bPauseDynamicHordeDuringEvent && !State.ActiveEvents.IsEmpty();
                if (bPausedByEvent)
                {
                    if (State.NextDynamicHordeTime > 0.0)
                    {
                        State.NextDynamicHordeTime += Definition->UpdateInterval;
                    }
                }
                else if (State.NextDynamicHordeTime <= 0.0)
                {
                    FRandomStream Random(UBBBMonsterSpawnProcessor::RandomBits(State.Seed, ++State.DynamicHordeCounter, 0x511u));
                    State.NextDynamicHordeTime = Now + Random.FRandRange(Definition->DynamicHordeMinInterval, Definition->DynamicHordeMaxInterval);
                }
                else if (Now >= State.NextDynamicHordeTime && !Players.IsEmpty())
                {
                    const bool bAlreadyQueued = State.Queue.ContainsByPredicate([](const FBBBMonsterDirectorQueuedRequest& Item)
                    {
                        return Item.Source == EBBBMonsterSpawnSource::DynamicHorde;
                    });
                    if (!bAlreadyQueued)
                    {
                        FBBBMonsterDirectorQueuedRequest Req;
                        Req.Source = EBBBMonsterSpawnSource::DynamicHorde;
                        Req.Definition = Definition->DynamicHordeSpawnDefinition;
                        BuildPlayerRingRegions(Players, Definition->DynamicHordeSampleRadius, Definition->DynamicHordeSampleHalfHeight, Req.Regions);
                        Req.Count = FMath::Clamp(FMath::RoundToInt32(Scale * Definition->DynamicHordeBatchCount), 1, 512);
                        Req.bAlertOnSpawn = true;
                        State.Queue.Add(MoveTemp(Req));
                    }
                    FRandomStream Random(UBBBMonsterSpawnProcessor::RandomBits(State.Seed, ++State.DynamicHordeCounter, 0x511u));
                    State.NextDynamicHordeTime = Now + Random.FRandRange(Definition->DynamicHordeMinInterval, Definition->DynamicHordeMaxInterval);
                }
            }

            // 4. 从内部短队列按优先级消费下一条刷怪请求
            if (Spawn.Reserved > 0 || Job.bCreating || State.Queue.IsEmpty())
            {
                continue;
            }
            int32 BestQueueIndex = 0;
            for (int32 QueueIndex = 1; QueueIndex < State.Queue.Num(); ++QueueIndex)
            {
                if (RequestPriority(State.Queue[QueueIndex].Source) > RequestPriority(State.Queue[BestQueueIndex].Source))
                {
                    BestQueueIndex = QueueIndex;
                }
            }
            const int32 WorldSlots = FMath::Max(0, WorldLimit - WorldAlive - Reserved);
            if (WorldSlots <= 0)
            {
                State.Reason = TEXT("等待世界存活额度");
                continue;
            }
            FBBBMonsterDirectorQueuedRequest NextReq = MoveTemp(State.Queue[BestQueueIndex]);
            State.Queue.RemoveAt(BestQueueIndex);
            const auto* SpawnDef = NextReq.Definition.Get();
            const int32 FinalCount = FMath::Min(NextReq.Count, WorldSlots);
            if (!SpawnDef || FinalCount <= 0 || NextReq.Regions.IsEmpty())
            {
                continue;
            }
            ++Spawn.RequestId;
            Spawn.Source = NextReq.Source;
            Spawn.EventId = NextReq.EventId;
            Spawn.Reserved = FinalCount;
            Spawn.StartedAt = Now;
            Spawn.Accounted = 0;
            FBBBMonsterSpawnRequestLocalControlPacket Packet;
            Packet.OwnerId = State.InstanceId;
            Packet.Generation = State.Generation;
            Packet.Source = NextReq.Source;
            Packet.EventId = NextReq.EventId;
            Packet.InitialTargetPlayerId = NextReq.InitialTargetPlayerId;
            Packet.bAlertOnSpawn = NextReq.bAlertOnSpawn;
            Packet.RequestId = Spawn.RequestId;
            Packet.Definition = NextReq.Definition;
            const uint32 RootSeed = SpawnDef->Seed != 0 ? static_cast<uint32>(SpawnDef->Seed) : State.Seed;
            Packet.Seed = UBBBMonsterSpawnProcessor::RandomBits(RootSeed, Spawn.RequestId, static_cast<uint32>(NextReq.Source));
            Packet.Count = FinalCount;
            Packet.Regions = MoveTemp(NextReq.Regions);
            Packet.ExcludeRegions = State.ExcludeRegions;
            World->GetSubsystem<UBBBMassSubsystem>()->SubmitInput(Chunk.GetEntity(Index), MoveTemp(Packet));
            Reserved += FinalCount;
            State.Reason = TEXT("已提交刷怪请求");
        }
    });
}
