#include "BBBWork/UBBBNexus/Character/Logic/System/NetworkSystem/Processors/BBBSlideObservationProcessor.h"

#include "BBBWork/UBBBNexus/Character/Network/BBBCharacterNetworkComponent.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/LocomotionSystem/DomainData/States/BBBCharacterLocomotionState.h"
#include "BBBWork/UBBBNexus/Character/Logic/RuntimeData/ExternalDomain/States/BBBCharacterNetworkIdentityState.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/NetworkSystem/DomainData/Context/BBBCharacterNetworkUpdateContext.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/NetworkSystem/DomainData/States/BBBSlideNetworkObservationState.h"

void FBBBSlideObservationProcessor::Update(
    FBBBCharacterNetworkUpdateContext &Context) const
{
    // 滑铲状态变化才需要同步 重复状态由上次观察值拦截
    if (Context.SlideObservationState.LastObservedSlide.IsSet()
        && Context.SlideObservationState.LastObservedSlide.GetValue()
        == Context.LocomotionState.bSlide)
    {
        return;
    }

    // 变更一经观察便更新基准 避免后续帧重复提交同一值
    Context.SlideObservationState.LastObservedSlide = Context.LocomotionState.bSlide;

    // 权威端更新复制属性 本机控制的非权威端向服务器提交状态
    if (Context.NetworkIdentityState.bHasAuthority)
    {
        Context.NetworkComponent.ReplicateSlideState(Context.LocomotionState.bSlide);
    }

    if (!Context.NetworkIdentityState.bHasAuthority
        && Context.NetworkIdentityState.bLocallyControlled)
    {
        Context.NetworkComponent.ServerSubmitSlideState(Context.LocomotionState.bSlide);
    }
}
