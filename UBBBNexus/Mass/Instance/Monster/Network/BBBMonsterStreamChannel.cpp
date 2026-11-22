#include "BBBMonsterStreamChannel.h"

#include "EngineUtils.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/PlayerState.h"
#include "Net/UnrealNetwork.h"
#include "BBBWork/UBBBNexus/Mass/Network/BBBMassNetworkActor.h"

namespace
{
    ABBBMassNetworkActor* FindTransport(UWorld* World)
    {
        for (TActorIterator<ABBBMassNetworkActor> It(World); It; ++It)
        {
            return *It;
        }
        return nullptr;
    }
}

ABBBMonsterStreamChannel::ABBBMonsterStreamChannel()
{
    PrimaryActorTick.bCanEverTick = false;
    bReplicates = true;
    bOnlyRelevantToOwner = true;
    bRelevantForNetworkReplays = false;
    SetNetUpdateFrequency(15.0f);
}

void ABBBMonsterStreamChannel::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME_CONDITION(ABBBMonsterStreamChannel, bControl, COND_InitialOnly);
    DOREPLIFETIME_CONDITION(ABBBMonsterStreamChannel, ReplayBatch, COND_ReplayOnly);
}

void ABBBMonsterStreamChannel::EndPlay(const EEndPlayReason::Type Reason)
{
    PendingReceive.Reset();
    Super::EndPlay(Reason);
}

bool ABBBMonsterStreamChannel::CanSendReliable() const
{
    return LastSent - LastAcknowledged < 4;
}

bool ABBBMonsterStreamChannel::IsDelivered(uint32 Sequence) const
{
    return int32(LastAcknowledged - Sequence) >= 0;
}

uint32 ABBBMonsterStreamChannel::SendReliable(FBBBMonsterStreamBatch& Batch)
{
    check(CanSendReliable() && !Batch.Records.IsEmpty());
    Batch.Sequence = ++LastSent;
    ClientReceiveReliable(Batch);
    return LastSent;
}

bool ABBBMonsterStreamChannel::CanUpload() const
{
    return LastUploaded - LastUploadAcknowledged < 4;
}

void ABBBMonsterStreamChannel::SendUpload(FBBBMonsterStreamBatch& Batch)
{
    check(CanUpload() && !Batch.Records.IsEmpty());
    Batch.Sequence = ++LastUploaded;
    ServerReportDamage(Batch);
}

void ABBBMonsterStreamChannel::ServerAcknowledge_Implementation(uint32 Sequence)
{
    if (int32(Sequence - LastAcknowledged) > 0 && int32(LastSent - Sequence) >= 0)
    {
        LastAcknowledged = Sequence;
    }
}

void ABBBMonsterStreamChannel::PumpReceive(ABBBMassNetworkActor& Transport)
{
    if (LastAppliedAwaitingAck != 0)
    {
        ServerAcknowledge(LastAppliedAwaitingAck);
        LastAppliedAwaitingAck = 0;
    }
    uint32 Applied = 0;
    int32 Consumed = 0;
    for (const auto& Batch : PendingReceive)
    {
        bool bApplied = true;
        for (const auto& Record : Batch.Records)
        {
            if (!Transport.ReceiveMonsterRecord(Record))
            {
                bApplied = false;
                break;
            }
        }
        if (!bApplied)
        {
            break;
        }
        Applied = Batch.Sequence;
        ++Consumed;
    }
    if (Consumed > 0)
    {
        PendingReceive.RemoveAt(0, Consumed, EAllowShrinking::No);
        LastAppliedAwaitingAck = Applied;
    }
}

void ABBBMonsterStreamChannel::ClientReceiveReliable_Implementation(const FBBBMonsterStreamBatch& Batch)
{
    PendingReceive.Add(Batch);
}

void ABBBMonsterStreamChannel::ClientReceiveMotion_Implementation(const FBBBMonsterStreamBatch& Batch)
{
    if (auto* Transport = FindTransport(GetWorld()))
    {
        for (const auto& Record : Batch.Records)
        {
            Transport->ReceiveMonsterRecord(Record);
        }
    }
}

void ABBBMonsterStreamChannel::ClientAcknowledgeUpload_Implementation(uint32 Sequence)
{
    if (int32(Sequence - LastUploadAcknowledged) > 0 && int32(LastUploaded - Sequence) >= 0)
    {
        LastUploadAcknowledged = Sequence;
    }
}

void ABBBMonsterStreamChannel::ServerReportDamage_Implementation(const FBBBMonsterStreamBatch& Batch)
{
    const auto* Controller = Cast<APlayerController>(GetOwner());
    const auto* Player = Controller ? Controller->GetPlayerState<APlayerState>() : nullptr;
    if (!Player)
    {
        return;
    }
    if (auto* Transport = FindTransport(GetWorld()))
    {
        for (const auto& Record : Batch.Records)
        {
            if (Record.Kind == FBBBMonsterStreamRecord::EKind::Damage &&
                Subscriptions.Contains(Record.Action.InstanceId))
            {
                auto Value = Record.Damage;
                Value.PlayerId = Player->GetPlayerId();
                Value.bWorldSource = false;
                Transport->ReceivePlayerDamage(Record.Action.InstanceId, Value);
            }
        }
        ClientAcknowledgeUpload(Batch.Sequence);
    }
}

void ABBBMonsterStreamChannel::PublishReplay(FBBBMonsterStreamBatch&& Batch)
{
    ReplayBatch = MoveTemp(Batch);
}
