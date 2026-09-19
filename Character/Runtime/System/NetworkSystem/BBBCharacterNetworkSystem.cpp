#include "BBBWork/UBBBNexus/Character/Runtime/System/NetworkSystem/BBBCharacterNetworkSystem.h"

#include "BBBWork/UBBBNexus/Character/Core/Config/Network/BBBNetworkConfig.h"
#include "BBBWork/UBBBNexus/Character/Runtime/System/ParseSystem/Context/BBBCharacterInputFrame.h"
#include "BBBWork/UBBBNexus/Character/Runtime/Controller/EquipmentController/Definition/BBBCharacterEquipmentRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/Runtime/Controller/LocomotionController/Definition/BBBCharacterLocomotionRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/Runtime/State/Definition/BBBCharacterWorldRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/Runtime/System/NetworkSystem/BBBCharacterNetworkComponent.h"
#include "BBBWork/UBBBNexus/Character/Runtime/System/NetworkSystem/Definition/BBBNetworkRuntimeData.h"

void FBBBCharacterNetworkSystem::Initialize(
    FBBBNetworkRuntimeData &InNetworkData,
    FBBBCharacterInputFrame &InInputFrame,
    FBBBAimRuntimeData &InAimData,
    FBBBCharacterLocomotionRuntimeData &InLocomotionData,
    const FBBBCharacterEquipmentState &InEquipmentState,
    UBBBCharacterNetworkComponent &InNetworkComponent,
    const FBBBCharacterWorldRuntimeData &InWorldData,
    const FBBBCharacterEquipmentEvents &InEquipmentEvents,
    const FBBBCharacterNetworkConfig &InNetworkConfig)
{
    NetworkData = &InNetworkData;
    InputFrame = &InInputFrame;
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
    if (!ensureMsgf(NetworkComponent && InputFrame,
        TEXT("[UBBBC]Network system update failed because core dependencies are null")))
    {
        return;
    }

    if (NetworkComponent->IsOwnerAuthority())
    {
        ObserveFacts();
        return;
    }

    if (NetworkComponent->IsOwnerLocallyControlled())
    {
        CommandProcessor.Update(*InputFrame, *NetworkComponent);
        return;
    }

    RemoteProcessor.Update();
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

void FBBBCharacterNetworkSystem::SubmitEquipmentPacket(FBBBEquipmentNetworkPacket Packet)
{
    if (!ensureMsgf(NetworkComponent && NetworkComponent->IsOwnerAuthority(),
        TEXT("[UBBBC]Only authority may publish equipment facts")))
    {
        return;
    }

    NetworkComponent->MulticastEquipmentPacket(MoveTemp(Packet));
}

void FBBBCharacterNetworkSystem::SubmitEquipmentActionPacket(FBBBEquipmentActionNetworkPacket Packet)
{
    if (!ensureMsgf(NetworkComponent && NetworkComponent->IsOwnerAuthority(),
        TEXT("[UBBBC]Only authority may publish equipment action facts")))
    {
        return;
    }

    NetworkComponent->MulticastEquipmentActionPacket(MoveTemp(Packet));
}

void FBBBCharacterNetworkSystem::SubmitAimState(const FBBBAimNetworkState &AimState)
{
    if (!ensureMsgf(NetworkComponent && NetworkComponent->IsOwnerAuthority(),
        TEXT("[UBBBC]Only authority may publish aim state")))
    {
        return;
    }

    NetworkComponent->SetReplicatedAimState(AimState);
}

void FBBBCharacterNetworkSystem::SubmitLocomotionState(
    const FBBBLocomotionNetworkState &LocomotionState)
{
    if (!ensureMsgf(NetworkComponent && NetworkComponent->IsOwnerAuthority(),
        TEXT("[UBBBC]Only authority may publish locomotion state")))
    {
        return;
    }

    NetworkComponent->SetReplicatedLocomotionState(LocomotionState);
}
