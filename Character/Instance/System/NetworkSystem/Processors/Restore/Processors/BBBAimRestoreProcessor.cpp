
#include "BBBWork/UBBBNexus/Character/Instance/System/NetworkSystem/Processors/Restore/Processors/BBBAimRestoreProcessor.h"
#include "BBBWork/UBBBNexus/Character/Instance/System/AimSystem/Definition/BBBAimRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/Instance/System/NetworkSystem/Definition/States/BBBNetworkStates.h"

void FBBBAimRestoreProcessor::Update(
    FBBBAimRuntimeData &AimData,
    const FBBBAimNetworkState &AimState) const
{

    // 将网络瞄准状态转换为本地远端表现状态
    FBBBRemoteAimState RemoteState;
    RemoteState.bIsAiming = AimState.bIsAiming;
    RemoteState.AimTargetWorld = AimState.AimTargetWorld;

    // 提交远端瞄准状态供表现系统读取
    AimData.RestoreRemoteState(RemoteState);
}
