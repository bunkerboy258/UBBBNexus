
#pragma once
#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/System/NetworkSystem/Definition/Packets/BBBEquipmentNetworkPacket.h"
#include "BBBWork/UBBBNexus/Character/System/NetworkSystem/Definition/Packets/BBBFireNetworkPacket.h"
#include "BBBWork/UBBBNexus/Character/System/NetworkSystem/Definition/Packets/BBBReloadNetworkPacket.h"
#include "BBBWork/UBBBNexus/Character/System/NetworkSystem/Definition/States/BBBAimNetworkState.h"
#include "BBBNetworkRuntimeData.generated.h"

USTRUCT()
struct FBBBAimNetworkObserverState
{
    GENERATED_BODY()

    //上次发给服务器的瞄准数据
    TOptional<FBBBAimNetworkState> LastObservedState;

    float LastUploadTime = -1000.0f;
};

USTRUCT()
struct FBBBNetworkRuntimeData
{
    GENERATED_BODY()

    //读取最后上传的主手物品实例
    const FGuid &GetLastUploadedItemInstanceId() const
    {
        return LastUploadedItemInstanceId;
    }

    //记录最后上传的主手物品实例
    void CommitLastUploadedItemInstanceId(const FGuid &ItemInstanceId)
    {
        LastUploadedItemInstanceId = ItemInstanceId;
    }

    void EnqueueValidationEquipmentPacket(FBBBEquipmentNetworkPacket Packet)
    {
        PendingValidationEquipmentPackets.Add(MoveTemp(Packet));
    }

    void EnqueueRestoreEquipmentPacket(FBBBEquipmentNetworkPacket Packet)
    {
        PendingRestoreEquipmentPackets.Add(MoveTemp(Packet));
    }

    void EnqueueRestoreFirePacket(FBBBFireNetworkPacket Packet)
    {
        PendingRestoreFirePackets.Add(MoveTemp(Packet));
    }

    void EnqueueRestoreReloadPacket(FBBBReloadNetworkPacket Packet)
    {
        PendingRestoreReloadPackets.Add(MoveTemp(Packet));
    }

    void SetPendingRestoreAimState(const FBBBAimNetworkState &AimState)
    {
        PendingRestoreAimState = AimState;
    }

    TArray<FBBBEquipmentNetworkPacket> ValidationEquipmentPackets()
    {
        return MoveTemp(PendingValidationEquipmentPackets);
    }

    TArray<FBBBEquipmentNetworkPacket> RestoreEquipmentPackets()
    {
        return MoveTemp(PendingRestoreEquipmentPackets);
    }

    TArray<FBBBFireNetworkPacket> RestoreFirePackets()
    {
        return MoveTemp(PendingRestoreFirePackets);
    }

    TArray<FBBBReloadNetworkPacket> RestoreReloadPackets()
    {
        return MoveTemp(PendingRestoreReloadPackets);
    }

    TOptional<FBBBAimNetworkState> RestoreAimState()
    {
        TOptional<FBBBAimNetworkState> AimState = MoveTemp(PendingRestoreAimState);
        PendingRestoreAimState.Reset();
        return AimState;
    }

    const FBBBAimNetworkObserverState &GetAimObserverState() const
    {
        return AimObserverState;
    }

    void CommitAimObserverState(const FBBBAimNetworkObserverState &ObserverState)
    {
        AimObserverState = ObserverState;
    }
private:

    //最后上传的 主手物品实例标识
    FGuid LastUploadedItemInstanceId;

    //上次发给服务器的瞄准数据
    FBBBAimNetworkObserverState AimObserverState;

    UPROPERTY(Transient)
    //装备Packets待校验队列
    TArray<FBBBEquipmentNetworkPacket> PendingValidationEquipmentPackets;

    UPROPERTY(Transient)
    //装备Packets待还原队列
    TArray<FBBBEquipmentNetworkPacket> PendingRestoreEquipmentPackets;

    UPROPERTY(Transient)
    //开火Packets待还原队列
    TArray<FBBBFireNetworkPacket> PendingRestoreFirePackets;

    UPROPERTY(Transient)
    //换弹Packets待还原队列
    TArray<FBBBReloadNetworkPacket> PendingRestoreReloadPackets;

    //瞄准States待还原队列
    TOptional<FBBBAimNetworkState> PendingRestoreAimState;
};
