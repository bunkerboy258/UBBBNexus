#include "BBBWork/UBBBNexus/Mass/Network/BBBMassNetworkActor.h"

#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/PlayerState.h"
#include "GameFramework/GameStateBase.h"
#include "MassSpawnerSubsystem.h"
#include "Net/UnrealNetwork.h"
#include "BBBWork/UBBBNexus/Mass/Core/BBBMassSubsystem.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Network/BBBMonsterStreamChannel.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Input/RemoteMessage/Health/FBBBMonsterDamageTotalsRemoteMessagePacket.h"
#include "Engine/NetConnection.h"
#include "Engine/NetDriver.h"
#include "EngineUtils.h"
#include "ProfilingDebugging/CountersTrace.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Config/BBBMonsterDefinition.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Input/AuthorityFact/Network/FBBBMonsterStateAuthorityFactPacket.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Slime/Input/AuthorityFact/Network/FBBBSlimeStateAuthorityFactPacket.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Slime/Presentation/BBBSlimePresentationActor.h"

TRACE_DECLARE_INT_COUNTER(BBBMonsterReliablePayloadBytes, TEXT("BBBMonster/MaxReliablePayloadBytes"));
TRACE_DECLARE_INT_COUNTER(BBBMonsterMotionPayloadBytes, TEXT("BBBMonster/MaxMotionPayloadBytes"));
TRACE_DECLARE_INT_COUNTER(BBBMonsterStreamPeers, TEXT("BBBMonster/StreamPeers"));

ABBBMassNetworkActor::ABBBMassNetworkActor()
{
    PrimaryActorTick.bCanEverTick = false;
    bReplicates = true;
    bAlwaysRelevant = true;
    SetNetUpdateFrequency(10.0f);
}

void ABBBMassNetworkActor::BeginPlay()
{
    Super::BeginPlay();
    ReceiveTickHandle = FWorldDelegates::OnWorldPreActorTick.AddUObject(this, &ABBBMassNetworkActor::TickStreamReceive);
}

void ABBBMassNetworkActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(ABBBMassNetworkActor, Slimes);
}

void ABBBMassNetworkActor::EndPlay(const EEndPlayReason::Type Reason)
{
    FWorldDelegates::OnWorldPreActorTick.Remove(ReceiveTickHandle);
    // 回放跳转会销毁并重建动态复制载体 镜像实体必须跟随其唯一持有者一起回收
    if (!HasAuthority() && GetWorld() && !GetWorld()->bIsTearingDown)
    {
        UBBBMassSubsystem* Mass = GetWorld()->GetSubsystem<UBBBMassSubsystem>();
        UMassEntitySubsystem* Entities = GetWorld()->GetSubsystem<UMassEntitySubsystem>();
        if (Mass && Entities)
        {
            for (const auto& Pair : LocalEntities)
            {
                Mass->DestroyEntity(Pair.Value);
            }
            const auto& Manager = Entities->GetEntityManager();
            for (const auto& Pair : LocalSlimes)
            {
                if (Manager.IsEntityValid(Pair.Value))
                {
                    const auto* State = Manager.GetFragmentDataPtr<FBBBSlimeFragment>(Pair.Value);
                    if (State && State->Presentation.IsValid())
                    {
                        State->Presentation->Destroy();
                    }
                    Mass->DestroyEntity(Pair.Value);
                }
            }
        }
    }
    LocalEntities.Reset();
    LocalSlimes.Reset();
    Super::EndPlay(Reason);
}

void ABBBMassNetworkActor::PublishSlimes(const TArray<FBBBSlimeFact>& States)
{
    if (HasAuthority() && ensureMsgf(States.Num() <= 128, TEXT("粘液当前快照超过世界上限")))
    {
        Slimes = States;
    }
}

void ABBBMassNetworkActor::OnRep_Slimes()
{
    auto* Mass = GetWorld()->GetSubsystem<UBBBMassSubsystem>();
    auto* Entities = GetWorld()->GetSubsystem<UMassEntitySubsystem>();
    if (!Mass || !Entities)
    {
        return;
    }
    auto& Manager = Entities->GetMutableEntityManager();
    TSet<FGuid> Present;
    const auto* GameState = GetWorld()->GetGameState();
    const double ServerTime = GameState ? GameState->GetServerWorldTimeSeconds() : GetWorld()->GetTimeSeconds();
    for (const auto& Fact : Slimes)
    {
        if (!Fact.IsValid())
        {
            continue;
        }
        Present.Add(Fact.InstanceId);
        auto& Entity = LocalSlimes.FindOrAdd(Fact.InstanceId);
        if (!Manager.IsEntityValid(Entity))
        {
            Entity = Mass->CreateEntity(*Fact.Definition->EntityConfig);
        }
        if (Entity.IsSet())
        {
            FBBBSlimeStateAuthorityFactPacket Packet;
            Packet.Fact = Fact;
            Packet.Fact.EndsAt = GetWorld()->GetTimeSeconds() + FMath::Max(0.0, Fact.EndsAt - ServerTime);
            Mass->SubmitInput(Entity, MoveTemp(Packet));
        }
    }
    for (auto It = LocalSlimes.CreateIterator(); It; ++It)
    {
        if (!Present.Contains(It.Key()))
        {
            if (Manager.IsEntityValid(It.Value()))
            {
                if (const auto* State = Manager.GetFragmentDataPtr<FBBBSlimeFragment>(It.Value()); State && State->Presentation.IsValid())
                {
                    State->Presentation->Destroy();
                }
                Mass->DestroyEntity(It.Value());
            }
            It.RemoveCurrent();
        }
    }
}


void ABBBMassNetworkActor::BeginPublish()
{
    Observed.Reset();
}

void ABBBMassNetworkActor::Publish(FBBBMonsterReplicationItem&& State, FMassEntityHandle Entity,
    const TMap<int64, FBBBMonsterDamageTotals>& Damage, bool bUpdateAction)
{
    const FGuid Id = State.InstanceId;
    Observed.Add(Id);
    LocalEntities.Add(Id, Entity);
    const int32* ExistingIndex = ReplicationIndices.Find(Id);
    if (!ExistingIndex)
    {
        ReplicationIndices.Add(Id, Monsters.AddDefaulted());
        ExistingIndex = ReplicationIndices.Find(Id);
        bUpdateAction = true;
    }

    auto& Existing = Monsters[*ExistingIndex];
    const bool bActionChanged = Existing.InstanceId != Id || Existing.Behavior != State.Behavior
        || Existing.Traversal.Serial != State.Traversal.Serial || Existing.Traversal.Kind != State.Traversal.Kind
        || Existing.Special.Serial != State.Special.Serial || Existing.Special.Phase != State.Special.Phase
        || Existing.ActionId != State.ActionId || Existing.StateEnteredTime != State.StateEnteredTime
        || Existing.CrawlStartedAt != State.CrawlStartedAt || FMemory::Memcmp(Existing.DismembermentPresets,
            State.DismembermentPresets, sizeof(State.DismembermentPresets)) != 0
        || (bUpdateAction && (!Existing.Location.Equals(State.Location, 0.5f)
            || !Existing.Rotation.Equals(State.Rotation, 0.25f) || !Existing.Velocity.Equals(State.Velocity, 1.0f)));
    const bool bDamageChanged = Damage.Num() != Existing.Contributions.Num()
        || Existing.Contributions.ContainsByPredicate([&Damage](const FBBBMonsterDamageTotals& Value)
        {
            const auto* Latest = Damage.Find(Value.WriterKey());
            return !Latest || !(*Latest == Value);
        });

    if (bDamageChanged)
    {
        auto& Versions = DamageRevisions.FindOrAdd(Id);
        TArray<FBBBMonsterDamageTotals> Values;
        Values.Reserve(Damage.Num());
        for (const auto& Pair : Damage)
        {
            Values.Add(Pair.Value);
        }
        Values.Sort([](const auto& L, const auto& R) { return L.WriterKey() < R.WriterKey(); });

        TArray<uint64> NewVersions;
        NewVersions.Reserve(Values.Num());
        int32 Old = 0;
        for (const auto& Value : Values)
        {
            while (Existing.Contributions.IsValidIndex(Old) && Existing.Contributions[Old].WriterKey() < Value.WriterKey())
            {
                ++Old;
            }
            const bool bSame = Existing.Contributions.IsValidIndex(Old) && Existing.Contributions[Old] == Value;
            NewVersions.Add(bSame && Versions.IsValidIndex(Old) ? Versions[Old] : ++LatestDamageRevision);
        }
        Existing.Contributions = MoveTemp(Values);
        Versions = MoveTemp(NewVersions);
    }

    if (bActionChanged)
    {
        State.Revision = Existing.Revision + 1;
        State.Contributions = MoveTemp(Existing.Contributions);
        Existing = MoveTemp(State);
    }
}

namespace
{
    using ERecord = FBBBMonsterStreamRecord::EKind;

    FBBBMonsterStreamRecord MakeActionRecord(const FBBBMonsterReplicationItem& State, ERecord Kind)
    {
        FBBBMonsterStreamRecord Record;
        Record.Kind = Kind;
        Record.Action.InstanceId = State.InstanceId;
        Record.Action.BirthSeed = State.BirthSeed;
        Record.Action.Revision = State.Revision;
        Record.Action.Traversal = State.Traversal;
        Record.Action.Special = State.Special;
        FMemory::Memcpy(Record.Action.DismembermentPresets, State.DismembermentPresets, sizeof(State.DismembermentPresets));
        Record.Action.Location = State.Location;
        Record.Action.Rotation = State.Rotation;
        Record.Action.Velocity = State.Velocity;
        Record.Action.Behavior = State.Behavior;
        Record.Action.ActionId = State.ActionId;
        Record.Action.StateEnteredTime = State.StateEnteredTime;
        Record.Action.CrawlStartedAt = State.CrawlStartedAt;
        if (Kind == ERecord::Birth && State.Definition)
        {
            Record.DefinitionPath = State.Definition->GetPathName();
        }
        return Record;
    }

    FBBBMonsterStreamRecord MakeDamageRecord(const FGuid& Id, const FBBBMonsterDamageTotals& Damage, bool bInitial)
    {
        FBBBMonsterStreamRecord Record;
        Record.Kind = bInitial ? ERecord::InitialDamage : ERecord::Damage;
        Record.Action.InstanceId = Id;
        Record.Damage = Damage;
        return Record;
    }

    ABBBMonsterStreamChannel* SpawnStream(UWorld& World, APlayerController* Controller, bool bReplay)
    {
        FActorSpawnParameters Params;
        Params.Owner = Controller;
        Params.bDeferConstruction = true;
        if (auto* Channel = World.SpawnActor<ABBBMonsterStreamChannel>(ABBBMonsterStreamChannel::StaticClass(),
            FTransform::Identity, Params))
        {
            if (bReplay)
            {
                Channel->SetNetDriverName(NAME_DemoNetDriver);
                Channel->bOnlyRelevantToOwner = false;
                Channel->bAlwaysRelevant = true;
                Channel->bRelevantForNetworkReplays = true;
            }
            Channel->FinishSpawning(FTransform::Identity);
            return Channel;
        }
        return nullptr;
    }
}

void ABBBMassNetworkActor::EndPublish()
{
    for (int32 Index = Monsters.Num() - 1; Index >= 0; --Index)
    {
        const FGuid Id = Monsters[Index].InstanceId;
        if (!Observed.Contains(Id))
        {
            LocalEntities.Remove(Id);
            ReplicationIndices.Remove(Id);
            DamageRevisions.Remove(Id);
            Monsters.RemoveAtSwap(Index, 1, EAllowShrinking::No);
            if (Monsters.IsValidIndex(Index))
            {
                ReplicationIndices.Add(Monsters[Index].InstanceId, Index);
            }
        }
    }

    UWorld& World = *GetWorld();
    const float Now = World.GetTimeSeconds();
    for (auto It = PeerChannels.CreateIterator(); It; ++It)
    {
        const auto* Controller = It.Key().Get();
        if (!Controller || !Controller->GetNetConnection() || Controller->GetNetConnection()->GetConnectionState() != USOCK_Open)
        {
            if (auto* Channel = It.Value().Get())
            {
                if (auto* Motion = Channel->MotionChannel.Get())
                {
                    Motion->Destroy();
                }
                Channel->Destroy();
            }
            It.RemoveCurrent();
        }
    }
    for (auto It = World.GetPlayerControllerIterator(); It; ++It)
    {
        auto* Controller = It->Get();
        auto* Connection = Controller ? Controller->GetNetConnection() : nullptr;
        if (!Connection || Connection->GetConnectionState() != USOCK_Open)
        {
            continue;
        }
        auto& Peer = PeerChannels.FindOrAdd(Controller);
        if (!Peer.IsValid())
        {
            Peer = SpawnStream(World, Controller, false);
            if (auto* Channel = Peer.Get())
            {
                Channel->SetControl();
                Channel->MotionChannel = SpawnStream(World, Controller, false);
            }
        }
        if (auto* Channel = Peer.Get(); Channel && Now >= Channel->NextSendTime && Connection->IsNetReady())
        {
            Channel->NextSendTime = Now + 0.066f;
            const APawn* Pawn = Controller->GetPawn();
            const FVector View = Pawn ? Pawn->GetActorLocation() : Controller->GetFocalLocation();
            SendPeerFrame(*Channel, View, Now);
        }
    }

    if (World.IsRecordingReplay() && Now >= NextReplayUpdateAt)
    {
        NextReplayUpdateAt = Now + 0.1f;
        PublishReplay();
    }
    if (!World.IsRecordingReplay() && !ReplayPages.IsEmpty())
    {
        for (const auto& Page : ReplayPages)
        {
            if (auto* Channel = Page.Get())
            {
                Channel->Destroy();
            }
        }
        ReplayPages.Reset();
    }
    TRACE_COUNTER_SET(BBBMonsterStreamPeers, PeerChannels.Num());
}

void ABBBMassNetworkActor::SendPeerFrame(ABBBMonsterStreamChannel& Channel, const FVector& View, float Now)
{
    TRACE_CPUPROFILER_EVENT_SCOPE(BBBMonsterSendPeerFrame);
    if (Channel.CanSendReliable())
    {
        FBBBMonsterStreamBatch Batch;
        TArray<FGuid, TInlineAllocator<32>> Forgotten;
        TArray<FGuid, TInlineAllocator<32>> Changed;
        TArray<FGuid, TInlineAllocator<32>> Born;
        for (const auto& Pair : Channel.Subscriptions)
        {
            if (!ReplicationIndices.Contains(Pair.Key))
            {
                FBBBMonsterStreamRecord Record;
                Record.Kind = ERecord::Forget;
                Record.Action.InstanceId = Pair.Key;
                if (!Batch.TryAdd(MoveTemp(Record), FBBBMonsterStreamBatch::ReliableBytes))
                {
                    break;
                }
                Forgotten.Add(Pair.Key);
            }
        }

        TArray<int32, TInlineAllocator<256>> Work;
        for (int32 Offset = 0; Offset < Monsters.Num(); ++Offset)
        {
            const int32 Index = (Channel.ReliableCursor + Offset) % Monsters.Num();
            const auto& State = Monsters[Index];
            const auto* Subscription = Channel.Subscriptions.Find(State.InstanceId);
            const auto* Versions = DamageRevisions.Find(State.InstanceId);
            if (!Subscription || (State.Behavior == EBBBMonsterBehavior::Dead && !Subscription->bSentDeathPose)
                || (Versions && Versions->ContainsByPredicate([Subscription](uint64 Version)
                { return Version > Subscription->DamageRevision; })))
            {
                Work.Add(Index);
            }
        }
        const auto Priority = [&](int32 Index)
        {
            const auto& State = Monsters[Index];
            const auto* Subscription = Channel.Subscriptions.Find(State.InstanceId);
            if (Subscription && State.Behavior == EBBBMonsterBehavior::Dead)
            {
                return 0;
            }
            const double Distance = FVector::DistSquared(View, State.Location);
            if (Distance <= FMath::Square(2500.0))
            {
                return 1;
            }
            return Distance <= FMath::Square(10000.0) ? 2 : 3;
        };
        const bool bPrioritized = (++Channel.ReliableFrames % 4) != 0;
        if (bPrioritized)
        {
            Work.StableSort([&Priority](int32 L, int32 R) { return Priority(L) < Priority(R); });
        }

        // 轮转起点为各优先档提供确定的遍历顺序
        int32 Processed = 0;
        for (const int32 Index : Work)
        {
            const auto& State = Monsters[Index];
            const FGuid Id = State.InstanceId;
            auto* Subscription = Channel.Subscriptions.Find(Id);
            if (!Subscription)
            {
                if (!Batch.TryAdd(MakeActionRecord(State, ERecord::Birth), FBBBMonsterStreamBatch::ReliableBytes))
                {
                    break;
                }
                Subscription = &Channel.Subscriptions.Add(Id);
                Subscription->InitialDamageThrough = LatestDamageRevision;
                Born.Add(Id);
            }

            if (const auto* Versions = DamageRevisions.Find(Id))
            {
                TArray<int32, TInlineAllocator<16>> Dirty;
                for (int32 Part = 0; Part < Versions->Num(); ++Part)
                {
                    if ((*Versions)[Part] > Subscription->DamageRevision)
                    {
                        Dirty.Add(Part);
                    }
                }
                Dirty.Sort([Versions](int32 L, int32 R) { return (*Versions)[L] < (*Versions)[R]; });
                bool bBatchFull = false;
                for (const int32 Part : Dirty)
                {
                    const uint64 Revision = (*Versions)[Part];
                    const bool bInitial = Revision <= Subscription->InitialDamageThrough;
                    if (!Batch.TryAdd(MakeDamageRecord(Id, State.Contributions[Part], bInitial),
                        FBBBMonsterStreamBatch::ReliableBytes))
                    {
                        bBatchFull = true;
                        break;
                    }
                    Subscription->DamageRevision = Revision;
                    Changed.AddUnique(Id);
                }
                if (bBatchFull)
                {
                    break;
                }
            }
            if (State.Behavior == EBBBMonsterBehavior::Dead && !Subscription->bSentDeathPose)
            {
                if (!Batch.TryAdd(MakeActionRecord(State, ERecord::Motion), FBBBMonsterStreamBatch::ReliableBytes))
                {
                    break;
                }
                Subscription->bSentDeathPose = true;
                Changed.AddUnique(Id);
            }
            ++Processed;
        }
        if (!Monsters.IsEmpty() && Processed > 0)
        {
            Channel.ReliableCursor = (Channel.ReliableCursor + Processed) % Monsters.Num();
        }
        if (!Batch.Records.IsEmpty())
        {
            MaxReliablePayloadBytes = FMath::Max(MaxReliablePayloadBytes, (Batch.PayloadBits + 7) / 8);
            TRACE_COUNTER_SET(BBBMonsterReliablePayloadBytes, MaxReliablePayloadBytes);
            const uint32 Sequence = Channel.SendReliable(Batch);
            for (const auto& Id : Born)
            {
                auto& Subscription = Channel.Subscriptions.FindChecked(Id);
                Subscription.BirthSequence = Sequence;
                Subscription.DeliverySequence = Sequence;
            }
            for (const auto& Id : Changed)
            {
                Channel.Subscriptions.FindChecked(Id).DeliverySequence = Sequence;
            }
            for (const auto& Id : Forgotten)
            {
                Channel.Subscriptions.Remove(Id);
            }
        }
    }

    auto* Motion = Channel.MotionChannel.Get();
    if (!Motion || Monsters.IsEmpty())
    {
        return;
    }
    for (int32 Page = 0; Page < 2; ++Page)
    {
        FBBBMonsterStreamBatch Batch;
        for (int32 Count = 0; Count < Monsters.Num(); ++Count)
        {
            const int32 Index = Channel.MotionCursor % Monsters.Num();
            const auto& State = Monsters[Index];
            auto* Subscription = Channel.Subscriptions.Find(State.InstanceId);
            if (Subscription && Channel.IsDelivered(Subscription->BirthSequence))
            {
                const double Distance = FVector::DistSquared(View, State.Location);
                const bool bHigh = Distance < FMath::Square(2500.0)
                    || (Distance < FMath::Square(6000.0) && State.Traversal.Kind != 0);
                const float Interval = bHigh ? 0.066f : Distance < FMath::Square(8000.0) ? 0.18f : 0.5f;
                const bool bChanged = State.Revision != Subscription->MotionRevision;
                if (Now < Subscription->NextMotionAt && (!bChanged || Now - Subscription->LastMotionAt < Interval))
                {
                    Channel.MotionCursor = (Index + 1) % Monsters.Num();
                    continue;
                }
                if (!Batch.TryAdd(MakeActionRecord(State, ERecord::Motion), FBBBMonsterStreamBatch::MotionBytes))
                {
                    break;
                }
                Subscription->LastMotionAt = Now;
                Subscription->MotionRevision = State.Revision;
                Subscription->NextMotionAt = Now + (bChanged ? FMath::Max(Interval, 0.20f) : bHigh ? 0.50f : 1.0f);
            }
            Channel.MotionCursor = (Index + 1) % Monsters.Num();
        }
        if (Batch.Records.IsEmpty())
        {
            break;
        }
        MaxMotionPayloadBytes = FMath::Max(MaxMotionPayloadBytes, (Batch.PayloadBits + 7) / 8);
        TRACE_COUNTER_SET(BBBMonsterMotionPayloadBytes, MaxMotionPayloadBytes);
        Motion->ClientReceiveMotion(Batch);
    }
}

bool ABBBMassNetworkActor::CanRetireMonster(const FGuid& Id) const
{
    const auto* Versions = DamageRevisions.Find(Id);
    for (const auto& Pair : PeerChannels)
    {
        if (const auto* Channel = Pair.Value.Get())
        {
            const auto* Subscription = Channel->Subscriptions.Find(Id);
            if (Subscription && (!Subscription->bSentDeathPose || !Channel->IsDelivered(Subscription->DeliverySequence)
                || (Versions && Versions->ContainsByPredicate([Subscription](uint64 Version)
                    { return Version > Subscription->DamageRevision; }))))
            {
                return false;
            }
        }
    }
    return true;
}

FMassEntityHandle ABBBMassNetworkActor::FindEntity(const FGuid& InstanceId) const
{
    const auto* Entity = LocalEntities.Find(InstanceId);
    return Entity != nullptr ? *Entity : FMassEntityHandle();
}

bool ABBBMassNetworkActor::ReportLocalDamage(const FGuid& InstanceId, const FBBBMonsterDamageTotals& Contribution)
{
    const auto* Submitted = SubmittedDamage.Find(InstanceId);
    if ((Contribution.Parts.Sum() <= 0.0 && Contribution.LastHitResult.Sequence == 0)
        || (Submitted && !Contribution.Parts.Exceeds(Submitted->Parts)
            && !Contribution.Destruction.Exceeds(Submitted->Destruction)
            && Contribution.LastHitResult.Sequence <= Submitted->LastHitResult.Sequence))
    {
        return true;
    }
    if (Contribution.IsValid())
    {
        PendingDamageReports.Add(InstanceId, Contribution);
    }
    return false;
}

void ABBBMassNetworkActor::TickStreamReceive(UWorld* World, ELevelTick, float)
{
    if (World != GetWorld())
    {
        return;
    }
    if (World->IsPlayingReplay())
    {
        ReceiveReplay();
        return;
    }
    if (HasAuthority())
    {
        return;
    }
    if (!LocalControl.IsValid())
    {
        for (TActorIterator<ABBBMonsterStreamChannel> It(World); It; ++It)
        {
            if (It->IsControl() && It->GetOwner() == World->GetFirstPlayerController())
            {
                LocalControl = *It;
                break;
            }
        }
    }
    if (auto* Channel = LocalControl.Get())
    {
        Channel->PumpReceive(*this);
    }
}

void ABBBMassNetworkActor::FlushPendingDamageReports()
{
    auto* Channel = LocalControl.Get();
    if (!Channel || !Channel->CanUpload())
    {
        return;
    }
    UWorld& World = *GetWorld();
    const float Now = World.GetTimeSeconds();
    if (PendingDamageReports.IsEmpty() || Now < NextUploadAt)
    {
        return;
    }
    const auto* Driver = World.GetNetDriver();
    const auto* Connection = Driver ? Driver->ServerConnection.Get() : nullptr;
    if (Connection && !Connection->IsNetReady())
    {
        return;
    }

    FBBBMonsterStreamBatch Batch;
    for (auto It = PendingDamageReports.CreateIterator(); It; ++It)
    {
        if (!Batch.TryAdd(MakeDamageRecord(It.Key(), It.Value(), false), FBBBMonsterStreamBatch::ReliableBytes))
        {
            break;
        }
        SubmittedDamage.Add(It.Key(), It.Value());
        It.RemoveCurrent();
    }
    if (!Batch.Records.IsEmpty())
    {
        NextUploadAt = Now + 0.066f;
        Channel->SendUpload(Batch);
    }
}

void ABBBMassNetworkActor::ReceivePlayerDamage(const FGuid& InstanceId, const FBBBMonsterDamageTotals& Value)
{
    if (const auto Entity = FindEntity(InstanceId); Entity.IsSet() && Value.IsValid())
    {
        FBBBMonsterDamageTotalsRemoteMessagePacket Packet;
        Packet.InstanceId = InstanceId;
        Packet.Include(Value);
        GetWorld()->GetSubsystem<UBBBMassSubsystem>()->SubmitInput(Entity, MoveTemp(Packet));
    }
}

bool ABBBMassNetworkActor::ReceiveMonsterRecord(const FBBBMonsterStreamRecord& Record)
{
    const auto& Item = Record.Action;
    const FGuid Id = Item.InstanceId;
    if (!Id.IsValid())
    {
        return false;
    }
    if (Record.Kind == ERecord::Forget)
    {
        ForgetMonster(Id);
        return true;
    }

    auto* Mass = GetWorld()->GetSubsystem<UBBBMassSubsystem>();
    if (!Mass)
    {
        return false;
    }
    FMassEntityHandle Entity = FindEntity(Id);
    if (Record.Kind == ERecord::Damage || Record.Kind == ERecord::InitialDamage)
    {
        if (!Entity.IsSet())
        {
            return false;
        }
        FBBBMonsterDamageTotalsRemoteMessagePacket Packet;
        Packet.InstanceId = Id;
        Packet.Include(Record.Damage, Record.Kind == ERecord::InitialDamage);
        Mass->SubmitInput(Entity, MoveTemp(Packet));
        if (const auto* Index = ReplicationIndices.Find(Id))
        {
            auto& Values = Monsters[*Index].Contributions;
            if (auto* Value = Values.FindByPredicate([&Record](const auto& A) { return A.WriterKey() == Record.Damage.WriterKey(); }))
            {
                Value->Merge(Record.Damage);
            }
            else
            {
                Values.Add(Record.Damage);
            }
        }
        return true;
    }

    const auto* Last = ReceivedRevisions.Find(Id);
    if (Last && int32(Item.Revision - *Last) <= 0)
    {
        return true;
    }
    if (!Entity.IsSet())
    {
        if (Record.Kind != ERecord::Birth)
        {
            return true;
        }
        const FSoftObjectPath Path(Record.DefinitionPath);
        const auto* Definition = Cast<UBBBMonsterDefinition>(Path.TryLoad());
        if (!Definition || !Definition->EntityConfig)
        {
            UE_LOG(LogTemp, Error, TEXT("小怪初始快照缺少配置 %s"), *Record.DefinitionPath);
            return false;
        }
        Entity = Mass->CreateEntity(*Definition->EntityConfig);
        if (!Entity.IsSet())
        {
            return false;
        }
        LocalEntities.Add(Id, Entity);
        ReplicationIndices.Add(Id, Monsters.Add(Item));
        Monsters.Last().Definition = const_cast<UBBBMonsterDefinition*>(Definition);
    }

    auto& Current = Monsters[ReplicationIndices.FindChecked(Id)];
    auto* Definition = Current.Definition.Get();
    const uint32 BirthSeed = Current.BirthSeed;
    auto Contributions = MoveTemp(Current.Contributions);
    Current = Item;
    Current.Definition = Definition;
    Current.BirthSeed = BirthSeed;
    Current.Contributions = MoveTemp(Contributions);
    ReceivedRevisions.Add(Id, Item.Revision);

    FBBBMonsterStateAuthorityFactPacket Packet;
    Packet.InstanceId = Id;
    Packet.BirthSeed = BirthSeed;
    FMemory::Memcpy(Packet.DismembermentPresets, Item.DismembermentPresets, sizeof(Item.DismembermentPresets));
    Packet.Revision = Item.Revision;
    Packet.Transform = FTransform(Item.Rotation, Item.Location);
    Packet.Velocity = Item.Velocity;
    Packet.Behavior = Item.Behavior;
    Packet.ActionId = Item.ActionId;
    const AGameStateBase* GameState = GetWorld()->GetGameState();
    const double LocalTime = GetWorld()->GetTimeSeconds();
    const double ServerTime = GameState ? GameState->GetServerWorldTimeSeconds() : LocalTime;
    Packet.StateEnteredTime = LocalTime - FMath::Max(0.0, ServerTime - Item.StateEnteredTime);
    Packet.bHasCrawlTime = Item.CrawlStartedAt >= 0.0f;
    Packet.CrawlStartedAt = Packet.bHasCrawlTime ? LocalTime - FMath::Max(0.0, ServerTime - Item.CrawlStartedAt) : -1.0f;
    Packet.Traversal = Item.Traversal;
    Packet.Special = Item.Special;
    Packet.Special.StartedAt = LocalTime - FMath::Max(0.0, ServerTime - Item.Special.StartedAt);
    if (Packet.Traversal.Kind != 0)
    {
        Packet.Traversal.StartedAt = LocalTime - FMath::Max(0.0, ServerTime - Item.Traversal.StartedAt);
    }
    Mass->SubmitInput(Entity, MoveTemp(Packet));
    return true;
}

void ABBBMassNetworkActor::ForgetMonster(const FGuid& Id)
{
    FMassEntityHandle Entity;
    if (LocalEntities.RemoveAndCopyValue(Id, Entity))
    {
        GetWorld()->GetSubsystem<UBBBMassSubsystem>()->DestroyEntity(Entity);
    }
    ReceivedRevisions.Remove(Id);
    SubmittedDamage.Remove(Id);
    PendingDamageReports.Remove(Id);
    int32 Index = INDEX_NONE;
    if (ReplicationIndices.RemoveAndCopyValue(Id, Index))
    {
        Monsters.RemoveAtSwap(Index, 1, EAllowShrinking::No);
        if (Monsters.IsValidIndex(Index))
        {
            ReplicationIndices.Add(Monsters[Index].InstanceId, Index);
        }
    }
}

void ABBBMassNetworkActor::PublishReplay()
{
    int32 Page = 0;
    FBBBMonsterStreamBatch Batch;
    const auto Flush = [&]()
    {
        if (Batch.Records.IsEmpty())
        {
            return;
        }
        if (!ReplayPages.IsValidIndex(Page))
        {
            ReplayPages.Add(SpawnStream(*GetWorld(), nullptr, true));
        }
        if (auto* Channel = ReplayPages[Page].Get())
        {
            Batch.Sequence = ++ReplayRevision;
            Channel->PublishReplay(MoveTemp(Batch));
        }
        ++Page;
        Batch = {};
    };
    const auto Add = [&](const FBBBMonsterStreamRecord& Record)
    {
        if (!Batch.TryAdd(Record, FBBBMonsterStreamBatch::ReliableBytes))
        {
            Flush();
            const bool bAdded = Batch.TryAdd(Record, FBBBMonsterStreamBatch::ReliableBytes);
            ensureMsgf(bAdded, TEXT("回放小怪单条记录超出预算"));
        }
    };
    for (const auto& State : Monsters)
    {
        Add(MakeActionRecord(State, ERecord::Birth));
        for (const auto& Contribution : State.Contributions)
        {
            Add(MakeDamageRecord(State.InstanceId, Contribution, false));
        }
    }
    Flush();
    for (int32 Index = ReplayPages.Num() - 1; Index >= Page; --Index)
    {
        if (auto* Channel = ReplayPages[Index].Get())
        {
            Channel->Destroy();
        }
        ReplayPages.RemoveAt(Index, 1, EAllowShrinking::No);
    }
}

void ABBBMassNetworkActor::ReceiveReplay()
{
    TSet<FGuid> Present;
    TSet<FGuid> NewBirths;
    TArray<const FBBBMonsterStreamRecord*, TInlineAllocator<256>> Damage;
    TSet<TWeakObjectPtr<ABBBMonsterStreamChannel>> ExistingPages;
    for (TActorIterator<ABBBMonsterStreamChannel> It(GetWorld()); It; ++It)
    {
        auto* Channel = *It;
        const auto& Page = Channel->ReadReplay();
        if (Page.Records.IsEmpty())
        {
            continue;
        }
        ExistingPages.Add(Channel);
        const bool bChanged = AppliedReplayPages.FindRef(Channel) != Page.Sequence;
        AppliedReplayPages.Add(Channel, Page.Sequence);
        for (const auto& Record : Page.Records)
        {
            if (Record.Kind == ERecord::Birth)
            {
                Present.Add(Record.Action.InstanceId);
                if (!FindEntity(Record.Action.InstanceId).IsSet())
                {
                    NewBirths.Add(Record.Action.InstanceId);
                }
                ReceiveMonsterRecord(Record);
            }
            if (bChanged && Record.Kind == ERecord::Damage)
            {
                Damage.Add(&Record);
            }
        }
    }
    for (auto It = AppliedReplayPages.CreateIterator(); It; ++It)
    {
        if (!ExistingPages.Contains(It.Key()))
        {
            It.RemoveCurrent();
        }
    }
    for (const auto* Record : Damage)
    {
        if (NewBirths.Contains(Record->Action.InstanceId))
        {
            FBBBMonsterStreamRecord Initial = *Record;
            Initial.Kind = ERecord::InitialDamage;
            ReceiveMonsterRecord(Initial);
        }
        else
        {
            ReceiveMonsterRecord(*Record);
        }
    }

    const double Now = GetWorld()->GetTimeSeconds();
    TArray<FGuid, TInlineAllocator<32>> Gone;
    for (const auto& Pair : LocalEntities)
    {
        if (Present.Contains(Pair.Key))
        {
            ReplayMissingSince.Remove(Pair.Key);
        }
        else if (Now - ReplayMissingSince.FindOrAdd(Pair.Key, Now) > 1.0)
        {
            Gone.Add(Pair.Key);
        }
    }
    for (const auto& Id : Gone)
    {
        ReplayMissingSince.Remove(Id);
        ForgetMonster(Id);
    }
}
