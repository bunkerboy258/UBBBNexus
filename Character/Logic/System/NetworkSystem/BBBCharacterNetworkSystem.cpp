#include "BBBWork/UBBBNexus/Character/Logic/System/NetworkSystem/BBBCharacterNetworkSystem.h"

#include "BBBWork/UBBBNexus/Character/Config/Network/BBBNetworkConfig.h"
#include "BBBWork/UBBBNexus/Character/Network/BBBCharacterNetworkComponent.h"
#include "BBBWork/UBBBNexus/Character/Logic/RuntimeData/BBBCharacterRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/NetworkSystem/DomainData/Context/BBBCharacterNetworkUpdateContext.h"

void FBBBCharacterNetworkSystem::Initialize(
    FBBBCharacterRuntimeData &InRuntimeData,
    UBBBCharacterNetworkComponent &InNetworkComponent,
    const FBBBCharacterNetworkConfig &InNetworkConfig)
{
    RuntimeData = &InRuntimeData;
    NetworkComponent = &InNetworkComponent;
    NetworkConfig = &InNetworkConfig;
}

void FBBBCharacterNetworkSystem::Update()
{
    // 依赖尚未完成装配时保持无副作用 避免访问未绑定的运行时数据或传输组件
    if (!RuntimeData || !NetworkComponent || !NetworkConfig)
    {
        return;
    }

    const FBBBCharacterNetworkIdentityState &NetworkIdentityState =
        RuntimeData->External.ReadNetworkIdentityState();

    // 远程模拟代理只消费网络组件投递的输入 不参与本机网络观察和发送
    if (!NetworkIdentityState.bHasAuthority && !NetworkIdentityState.bLocallyControlled)
    {
        return;
    }

    // 上下文只聚合已经生成的黑板状态 网络系统只观察状态不生成玩法事实
    FBBBCharacterNetworkUpdateContext Context{
        RuntimeData->Network.AimObservationState,
        RuntimeData->Network.LocomotionObservationState,
        NetworkIdentityState,
        RuntimeData->External.ReadWorldState(),
        RuntimeData->Aim.ReadAimState(),
        RuntimeData->Locomotion.ReadLocomotionState(),
        *NetworkConfig,
        *NetworkComponent};

    // 固定处理顺序先观察瞄准状态再观察移动步态
    AimObservationProcessor.Update(Context);
    LocomotionObservationProcessor.Update(Context);
}
