#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Instance/System/NetworkSystem/Definition/Packets/BBBEquipmentActionNetworkPacket.h"
#include "BBBWork/UBBBNexus/Character/Instance/System/NetworkSystem/Definition/Packets/BBBEquipmentNetworkPacket.h"
#include "BBBWork/UBBBNexus/Character/Instance/System/NetworkSystem/Processors/Mode/BBBNetworkAuthorityLocalProcessor.h"
#include "BBBWork/UBBBNexus/Character/Instance/System/NetworkSystem/Processors/Mode/BBBNetworkAuthorityRemoteProcessor.h"
#include "BBBWork/UBBBNexus/Character/Instance/System/NetworkSystem/Processors/Mode/BBBNetworkClientLocalProcessor.h"
#include "BBBWork/UBBBNexus/Character/Instance/System/NetworkSystem/Processors/Mode/BBBNetworkClientRemoteProcessor.h"
#include "BBBWork/UBBBNexus/Character/Instance/System/NetworkSystem/Processors/Observe/BBBCharacterNetworkFactProcessor.h"

class FBBBCharacterInitializer;
class UBBBCharacterNetworkComponent;
struct FBBBAimRuntimeData;
struct FBBBAimNetworkState;
struct FBBBCharacterEquipmentEvents;
struct FBBBCharacterEquipmentState;
struct FBBBCharacterLocomotionRuntimeData;
struct FBBBLocomotionNetworkState;
struct FBBBCharacterNetworkConfig;
struct FBBBCharacterWorldRuntimeData;
struct FBBBNetworkRuntimeData;

/** 观察角色黑板并同步已形成事实的网络系统 */
class ABBB_EVAC_API FBBBCharacterNetworkSystem final
{
public:
    void UpdateAuthorityLocal();
    void UpdateAuthorityRemote();
    void UpdateClientLocal();
    void UpdateClientRemote();

private:
    friend class FBBBCharacterInitializer;
    friend class FBBBNetworkAuthorityLocalProcessor;
    friend class FBBBNetworkAuthorityRemoteProcessor;
    friend class FBBBNetworkClientLocalProcessor;
    friend class FBBBNetworkClientRemoteProcessor;
    friend class FBBBAimUploadProcessor;
    friend class FBBBEquipmentActionUploadProcessor;
    friend class FBBBEquipmentUploadProcessor;
    friend class FBBBLocomotionUploadProcessor;

    void Initialize(
        FBBBNetworkRuntimeData &InNetworkData,
        FBBBAimRuntimeData &InAimData,
        FBBBCharacterLocomotionRuntimeData &InLocomotionData,
        const FBBBCharacterEquipmentState &InEquipmentState,
        UBBBCharacterNetworkComponent &InNetworkComponent,
        const FBBBCharacterWorldRuntimeData &InWorldData,
        const FBBBCharacterEquipmentEvents &InEquipmentEvents,
        const FBBBCharacterNetworkConfig &InNetworkConfig);

    void ObserveFacts();

    void SubmitEquipmentPacket(FBBBEquipmentNetworkPacket Packet);
    void SubmitEquipmentActionPacket(FBBBEquipmentActionNetworkPacket Packet);
    void SubmitAimState(const FBBBAimNetworkState &AimState);
    void SubmitLocomotionState(const FBBBLocomotionNetworkState &LocomotionState);

    FBBBNetworkRuntimeData *NetworkData = nullptr;
    const FBBBCharacterWorldRuntimeData *WorldData = nullptr;
    FBBBAimRuntimeData *AimData = nullptr;
    FBBBCharacterLocomotionRuntimeData *LocomotionData = nullptr;
    const FBBBCharacterEquipmentEvents *EquipmentEvents = nullptr;
    const FBBBCharacterEquipmentState *EquipmentState = nullptr;
    const FBBBCharacterNetworkConfig *NetworkConfig = nullptr;
    UBBBCharacterNetworkComponent *NetworkComponent = nullptr;
    FBBBCharacterNetworkFactProcessor FactProcessor;
    FBBBNetworkAuthorityLocalProcessor AuthorityLocalProcessor;
    FBBBNetworkAuthorityRemoteProcessor AuthorityRemoteProcessor;
    FBBBNetworkClientLocalProcessor ClientLocalProcessor;
    FBBBNetworkClientRemoteProcessor ClientRemoteProcessor;
};
