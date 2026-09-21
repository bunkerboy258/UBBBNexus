#include "BBBWork/UBBBNexus/Character/Runtime/System/NetworkSystem/BBBCharacterNetworkSystem.h"

#include "BBBWork/UBBBNexus/Character/Core/Config/Network/BBBNetworkConfig.h"
#include "BBBWork/UBBBNexus/Character/Network/BBBCharacterNetworkComponent.h"
#include "BBBWork/UBBBNexus/Character/Runtime/RuntimeData/BBBCharacterRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/Runtime/System/NetworkSystem/DomainData/Context/BBBCharacterNetworkUpdateContext.h"

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
    if (!RuntimeData || !NetworkComponent || !NetworkConfig)
    {
        return;
    }

    const FBBBCharacterNetworkIdentityState &NetworkIdentityState =
        RuntimeData->External.ReadNetworkIdentityState();
    if (!NetworkIdentityState.bHasAuthority && !NetworkIdentityState.bLocallyControlled)
    {
        return;
    }

    FBBBCharacterNetworkUpdateContext Context{
        RuntimeData->Network.EquipmentObservationState,
        RuntimeData->Network.AimObservationState,
        RuntimeData->Network.LocomotionObservationState,
        NetworkIdentityState,
        RuntimeData->External.ReadWorldState(),
        RuntimeData->Aim.ReadAimState(),
        RuntimeData->Locomotion.ReadLocomotionState(),
        RuntimeData->Equipment.ReadEquipmentSelectionState(),
        RuntimeData->Equipment.ReadEquipmentEventState(),
        *NetworkConfig,
        *NetworkComponent};

    EquipmentStateObservationProcessor.Update(Context);
    EquipmentFactObservationProcessor.Update(Context);
    AimObservationProcessor.Update(Context);
    LocomotionObservationProcessor.Update(Context);
}
