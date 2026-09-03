#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/System/NetworkSystem/Definition/Packets/BBBEquipmentActionNetworkPacket.h"
#include "BBBWork/UBBBNexus/Character/System/NetworkSystem/Definition/Packets/BBBEquipmentNetworkPacket.h"
#include "BBBWork/UBBBNexus/Character/System/NetworkSystem/Definition/States/BBBNetworkStates.h"
#include "BBBNetworkRuntimeData.generated.h"

/** 角色网络领域唯一运行数据根 */
USTRUCT()
struct FBBBNetworkRuntimeData
{
    GENERATED_BODY()

    /** @return 最后上传的主手装备实例标识 */
    const FGuid &GetLastUploadedEquipmentInstanceId() const
    {
        return LastUploadedEquipmentInstanceId;
    }

    /** @param EquipmentInstanceId 主手装备实例标识 */
    void CommitLastUploadedEquipmentInstanceId(const FGuid &EquipmentInstanceId)
    {
        LastUploadedEquipmentInstanceId = EquipmentInstanceId;
    }

    /** @param Packet 待还原装备包 */
    void EnqueueRestoreEquipmentPacket(FBBBEquipmentNetworkPacket Packet)
    {
        PendingRestoreEquipmentPackets.Add(MoveTemp(Packet));
    }

    /** @param Packet 待还原装备动作包 */
    void EnqueueRestoreEquipmentActionPacket(FBBBEquipmentActionNetworkPacket Packet)
    {
        PendingRestoreEquipmentActionPackets.Add(MoveTemp(Packet));
    }

    /** @return 待还原装备包 */
    TArray<FBBBEquipmentNetworkPacket> RestoreEquipmentPackets()
    {
        return MoveTemp(PendingRestoreEquipmentPackets);
    }

    /** @return 待还原装备动作包 */
    TArray<FBBBEquipmentActionNetworkPacket> RestoreEquipmentActionPackets()
    {
        return MoveTemp(PendingRestoreEquipmentActionPackets);
    }

    /** @param AimState 待还原瞄准状态 */
    void SetPendingRestoreAimState(const FBBBAimNetworkState &AimState)
    {
        PendingRestoreAimState = AimState;
    }

    /** @param LocomotionState 待还原移动状态 */
    void SetPendingRestoreLocomotionState(const FBBBLocomotionNetworkState &LocomotionState)
    {
        PendingRestoreLocomotionState = LocomotionState;
    }

    /** @return 待还原瞄准状态 */
    TOptional<FBBBAimNetworkState> RestoreAimState()
    {
        TOptional<FBBBAimNetworkState> AimState = MoveTemp(PendingRestoreAimState);
        PendingRestoreAimState.Reset();
        return AimState;
    }

    /** @return 待还原移动状态 */
    TOptional<FBBBLocomotionNetworkState> RestoreLocomotionState()
    {
        TOptional<FBBBLocomotionNetworkState> LocomotionState = MoveTemp(PendingRestoreLocomotionState);
        PendingRestoreLocomotionState.Reset();
        return LocomotionState;
    }

    /** @return 瞄准上传观测状态 */
    const FBBBAimNetworkObserverState &GetAimObserverState() const
    {
        return AimObserverState;
    }

    /** @param ObserverState 瞄准上传观测状态 */
    void CommitAimObserverState(const FBBBAimNetworkObserverState &ObserverState)
    {
        AimObserverState = ObserverState;
    }

    /** @return 移动状态上传观测值 */
    const FBBBLocomotionNetworkObserverState &GetLocomotionObserverState() const
    {
        return LocomotionObserverState;
    }

    /** @param ObserverState 移动状态上传观测值 */
    void CommitLocomotionObserverState(const FBBBLocomotionNetworkObserverState &ObserverState)
    {
        LocomotionObserverState = ObserverState;
    }

private:
    FGuid LastUploadedEquipmentInstanceId;

    FBBBAimNetworkObserverState AimObserverState;

    FBBBLocomotionNetworkObserverState LocomotionObserverState;

    UPROPERTY(Transient)
    TArray<FBBBEquipmentNetworkPacket> PendingRestoreEquipmentPackets;

    UPROPERTY(Transient)
    TArray<FBBBEquipmentActionNetworkPacket> PendingRestoreEquipmentActionPackets;

    TOptional<FBBBAimNetworkState> PendingRestoreAimState;

    TOptional<FBBBLocomotionNetworkState> PendingRestoreLocomotionState;
};
