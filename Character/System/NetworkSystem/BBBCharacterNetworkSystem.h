#pragma once
#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/System/NetworkSystem/Definition/Packets/BBBEquipmentNetworkPacket.h"
#include "BBBWork/UBBBNexus/Character/System/NetworkSystem/Definition/Packets/BBBFireNetworkPacket.h"
#include "BBBWork/UBBBNexus/Character/System/NetworkSystem/Definition/Packets/BBBReloadNetworkPacket.h"
#include "BBBWork/UBBBNexus/Character/System/NetworkSystem/Definition/States/BBBNetworkStates.h"
#include "BBBWork/UBBBNexus/Character/System/NetworkSystem/Processors/Restore/BBBCharacterNetworkRestorer.h"
#include "BBBWork/UBBBNexus/Character/System/NetworkSystem/Processors/Upload/BBBCharacterNetworkUploader.h"
#include "BBBWork/UBBBNexus/Character/System/NetworkSystem/Processors/Validation/BBBCharacterNetworkValidator.h"
class FBBBCharacterInitializer;
class UBBBCharacterNetworkComponent;
class UBBBEquipmentCatalog;
struct FBBBCharacterNetworkConfig;
struct FBBBAimRuntimeData;
struct FBBBCharacterEquipmentState;
struct FBBBCharacterEquipmentResults;
struct FBBBCharacterEquipmentCommands;
struct FBBBCharacterWorldRuntimeData;
struct FBBBNetworkRuntimeData;

//角色网络系统
class ABBB_EVAC_API FBBBCharacterNetworkSystem final
{
public:

    /**
     * 逐帧校验待校验网络Packet 通过者广播
     */
    void UpdateValidation();

    /**
     * 逐帧取出待还原网络Packet并应用到本地状态
     */
    void UpdateRestore();

    /**
     * 逐帧将本地状态变化生成网络Packet并上传
     */
    void UpdateUpload();

private:
    friend class FBBBCharacterInitializer;
    friend class UBBBCharacterNetworkComponent;
    friend class FBBBAimUploadProcessor;
    friend class FBBBEquipmentUploadProcessor;
    friend class FBBBFireUploadProcessor;
    friend class FBBBReloadUploadProcessor;

    /**
     * 初始化网络系统依赖
     * @param InNetworkData	网络运行时数据
     * @param InAimData	瞄准运行时数据
     * @param InEquipmentState	角色装备状态
     * @param InNetworkComponent	角色网络组件
     * @param InEquipmentCatalog	装备目录
     * @param InWorldData	角色世界运行时数据
     * @param InEquipmentCommands	角色物品指令
     * @param InEquipmentResults	装备动作结果
     * @param InNetworkConfig	网络配置
     */
    void Initialize(
        FBBBNetworkRuntimeData &InNetworkData,
        FBBBAimRuntimeData &InAimData,
        const FBBBCharacterEquipmentState &InEquipmentState,
        UBBBCharacterNetworkComponent &InNetworkComponent,
        UBBBEquipmentCatalog &InEquipmentCatalog,
        const FBBBCharacterWorldRuntimeData &InWorldData,
        FBBBCharacterEquipmentCommands &InEquipmentCommands,
        const FBBBCharacterEquipmentResults &InEquipmentResults,
        const FBBBCharacterNetworkConfig &InNetworkConfig);

    /** @param Packet 提交本地装备Packet */
    void SubmitEquipmentPacket(FBBBEquipmentNetworkPacket Packet);

    /** @param Packet 提交本地开火Packet */
    void SubmitFirePacket(FBBBFireNetworkPacket Packet);

    /** @param Packet 提交本地换弹Packet */
    void SubmitReloadPacket(FBBBReloadNetworkPacket Packet);

    /** @param AimState 提交本地瞄准状态 */
    void SubmitAimState(const FBBBAimNetworkState &AimState);

    /** @param Packet 接收等待权威校验的装备Packet */
    void ReceiveEquipmentForValidation(FBBBEquipmentNetworkPacket Packet);

    /** @param Packet 接收等待本地还原的装备Packet */
    void ReceiveEquipmentForRestore(FBBBEquipmentNetworkPacket Packet);

    /** @param Packet 接收等待权威分发的开火Packet */
    void ReceiveFireForDistribution(FBBBFireNetworkPacket Packet);

    /** @param Packet 接收等待本地还原的开火Packet */
    void ReceiveFireForRestore(FBBBFireNetworkPacket Packet);

    /** @param Packet 接收等待权威分发的换弹Packet */
    void ReceiveReloadForDistribution(FBBBReloadNetworkPacket Packet);

    /** @param Packet 接收等待本地还原的换弹Packet */
    void ReceiveReloadForRestore(FBBBReloadNetworkPacket Packet);

    /** @param AimState 接收权威端提交的瞄准状态 */
    void ReceiveSubmittedAimState(const FBBBAimNetworkState &AimState);

    /** @param AimState 接收复制到达的瞄准状态 */
    void ReceiveReplicatedAimState(const FBBBAimNetworkState &AimState);

    FBBBNetworkRuntimeData *NetworkData = nullptr;
    const FBBBCharacterWorldRuntimeData *WorldData = nullptr;
    FBBBAimRuntimeData *AimData = nullptr;
    FBBBCharacterEquipmentCommands *EquipmentCommands = nullptr;
    const FBBBCharacterEquipmentResults *EquipmentResults = nullptr;
    const FBBBCharacterEquipmentState *EquipmentState = nullptr;

    const FBBBCharacterNetworkConfig *NetworkConfig = nullptr;

    UBBBCharacterNetworkComponent *NetworkComponent = nullptr;
    UBBBEquipmentCatalog *EquipmentCatalog = nullptr;
    FBBBCharacterNetworkUploader Uploader;
    FBBBCharacterNetworkValidator Validator;
    FBBBCharacterNetworkRestorer Restorer;
};
