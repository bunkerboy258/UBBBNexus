#include "BBBWork/UBBBNexus/Character/Runtime/System/NetworkSystem/BBBCharacterNetworkSystem.h"

#include "BBBWork/UBBBNexus/Character/Core/Config/Network/BBBNetworkConfig.h"
#include "BBBWork/UBBBNexus/Character/Runtime/Controller/EquipmentController/Definition/BBBCharacterEquipmentRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/Runtime/Controller/LocomotionController/Definition/BBBCharacterLocomotionRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/Runtime/State/Definition/BBBCharacterWorldRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/Runtime/System/NetworkSystem/BBBCharacterNetworkComponent.h"
#include "BBBWork/UBBBNexus/Character/Runtime/System/NetworkSystem/State/BBBNetworkState.h"

void FBBBCharacterNetworkSystem::Initialize(
    FBBBNetworkState &InNetworkData,
    FBBBAimRuntimeData &InAimData,
    FBBBCharacterLocomotionRuntimeData &InLocomotionData,
    const FBBBCharacterEquipmentState &InEquipmentState,
    UBBBCharacterNetworkComponent &InNetworkComponent,
    const FBBBCharacterWorldRuntimeData &InWorldData,
    const FBBBCharacterEquipmentEvents &InEquipmentEvents,
    const FBBBCharacterNetworkConfig &InNetworkConfig)
{
    NetworkData = &InNetworkData;
    AimData = &InAimData;
    LocomotionData = &InLocomotionData;
    EquipmentState = &InEquipmentState;
    NetworkComponent = &InNetworkComponent;
    WorldData = &InWorldData;
    EquipmentEvents = &InEquipmentEvents;
    NetworkConfig = &InNetworkConfig;
}

void FBBBCharacterNetworkSystem::Update()
{
    if (!NetworkComponent)
    {
        return;
    }

    // 只有权威或本机控制角色能够产生新事实
    // 模拟代理只消费复制到达的领域输入
    if (NetworkComponent->IsOwnerAuthority() || NetworkComponent->IsOwnerLocallyControlled())
    {
        Observe();
    }
}

void FBBBCharacterNetworkSystem::Observe()
{
    if (!(NetworkData && WorldData && AimData && LocomotionData && NetworkConfig
        && EquipmentState && EquipmentEvents && NetworkComponent))
    {
        return;
    }

    // 主管线已完成输入解析后再读取黑板
    // 观察器绝不写入角色状态
    ObservationProcessor.Update(
        *NetworkData,
        WorldData->GetWorldTimeSeconds(),
        *AimData,
        *LocomotionData,
        *NetworkConfig,
        *EquipmentState,
        *EquipmentEvents,
        *this);
}

void FBBBCharacterNetworkSystem::TransmitEquipmentFact(FBBBEquipmentActionFact Fact)
{
    if (!NetworkComponent)
    {
        return;
    }

    if (NetworkComponent->IsOwnerAuthority())
    {
        // 权威将最终事实写入只面向模拟代理的增量账本
        NetworkComponent->ReplicateEquipmentFact(MoveTemp(Fact));
        return;
    }

    if (NetworkComponent->IsOwnerLocallyControlled())
    {
        // 非权威本机控制角色只能把已成立事实交给权威
        NetworkComponent->ServerSubmitEquipmentFact(MoveTemp(Fact));
    }
}

void FBBBCharacterNetworkSystem::TransmitEquipmentState(const FName EquipmentId)
{
    if (!NetworkComponent)
    {
        return;
    }

    if (NetworkComponent->IsOwnerAuthority())
    {
        // 最终装备状态由权威复制给模拟代理
        NetworkComponent->ReplicateEquipmentState(EquipmentId);
        return;
    }

    if (NetworkComponent->IsOwnerLocallyControlled())
    {
        // 客户端不直接修改远端状态
        NetworkComponent->ServerSubmitEquipmentState(EquipmentId);
    }
}

void FBBBCharacterNetworkSystem::TransmitAimState(const FBBBAimNetworkState &AimState)
{
    if (!NetworkComponent)
    {
        return;
    }

    if (NetworkComponent->IsOwnerAuthority())
    {
        // 权威发布已经稳定的瞄准快照
        NetworkComponent->ReplicateAimState(AimState);
        return;
    }

    if (NetworkComponent->IsOwnerLocallyControlled())
    {
        // 连续快照经服务端接收边界重新进入输入系统
        NetworkComponent->ServerSubmitAimState(AimState);
    }
}

void FBBBCharacterNetworkSystem::TransmitLocomotionState(
    const FBBBLocomotionNetworkState &LocomotionState)
{
    if (!NetworkComponent)
    {
        return;
    }

    if (NetworkComponent->IsOwnerAuthority())
    {
        // 权威发布已经稳定的移动快照
        NetworkComponent->ReplicateLocomotionState(LocomotionState);
        return;
    }

    if (NetworkComponent->IsOwnerLocallyControlled())
    {
        // 模拟代理不会进入此分支 因而不会形成转发回路
        NetworkComponent->ServerSubmitLocomotionState(LocomotionState);
    }
}
