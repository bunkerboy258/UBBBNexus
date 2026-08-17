
#include "BBBWork/UBBBNexus/Character/System/NetworkSystem/Processors/Upload/Processors/BBBAimUploadProcessor.h"
#include "BBBWork/UBBBNexus/Character/Core/Config/Network/BBBNetworkConfig.h"
#include "BBBWork/UBBBNexus/Character/System/AimSystem/Definition/BBBAimRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/System/AimSystem/Definition/States/BBBAimStates.h"
#include "BBBWork/UBBBNexus/Character/System/NetworkSystem/BBBCharacterNetworkSystem.h"
#include "BBBWork/UBBBNexus/Character/System/NetworkSystem/Definition/BBBNetworkRuntimeData.h"
namespace
{
    //判断是否满足提交条件(最重要的作用是避免频繁提交卡爆带宽)
bool ShouldSubmitAimState(const FBBBAimNetworkObserverState &Observer, const FBBBAimNetworkState &AimState, const FBBBCharacterNetworkConfig &NetworkConfig, float Now)
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

void FBBBAimUploadProcessor::Update(
    const FBBBAimRuntimeData &AimData,
    const FBBBCharacterNetworkConfig &NetworkConfig,
    float WorldTimeSeconds,
    FBBBNetworkRuntimeData &NetworkData,
    FBBBCharacterNetworkSystem &NetworkSystem) const
{

    //上次上传的瞄准状态数据
    FBBBAimNetworkObserverState Observer = NetworkData.GetAimObserverState();

    FBBBAimNetworkState AimState;

    const FBBBAimRuntimeState &State = AimData.GetState();

    AimState.bIsAiming = State.bIsAiming;
    AimState.AimTargetWorld = State.AimTargetWorld;

    const float Now = WorldTimeSeconds;

    if (!ShouldSubmitAimState(Observer, AimState, NetworkConfig, Now))
    { return; }

    Observer.LastObservedState = AimState;

    Observer.LastUploadTime = Now;

    NetworkData.CommitAimObserverState(Observer);

    NetworkSystem.SubmitAimState(AimState);
}
