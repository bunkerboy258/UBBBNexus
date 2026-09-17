
#include "BBBWork/UBBBNexus/Character/Instance/System/NetworkSystem/Processors/Restore/Processors/BBBAimRestoreProcessor.h"
#include "BBBWork/UBBBNexus/Character/Instance/System/AimSystem/Definition/BBBAimRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/Instance/System/NetworkSystem/Definition/States/BBBNetworkStates.h"

void FBBBAimRestoreProcessor::Update(
    FBBBAimRuntimeData &AimData,
    const FBBBAimNetworkState &AimState) const
{

    FBBBRemoteAimState RemoteState;
    RemoteState.bIsAiming = AimState.bIsAiming;
    RemoteState.AimTargetWorld = AimState.AimTargetWorld;

    AimData.RestoreRemoteState(RemoteState);
}
