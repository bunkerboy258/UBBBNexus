#pragma once

#include "GameFramework/Actor.h"
#include "BBBMonsterStreamBatch.h"
#include "BBBMonsterStreamSubscription.h"
#include "BBBMonsterStreamChannel.generated.h"

class ABBBMassNetworkActor;

/** 当前小怪状态的独立 UE Actor 通道 一个连接分别使用可靠和运动通道 */
UCLASS(NotBlueprintable)
class ABBB_EVAC_API ABBBMonsterStreamChannel final : public AActor
{
    GENERATED_BODY()

public:
    ABBBMonsterStreamChannel();
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
    virtual void EndPlay(const EEndPlayReason::Type Reason) override;

    bool CanSendReliable() const;
    bool IsDelivered(uint32 Sequence) const;
    uint32 SendReliable(FBBBMonsterStreamBatch& Batch);
    bool CanUpload() const;
    void SendUpload(FBBBMonsterStreamBatch& Batch);
    void PumpReceive(ABBBMassNetworkActor& Transport);
    void PublishReplay(FBBBMonsterStreamBatch&& Batch);
    const FBBBMonsterStreamBatch& ReadReplay() const { return ReplayBatch; }
    void SetControl() { bControl = true; }
    bool IsControl() const { return bControl; }

    UFUNCTION(Client, Reliable)
    void ClientReceiveReliable(const FBBBMonsterStreamBatch& Batch);

    UFUNCTION(Client, Unreliable)
    void ClientReceiveMotion(const FBBBMonsterStreamBatch& Batch);

    UFUNCTION(Server, Reliable)
    void ServerAcknowledge(uint32 Sequence);

    UFUNCTION(Server, Reliable)
    void ServerReportDamage(const FBBBMonsterStreamBatch& Batch);

    UFUNCTION(Client, Reliable)
    void ClientAcknowledgeUpload(uint32 Sequence);

    TMap<FGuid, FBBBMonsterStreamSubscription> Subscriptions;
    TWeakObjectPtr<ABBBMonsterStreamChannel> MotionChannel;

    float NextSendTime = 0.0f;
    int32 MotionCursor = 0;
    int32 ReliableCursor = 0;
    int32 ReliableFrames = 0;

private:
    UPROPERTY(Replicated)
    bool bControl = false;

    UPROPERTY(Replicated)
    FBBBMonsterStreamBatch ReplayBatch;

    TArray<FBBBMonsterStreamBatch> PendingReceive;
    uint32 LastSent = 0;
    uint32 LastAcknowledged = 0;
    uint32 LastAppliedAwaitingAck = 0;
    uint32 LastUploaded = 0;
    uint32 LastUploadAcknowledged = 0;
};
