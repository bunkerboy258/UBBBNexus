#include "BBBWork/UBBBNexus/Character/Runtime/System/NetworkSystem/BBBCharacterNetworkSystem.h"
#include "BBBWork/UBBBNexus/Character/Core/Config/Network/BBBNetworkConfig.h"
#include "BBBWork/UBBBNexus/Character/Runtime/Controller/LocomotionController/Definition/BBBCharacterLocomotionRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/Runtime/Controller/EquipmentController/Definition/BBBCharacterEquipmentRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/Runtime/State/Definition/BBBCharacterWorldRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/Runtime/System/NetworkSystem/BBBCharacterNetworkComponent.h"
#include "BBBWork/UBBBNexus/Character/Runtime/System/NetworkSystem/Definition/BBBNetworkRuntimeData.h"

void FBBBCharacterNetworkSystem::Initialize(
    FBBBNetworkRuntimeData &InNetworkData,
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

void FBBBCharacterNetworkSystem::UpdateAuthorityLocal()
{
    AuthorityLocalProcessor.Update(*this);
}

void FBBBCharacterNetworkSystem::UpdateAuthorityRemote()
{
    AuthorityRemoteProcessor.Update(*this);
}

void FBBBCharacterNetworkSystem::UpdateClientLocal()
{
    ClientLocalProcessor.Update(*this);
}

void FBBBCharacterNetworkSystem::UpdateClientRemote()
{
    ClientRemoteProcessor.Update(*this);
}

void FBBBCharacterNetworkSystem::ObserveFacts()
{
    if (!ensureMsgf(NetworkData && WorldData && AimData && LocomotionData && NetworkConfig
        && EquipmentState && EquipmentEvents && NetworkComponent, TEXT("[UBBBC]Network dependencies are null")))
    {
        return;
    }

    FactProcessor.Update(*NetworkData, WorldData->GetWorldTimeSeconds(), *AimData, *LocomotionData,
        *NetworkConfig, *EquipmentState, *EquipmentEvents, *this);
}

void FBBBCharacterNetworkSystem::SubmitEquipmentPacket(FBBBEquipmentNetworkPacket Packet)
{
    if (NetworkComponent->IsOwnerAuthority())
    {
        NetworkComponent->MulticastEquipmentPacket(MoveTemp(Packet));
        return;
    }

    NetworkComponent->ServerUploadEquipmentPacket(MoveTemp(Packet));
}

void FBBBCharacterNetworkSystem::SubmitEquipmentActionPacket(FBBBEquipmentActionNetworkPacket Packet)
{
    if (NetworkComponent->IsOwnerAuthority())
    {
        NetworkComponent->MulticastEquipmentActionPacket(MoveTemp(Packet));
        return;
    }

    NetworkComponent->ServerUploadEquipmentActionPacket(MoveTemp(Packet));
}

void FBBBCharacterNetworkSystem::SubmitAimState(const FBBBAimNetworkState &AimState)
{
    if (NetworkComponent->IsOwnerAuthority())
    {
        NetworkComponent->SetReplicatedAimState(AimState);
        return;
    }

    NetworkComponent->ServerSubmitAimState(AimState);
}

void FBBBCharacterNetworkSystem::SubmitLocomotionState(const FBBBLocomotionNetworkState &LocomotionState)
{
    if (NetworkComponent->IsOwnerAuthority())
    {
        NetworkComponent->SetReplicatedLocomotionState(LocomotionState);
        return;
    }

    NetworkComponent->ServerSubmitLocomotionState(LocomotionState);
}
