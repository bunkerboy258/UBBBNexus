
#include "BBBWork/UBBBNexus/Character/Runtime/System/NetworkSystem/Processors/BBBAimObservationProcessor.h"
#include "BBBWork/UBBBNexus/Character/Core/Config/Network/BBBNetworkConfig.h"
#include "BBBWork/UBBBNexus/Character/Runtime/Controller/AimController/Definition/BBBAimRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/Runtime/Controller/AimController/Definition/States/BBBAimStates.h"
#include "BBBWork/UBBBNexus/Character/Runtime/System/NetworkSystem/BBBCharacterNetworkSystem.h"
#include "BBBWork/UBBBNexus/Character/Runtime/System/NetworkSystem/State/BBBNetworkState.h"
namespace
{
    //判断是否满足提交条件(最重要的作用是避免频繁提交卡爆带宽)
bool ShouldTransmitAimState(
    const FBBBAimNetworkObserverState &Observer,
    const FBBBAimNetworkState &AimState,
    const FBBBCharacterNetworkConfig &NetworkConfig,
    float Now)
{

    //还没记录过直接返回
    if (!Observer.LastObservedState.IsSet())
    {
        return true;
    }

    const FBBBAimNetworkState &Previous = Observer.LastObservedState.GetValue();

    //如果是瞄准的行为变化 立即提交
    if (Previous.bIsAiming != AimState.bIsAiming)
    {
        return true;
    }

    //上传间隔不足时跳过本次提交
    if (Now - Observer.LastUploadTime < NetworkConfig.AimUploadInterval)
    {
        return false;
    }

    //目标点变化超过阈值时提交
    if (!FVector(Previous.AimTargetWorld).Equals(FVector(AimState.AimTargetWorld), 0.5f))
    {
        return true;
    }
    return false;
}
}

void FBBBAimObservationProcessor::Update(
    const FBBBAimRuntimeData &AimData,
    const FBBBCharacterNetworkConfig &NetworkConfig,
    float WorldTimeSeconds,
    FBBBNetworkState &NetworkData,
    FBBBCharacterNetworkSystem &NetworkSystem) const
{

    // 读取上次观察结果用于判断本次是否需要上传
    //上次上传的瞄准状态数据
    FBBBAimNetworkObserverState Observer = NetworkData.GetAimObserverState();

    FBBBAimNetworkState AimState;

    const FBBBAimRuntimeState &State = AimData.GetState();

    AimState.bIsAiming = State.bIsAiming;
    AimState.AimTargetWorld = State.AimTargetWorld;

    // 状态未达到提交条件时保持上次观察结果
    const float Now = WorldTimeSeconds;

    if (!ShouldTransmitAimState(Observer, AimState, NetworkConfig, Now))
    {
        return;
    }

    Observer.LastObservedState = AimState;

    Observer.LastUploadTime = Now;

    // 先记录本次观察结果再提交网络状态
    NetworkData.CommitAimObserverState(Observer);

    NetworkSystem.TransmitAimState(AimState);
}
