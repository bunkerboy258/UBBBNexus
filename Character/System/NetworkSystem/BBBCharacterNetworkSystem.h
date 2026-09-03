#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/System/NetworkSystem/Definition/Packets/BBBEquipmentActionNetworkPacket.h"
#include "BBBWork/UBBBNexus/Character/System/NetworkSystem/Definition/Packets/BBBEquipmentNetworkPacket.h"
#include "BBBWork/UBBBNexus/Character/System/NetworkSystem/Definition/States/BBBNetworkStates.h"
#include "BBBWork/UBBBNexus/Character/System/NetworkSystem/Processors/Restore/BBBCharacterNetworkRestorer.h"
#include "BBBWork/UBBBNexus/Character/System/NetworkSystem/Processors/Upload/BBBCharacterNetworkUploader.h"

class FBBBCharacterInitializer;
class UBBBCharacterNetworkComponent;
class UBBBEquipmentCatalog;
struct FBBBAimRuntimeData;
struct FBBBCharacterEquipmentCommands;
struct FBBBCharacterEquipmentEvents;
struct FBBBCharacterEquipmentState;
struct FBBBCharacterLocomotionRuntimeData;
struct FBBBCharacterNetworkConfig;
struct FBBBCharacterWorldRuntimeData;
struct FBBBNetworkRuntimeData;

/** 角色状态与装备动作的无校验P2P同步系统 */
class ABBB_EVAC_API FBBBCharacterNetworkSystem final
{
public:
    /** 逐帧还原远端状态与动作 */
    void UpdateRestore();

    /** 逐帧上传本地状态与动作 */
    void UpdateUpload();

private:
    friend class FBBBCharacterInitializer;
    friend class UBBBCharacterNetworkComponent;
    friend class FBBBAimUploadProcessor;
    friend class FBBBEquipmentActionUploadProcessor;
    friend class FBBBEquipmentUploadProcessor;
    friend class FBBBLocomotionUploadProcessor;

    /**
     * 初始化网络系统依赖
     * @param InNetworkData 网络运行时数据
     * @param InAimData 瞄准运行时数据
     * @param InLocomotionData 移动运行时数据
     * @param InEquipmentState 装备状态
     * @param InNetworkComponent 网络组件
     * @param InEquipmentCatalog 装备目录
     * @param InWorldData 世界运行时数据
     * @param InEquipmentCommands 装备命令
     * @param InEquipmentEvents 装备事件
     * @param InNetworkConfig 网络配置
     */
    void Initialize(
        FBBBNetworkRuntimeData &InNetworkData,
        FBBBAimRuntimeData &InAimData,
        FBBBCharacterLocomotionRuntimeData &InLocomotionData,
        const FBBBCharacterEquipmentState &InEquipmentState,
        UBBBCharacterNetworkComponent &InNetworkComponent,
        UBBBEquipmentCatalog &InEquipmentCatalog,
        const FBBBCharacterWorldRuntimeData &InWorldData,
        FBBBCharacterEquipmentCommands &InEquipmentCommands,
        const FBBBCharacterEquipmentEvents &InEquipmentEvents,
        const FBBBCharacterNetworkConfig &InNetworkConfig);

    /** @param Packet 提交本地装备包 */
    void SubmitEquipmentPacket(FBBBEquipmentNetworkPacket Packet);

    /** @param Packet 提交本地装备动作包 */
    void SubmitEquipmentActionPacket(FBBBEquipmentActionNetworkPacket Packet);

    /** @param AimState 提交本地瞄准状态 */
    void SubmitAimState(const FBBBAimNetworkState &AimState);

    /** @param LocomotionState 提交本地移动状态 */
    void SubmitLocomotionState(const FBBBLocomotionNetworkState &LocomotionState);

    /** @param Packet 接收等待权威端直接分发的装备包 */
    void ReceiveEquipmentForDistribution(FBBBEquipmentNetworkPacket Packet);

    /** @param Packet 接收等待本地还原的装备包 */
    void ReceiveEquipmentForRestore(FBBBEquipmentNetworkPacket Packet);

    /** @param Packet 接收等待权威端直接分发的装备动作包 */
    void ReceiveEquipmentActionForDistribution(FBBBEquipmentActionNetworkPacket Packet);

    /** @param Packet 接收等待本地还原的装备动作包 */
    void ReceiveEquipmentActionForRestore(FBBBEquipmentActionNetworkPacket Packet);

    void ReceiveSubmittedAimState(const FBBBAimNetworkState &AimState);

    void ReceiveReplicatedAimState(const FBBBAimNetworkState &AimState);

    void ReceiveSubmittedLocomotionState(const FBBBLocomotionNetworkState &LocomotionState);

    void ReceiveReplicatedLocomotionState(const FBBBLocomotionNetworkState &LocomotionState);

    FBBBNetworkRuntimeData *NetworkData = nullptr;

    const FBBBCharacterWorldRuntimeData *WorldData = nullptr;

    FBBBAimRuntimeData *AimData = nullptr;

    FBBBCharacterLocomotionRuntimeData *LocomotionData = nullptr;

    FBBBCharacterEquipmentCommands *EquipmentCommands = nullptr;

    const FBBBCharacterEquipmentEvents *EquipmentEvents = nullptr;

    const FBBBCharacterEquipmentState *EquipmentState = nullptr;

    const FBBBCharacterNetworkConfig *NetworkConfig = nullptr;

    UBBBCharacterNetworkComponent *NetworkComponent = nullptr;

    UBBBEquipmentCatalog *EquipmentCatalog = nullptr;

    FBBBCharacterNetworkUploader Uploader;

    FBBBCharacterNetworkRestorer Restorer;
};
