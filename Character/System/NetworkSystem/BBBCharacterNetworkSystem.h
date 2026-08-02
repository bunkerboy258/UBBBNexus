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
struct FBBBAimConfig;
struct FBBBAimRuntimeData;
struct FBBBCharacterEquipmentState;
struct FBBBCharacterItemEvents;
struct FBBBCharacterWorldRuntimeData;
struct FBBBNetworkRuntimeData;

//角色网络系统
class ABBB_EVAC_API FBBBCharacterNetworkSystem final
{
public:

    //校验网络包
    void UpdateValidation();

    //应用网络包
    void UpdateRestore();

    //生成网络包
    void UpdateUpload();

private:
    friend class FBBBCharacterInitializer;

    void Initialize(
        FBBBNetworkRuntimeData &InNetworkData,
        FBBBAimRuntimeData &InAimData,
        FBBBCharacterEquipmentState &InEquipmentState,
        UBBBCharacterNetworkComponent &InNetworkComponent,
        const FBBBCharacterWorldRuntimeData &InWorldData,
        const FBBBCharacterItemEvents &InItemEvents,
        const FBBBAimConfig &InAimConfig);

    bool HasRequiredDependencies() const;

    FBBBNetworkRuntimeData *NetworkData = nullptr;
    const FBBBCharacterWorldRuntimeData *WorldData = nullptr;
    FBBBAimRuntimeData *AimData = nullptr;
    const FBBBCharacterItemEvents *ItemEvents = nullptr;
    FBBBCharacterEquipmentState *EquipmentState = nullptr;

    //瞄准配置引用
    const FBBBAimConfig *AimConfig = nullptr;

    UBBBCharacterNetworkComponent *NetworkComponent = nullptr;
    FBBBCharacterNetworkUploader Uploader;
    FBBBCharacterNetworkValidator Validator;
    FBBBCharacterNetworkRestorer Restorer;
};
