#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Runtime/System/NetworkSystem/Processors/BBBCharacterNetworkObservationProcessor.h"

class FBBBCharacterInitializer;
class UBBBCharacterNetworkComponent;
struct FBBBAimRuntimeData;
struct FBBBAimNetworkState;
struct FBBBCharacterEquipmentEvents;
struct FBBBCharacterEquipmentState;
struct FBBBEquipmentActionFact;
struct FBBBCharacterLocomotionRuntimeData;
struct FBBBLocomotionNetworkState;
struct FBBBCharacterNetworkConfig;
struct FBBBCharacterNetworkIdentityRuntimeData;
struct FBBBCharacterWorldRuntimeData;
struct FBBBNetworkState;

/**
 * 角色网络边界
 *
 * 本机客户端只上传请求，权威角色只观察已经形成的事实，普通远端角色只等待还原输入
 */
class ABBB_EVAC_API FBBBCharacterNetworkSystem final
{
public:
    /** 根据角色网络身份执行唯一网络阶段 */
    void Update();

private:
    friend class FBBBCharacterInitializer;
    friend class FBBBAimObservationProcessor;
    friend class FBBBEquipmentFactObservationProcessor;
    friend class FBBBEquipmentStateObservationProcessor;
    friend class FBBBLocomotionObservationProcessor;

    /**
     * 注入角色网络阶段需要的黑板和传输依赖
     * @param InNetworkData       网络观测状态
     * @param InNetworkIdentity   角色本帧网络身份事实
     * @param InAimData           角色瞄准状态
     * @param InLocomotionData    角色移动状态
     * @param InEquipmentState    角色装备状态
     * @param InNetworkComponent  网络传输组件
     * @param InWorldData         角色世界时间
     * @param InEquipmentEvents   角色装备事实
     * @param InNetworkConfig     网络发送配置
     */
    void Initialize(
        FBBBNetworkState &InNetworkData,
        const FBBBCharacterNetworkIdentityRuntimeData &InNetworkIdentity,
        FBBBAimRuntimeData &InAimData,
        FBBBCharacterLocomotionRuntimeData &InLocomotionData,
        const FBBBCharacterEquipmentState &InEquipmentState,
        UBBBCharacterNetworkComponent &InNetworkComponent,
        const FBBBCharacterWorldRuntimeData &InWorldData,
        const FBBBCharacterEquipmentEvents &InEquipmentEvents,
        const FBBBCharacterNetworkConfig &InNetworkConfig);

    /** 观察权威角色已经形成的黑板事实并立即提交传输组件 */
    void Observe();

    void TransmitEquipmentFact(FBBBEquipmentActionFact Fact);
    void TransmitEquipmentState(FName EquipmentId);
    void TransmitAimState(const FBBBAimNetworkState &AimState);
    void TransmitLocomotionState(const FBBBLocomotionNetworkState &LocomotionState);

    FBBBNetworkState *NetworkData = nullptr;
    const FBBBCharacterNetworkIdentityRuntimeData *NetworkIdentity = nullptr;
    const FBBBCharacterWorldRuntimeData *WorldData = nullptr;
    FBBBAimRuntimeData *AimData = nullptr;
    FBBBCharacterLocomotionRuntimeData *LocomotionData = nullptr;
    const FBBBCharacterEquipmentEvents *EquipmentEvents = nullptr;
    const FBBBCharacterEquipmentState *EquipmentState = nullptr;
    const FBBBCharacterNetworkConfig *NetworkConfig = nullptr;
    UBBBCharacterNetworkComponent *NetworkComponent = nullptr;

    FBBBCharacterNetworkObservationProcessor ObservationProcessor;
};
