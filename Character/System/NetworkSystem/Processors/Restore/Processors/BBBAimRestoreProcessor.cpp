
#include "BBBWork/UBBBNexus/Character/System/NetworkSystem/Processors/Restore/Processors/BBBAimRestoreProcessor.h"
#include "BBBWork/UBBBNexus/Character/System/AimSystem/Context/BBBAimRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/System/NetworkSystem/Context/BBBNetworkStates.h"

void FBBBAimRestoreProcessor::Update(
    FBBBAimRuntimeData &AimData,
    const FBBBAimNetworkState &AimState) const
{

    FBBBRemoteAimState RemoteState;
    RemoteState.bIsAiming = AimState.bIsAiming;
    RemoteState.AimTargetWorld = AimState.AimIKTargetWorld;

    AimData.RestoreRemoteState(RemoteState);
}
