
#include "BBBWork/UBBBNexus/Character/System/NetworkSystem/Processors/Upload/Processors/BBBAimUploadProcessor.h"
#include "BBBWork/UBBBNexus/Character/Core/Config/Aim/BBBAimConfig.h"
#include "BBBWork/UBBBNexus/Character/System/AimSystem/Context/BBBAimRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/System/AimSystem/Context/BBBAimStates.h"
#include "BBBWork/UBBBNexus/Character/System/NetworkSystem/BBBCharacterNetworkComponent.h"
#include "BBBWork/UBBBNexus/Character/System/NetworkSystem/Context/BBBNetworkRuntimeData.h"
namespace
{
    //判断是否满足提交条件(最重要的作用是避免频繁提交卡爆带宽)
bool ShouldSubmitAimState(const FBBBAimNetworkObserverState &Observer, const FBBBAimNetworkState &AimState, const FBBBAimConfig &AimConfig, float Now)
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
    if (Now - Observer.LastUploadTime < AimConfig.AimUploadInterval)
    {
        return false;
    }

    //目标点变化超过阈值时提交
    if (!FVector(Previous.AimIKTargetWorld).Equals(FVector(AimState.AimIKTargetWorld), 0.5f))
    {
        return true;
    }
    return false;
}
}

void FBBBAimUploadProcessor::Update(
    const FBBBAimRuntimeData &AimData,
    const FBBBAimConfig &AimConfig,
    float WorldTimeSeconds,
    FBBBNetworkRuntimeData &NetworkData,
    UBBBCharacterNetworkComponent &NetworkComponent) const
{

    //上次上传的瞄准状态数据
    FBBBAimNetworkObserverState Observer = NetworkData.GetAimObserverState();

    FBBBAimNetworkState AimState;

    const FBBBAimRuntimeState &State = AimData.GetState();

    AimState.bIsAiming = State.bIsAiming;
    AimState.AimIKTargetWorld = State.AimIKTargetWorld;

    const float Now = WorldTimeSeconds;

    if (!ShouldSubmitAimState(Observer, AimState, AimConfig, Now))
    { return; }

    Observer.LastObservedState = AimState;

    Observer.LastUploadTime = Now;

    NetworkData.CommitAimObserverState(Observer);

    NetworkComponent.SendAimState(AimState);
}
