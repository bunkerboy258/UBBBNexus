#include "BBBWork/UBBBNexus/Character/Logic/System/NetworkSystem/Processors/BBBLocomotionObservationProcessor.h"

#include "BBBWork/UBBBNexus/Character/Network/BBBCharacterNetworkComponent.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/LocomotionSystem/DomainData/States/BBBCharacterLocomotionState.h"
#include "BBBWork/UBBBNexus/Character/Logic/RuntimeData/ExternalDomain/States/BBBCharacterNetworkIdentityState.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/NetworkSystem/DomainData/Context/BBBCharacterNetworkUpdateContext.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/NetworkSystem/DomainData/States/BBBLocomotionNetworkObservationState.h"

void FBBBLocomotionObservationProcessor::Update(
    FBBBCharacterNetworkUpdateContext &Context) const
{
    // 步态变化才需要同步 重复状态由上次观察值拦截
    if (Context.LocomotionObservationState.LastObservedGait.IsSet()
        && Context.LocomotionObservationState.LastObservedGait.GetValue()
        == Context.LocomotionState.Gait)
    {
        return;
    }

    // 变更一经观察便更新基准 避免后续帧重复提交同一值
    Context.LocomotionObservationState.LastObservedGait = Context.LocomotionState.Gait;

    // 权威端更新复制属性 本机控制的非权威端向服务器提交状态
    if (Context.NetworkIdentityState.bHasAuthority)
    {
        Context.NetworkComponent.ReplicateLocomotionState(Context.LocomotionState.Gait);
    }

    if (!Context.NetworkIdentityState.bHasAuthority
        && Context.NetworkIdentityState.bLocallyControlled)
    {
        Context.NetworkComponent.ServerSubmitLocomotionState(Context.LocomotionState.Gait);
    }
}
