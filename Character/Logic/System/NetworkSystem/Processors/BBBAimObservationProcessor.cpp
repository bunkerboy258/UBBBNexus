#include "BBBWork/UBBBNexus/Character/Logic/System/NetworkSystem/Processors/BBBAimObservationProcessor.h"

#include "BBBWork/UBBBNexus/Character/Config/Network/BBBNetworkConfig.h"
#include "BBBWork/UBBBNexus/Character/Network/BBBCharacterNetworkComponent.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/AimSystem/DomainData/States/BBBAimState.h"
#include "BBBWork/UBBBNexus/Character/Logic/RuntimeData/ExternalDomain/States/BBBCharacterNetworkIdentityState.h"
#include "BBBWork/UBBBNexus/Character/Logic/RuntimeData/ExternalDomain/States/BBBCharacterWorldState.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/NetworkSystem/DomainData/Context/BBBCharacterNetworkUpdateContext.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/NetworkSystem/DomainData/States/BBBAimNetworkObservationState.h"

void FBBBAimObservationProcessor::Update(FBBBCharacterNetworkUpdateContext &Context) const
{
    // 只投影本次需要复制的字段 避免观察流程改变领域瞄准状态
    FBBBReplicatedAimState ReplicatedAimState;
    ReplicatedAimState.bIsAiming = Context.AimState.bIsAiming;
    ReplicatedAimState.AimAlpha = Context.AimState.AimAlpha;
    ReplicatedAimState.AimTargetWorld = Context.AimState.AimTargetWorld;

    // 首次观测立即发送 瞄准开关变化优先于周期节流
    bool bShouldTransmit = !Context.AimObservationState.LastObservedAim.IsSet();
    if (Context.AimObservationState.LastObservedAim.IsSet())
    {
        const FBBBReplicatedAimState &Previous = Context.AimObservationState.LastObservedAim.GetValue();
        bShouldTransmit = Previous.bIsAiming != ReplicatedAimState.bIsAiming;

        // 目标位置使用半厘米容差 瞄准权重使用百分之一容差以过滤细微变化
        const bool bIntervalElapsed = Context.WorldState.WorldTimeSeconds
            - Context.AimObservationState.LastAimUploadTime
            >= Context.NetworkConfig.AimUploadInterval;
        const bool bTargetChanged = !FVector(Previous.AimTargetWorld).Equals(
            FVector(ReplicatedAimState.AimTargetWorld),
            0.5f);
        const bool bAimAlphaChanged = !FMath::IsNearlyEqual(
            Previous.AimAlpha,
            ReplicatedAimState.AimAlpha,
            0.01f);
        bShouldTransmit |= bIntervalElapsed && (bTargetChanged || bAimAlphaChanged);
    }

    // 只在发送条件成立时推进快照和时间 保证间隔从上次实际提交计算
    if (!bShouldTransmit)
    {
        return;
    }

    Context.AimObservationState.LastObservedAim = ReplicatedAimState;
    Context.AimObservationState.LastAimUploadTime = Context.WorldState.WorldTimeSeconds;

    // 权威端更新复制属性 本机控制的非权威端向服务器提交状态
    if (Context.NetworkIdentityState.bHasAuthority)
    {
        Context.NetworkComponent.ReplicateAimState(ReplicatedAimState);
    }

    if (!Context.NetworkIdentityState.bHasAuthority
        && Context.NetworkIdentityState.bLocallyControlled)
    {
        Context.NetworkComponent.ServerSubmitAimState(ReplicatedAimState);
    }
}
