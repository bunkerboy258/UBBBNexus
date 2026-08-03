#pragma once
#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/System/NetworkSystem/Definition/Packets/BBBEquipmentNetworkPacket.h"
#include "BBBWork/UBBBNexus/Character/System/NetworkSystem/Definition/Packets/BBBFireNetworkPacket.h"
#include "BBBWork/UBBBNexus/Character/System/NetworkSystem/Definition/Packets/BBBReloadNetworkPacket.h"
#include "BBBWork/UBBBNexus/Character/System/NetworkSystem/Definition/States/BBBAimNetworkState.h"
#include "BBBWork/UBBBNexus/Character/System/NetworkSystem/Processors/Restore/BBBCharacterNetworkRestorer.h"
#include "BBBWork/UBBBNexus/Character/System/NetworkSystem/Processors/Upload/BBBCharacterNetworkUploader.h"
#include "BBBWork/UBBBNexus/Character/System/NetworkSystem/Processors/Validation/BBBCharacterNetworkValidator.h"
class FBBBCharacterInitializer;
class UBBBCharacterNetworkComponent;
class UBBBEquipmentCatalog;
struct FBBBAimConfig;
struct FBBBAimRuntimeData;
struct FBBBCharacterEquipmentState;
struct FBBBCharacterItemActionResults;
struct FBBBCharacterItemCommands;
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

    /**
     * 初始化网络系统依赖
     * @param InNetworkData	网络运行时数据
     * @param InAimData	瞄准运行时数据
     * @param InEquipmentState	角色装备状态
     * @param InNetworkComponent	角色网络组件
     * @param InEquipmentCatalog	装备目录
     * @param InWorldData	角色世界运行时数据
     * @param InItemCommands	角色物品指令
     * @param InActionResults	装备动作结果
     * @param InAimConfig	瞄准配置
     */
    void Initialize(
        FBBBNetworkRuntimeData &InNetworkData,
        FBBBAimRuntimeData &InAimData,
        FBBBCharacterEquipmentState &InEquipmentState,
        UBBBCharacterNetworkComponent &InNetworkComponent,
        UBBBEquipmentCatalog &InEquipmentCatalog,
        const FBBBCharacterWorldRuntimeData &InWorldData,
        FBBBCharacterItemCommands &InItemCommands,
        const FBBBCharacterItemActionResults &InActionResults,
        const FBBBAimConfig &InAimConfig);

    /**
     * 检查网络系统依赖是否完备
     * @return 依赖是否全部有效
     */
    bool HasRequiredDependencies() const;

    FBBBNetworkRuntimeData *NetworkData = nullptr;
    const FBBBCharacterWorldRuntimeData *WorldData = nullptr;
    FBBBAimRuntimeData *AimData = nullptr;
    FBBBCharacterItemCommands *ItemCommands = nullptr;
    const FBBBCharacterItemActionResults *ActionResults = nullptr;
    FBBBCharacterEquipmentState *EquipmentState = nullptr;

    //瞄准配置引用
    const FBBBAimConfig *AimConfig = nullptr;

    UBBBCharacterNetworkComponent *NetworkComponent = nullptr;
    UBBBEquipmentCatalog *EquipmentCatalog = nullptr;
    FBBBCharacterNetworkUploader Uploader;
    FBBBCharacterNetworkValidator Validator;
    FBBBCharacterNetworkRestorer Restorer;
};
