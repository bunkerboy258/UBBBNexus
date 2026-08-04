
#pragma once
#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/System/NetworkSystem/Definition/Packets/BBBEquipmentNetworkPacket.h"
#include "BBBWork/UBBBNexus/Character/System/NetworkSystem/Definition/Packets/BBBFireNetworkPacket.h"
#include "BBBWork/UBBBNexus/Character/System/NetworkSystem/Definition/Packets/BBBReloadNetworkPacket.h"
#include "BBBWork/UBBBNexus/Character/System/NetworkSystem/Definition/States/BBBNetworkStates.h"
#include "BBBNetworkRuntimeData.generated.h"

USTRUCT()
struct FBBBNetworkRuntimeData
{
    GENERATED_BODY()

    /**
     * 读取最后上传的主手物品实例
     * @return 最后上传的主手物品实例标识
     */
    const FGuid &GetLastUploadedEquipmentInstanceId() const
    {
        return LastUploadedEquipmentInstanceId;
    }

    /**
     * 记录最后上传的主手物品实例
     * @param EquipmentInstanceId	主手物品实例标识
     */
    void CommitLastUploadedEquipmentInstanceId(const FGuid &EquipmentInstanceId)
    {
        LastUploadedEquipmentInstanceId = EquipmentInstanceId;
    }

    /**
     * 将装备Packet加入待校验队列
     * @param Packet	装备网络Packet
     */
    void EnqueueValidationEquipmentPacket(FBBBEquipmentNetworkPacket Packet)
    {
        PendingValidationEquipmentPackets.Add(MoveTemp(Packet));
    }

    /**
     * 将装备Packet加入待还原队列
     * @param Packet	装备网络Packet
     */
    void EnqueueRestoreEquipmentPacket(FBBBEquipmentNetworkPacket Packet)
    {
        PendingRestoreEquipmentPackets.Add(MoveTemp(Packet));
    }

    /**
     * 将开火Packet加入待还原队列
     * @param Packet	开火网络Packet
     */
    void EnqueueRestoreFirePacket(FBBBFireNetworkPacket Packet)
    {
        PendingRestoreFirePackets.Add(MoveTemp(Packet));
    }

    /**
     * 将换弹Packet加入待还原队列
     * @param Packet	换弹网络Packet
     */
    void EnqueueRestoreReloadPacket(FBBBReloadNetworkPacket Packet)
    {
        PendingRestoreReloadPackets.Add(MoveTemp(Packet));
    }

    /**
     * 记录待还原的瞄准状态
     * @param AimState	待还原瞄准状态
     */
    void SetPendingRestoreAimState(const FBBBAimNetworkState &AimState)
    {
        PendingRestoreAimState = AimState;
    }

    /**
     * 取出并清空待校验装备Packet队列
     * @return 待校验装备Packet列表
     */
    TArray<FBBBEquipmentNetworkPacket> ValidationEquipmentPackets()
    {
        return MoveTemp(PendingValidationEquipmentPackets);
    }

    /**
     * 取出并清空待还原装备Packet队列
     * @return 待还原装备Packet列表
     */
    TArray<FBBBEquipmentNetworkPacket> RestoreEquipmentPackets()
    {
        return MoveTemp(PendingRestoreEquipmentPackets);
    }

    /**
     * 取出并清空待还原开火Packet队列
     * @return 待还原开火Packet列表
     */
    TArray<FBBBFireNetworkPacket> RestoreFirePackets()
    {
        return MoveTemp(PendingRestoreFirePackets);
    }

    /**
     * 取出并清空待还原换弹Packet队列
     * @return 待还原换弹Packet列表
     */
    TArray<FBBBReloadNetworkPacket> RestoreReloadPackets()
    {
        return MoveTemp(PendingRestoreReloadPackets);
    }

    /**
     * 取出并清空待还原瞄准状态
     * @return 待还原瞄准状态 无记录时为空
     */
    TOptional<FBBBAimNetworkState> RestoreAimState()
    {
        TOptional<FBBBAimNetworkState> AimState = MoveTemp(PendingRestoreAimState);
        PendingRestoreAimState.Reset();
        return AimState;
    }

    /**
     * 读取瞄准上传观测状态
     * @return 瞄准上传观测状态
     */
    const FBBBAimNetworkObserverState &GetAimObserverState() const
    {
        return AimObserverState;
    }

    /**
     * 提交瞄准上传观测状态
     * @param ObserverState	瞄准上传观测状态
     */
    void CommitAimObserverState(const FBBBAimNetworkObserverState &ObserverState)
    {
        AimObserverState = ObserverState;
    }
private:

    //最后上传的 主手物品实例标识
    FGuid LastUploadedEquipmentInstanceId;

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
