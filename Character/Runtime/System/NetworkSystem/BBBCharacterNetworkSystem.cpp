#include "BBBWork/UBBBNexus/Character/Runtime/System/NetworkSystem/BBBCharacterNetworkSystem.h"

#include "BBBWork/UBBBNexus/Character/Core/Config/Network/BBBNetworkConfig.h"
#include "BBBWork/UBBBNexus/Character/Runtime/Controller/EquipmentController/Definition/BBBCharacterEquipmentRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/Runtime/Controller/LocomotionController/Definition/BBBCharacterLocomotionRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/Runtime/State/Definition/BBBCharacterWorldRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/Runtime/System/NetworkSystem/BBBCharacterNetworkComponent.h"
#include "BBBWork/UBBBNexus/Character/Runtime/System/NetworkSystem/State/BBBNetworkState.h"

void FBBBCharacterNetworkSystem::Initialize(
    FBBBNetworkState &InNetworkData,
    FBBBAimRuntimeData &InAimData,
    FBBBCharacterLocomotionRuntimeData &InLocomotionData,
    const FBBBCharacterEquipmentState &InEquipmentState,
    UBBBCharacterNetworkComponent &InNetworkComponent,
    const FBBBCharacterWorldRuntimeData &InWorldData,
    const FBBBCharacterEquipmentEvents &InEquipmentEvents,
    const FBBBCharacterNetworkConfig &InNetworkConfig)
{
    NetworkData = &InNetworkData;
    AimData = &InAimData;
    LocomotionData = &InLocomotionData;
    EquipmentState = &InEquipmentState;
    NetworkComponent = &InNetworkComponent;
    WorldData = &InWorldData;
    EquipmentEvents = &InEquipmentEvents;
    NetworkConfig = &InNetworkConfig;
}

void FBBBCharacterNetworkSystem::Update()
{
    if (!ensureMsgf(NetworkComponent,
        TEXT("[UBBBC]Network system update failed because core dependencies are null")))
    {
        return;
    }

    if (NetworkComponent->IsOwnerAuthority() || NetworkComponent->IsOwnerLocallyControlled())
    {
        ObserveFacts();
    }
}

void FBBBCharacterNetworkSystem::ObserveFacts()
{
    if (!ensureMsgf(NetworkData && WorldData && AimData && LocomotionData && NetworkConfig
        && EquipmentState && EquipmentEvents && NetworkComponent,
        TEXT("[UBBBC]Authority fact observation failed because dependencies are null")))
    {
        return;
    }

    FactProcessor.Update(
        *NetworkData,
        WorldData->GetWorldTimeSeconds(),
        *AimData,
        *LocomotionData,
        *NetworkConfig,
        *EquipmentState,
        *EquipmentEvents,
        *this);
}

void FBBBCharacterNetworkSystem::SubmitEquipmentFact(FBBBEquipmentActionFact Fact)
{
    if (!ensureMsgf(NetworkComponent, TEXT("[UBBBC]Equipment fact has no network component")))
    {
        return;
    }

    if (NetworkComponent->IsOwnerAuthority())
    {
        NetworkComponent->PublishEquipmentFact(MoveTemp(Fact));
        return;
    }

    if (NetworkComponent->IsOwnerLocallyControlled())
    {
        NetworkComponent->ServerSubmitEquipmentFact(MoveTemp(Fact));
    }
}

void FBBBCharacterNetworkSystem::SubmitEquipmentState(const FName EquipmentId)
{
    if (!ensureMsgf(NetworkComponent, TEXT("[UBBBC]Equipment state has no network component")))
    {
        return;
    }

    if (NetworkComponent->IsOwnerAuthority())
    {
        NetworkComponent->PublishEquipmentState(EquipmentId);
        return;
    }

    if (NetworkComponent->IsOwnerLocallyControlled())
    {
        NetworkComponent->ServerSubmitEquipmentState(EquipmentId);
    }
}

void FBBBCharacterNetworkSystem::SubmitAimState(const FBBBAimNetworkState &AimState)
{
    if (!ensureMsgf(NetworkComponent, TEXT("[UBBBC]Aim state has no network component")))
    {
        return;
    }

    if (NetworkComponent->IsOwnerAuthority())
    {
        NetworkComponent->PublishAimState(AimState);
        return;
    }

    if (NetworkComponent->IsOwnerLocallyControlled())
    {
        NetworkComponent->ServerSubmitAimState(AimState);
    }
}

void FBBBCharacterNetworkSystem::SubmitLocomotionState(
    const FBBBLocomotionNetworkState &LocomotionState)
{
    if (!ensureMsgf(NetworkComponent, TEXT("[UBBBC]Locomotion state has no network component")))
    {
        return;
    }

    if (NetworkComponent->IsOwnerAuthority())
    {
        NetworkComponent->PublishLocomotionState(LocomotionState);
        return;
    }

    if (NetworkComponent->IsOwnerLocallyControlled())
    {
        NetworkComponent->ServerSubmitLocomotionState(LocomotionState);
    }
}
