#include "BBBWork/UBBBNexus/Character/Instance/System/NetworkSystem/Processors/Upload/Processors/BBBEquipmentUploadProcessor.h"
#include "BBBWork/UBBBNexus/Character/Instance/System/NetworkSystem/BBBCharacterNetworkSystem.h"
#include "BBBWork/UBBBNexus/Character/Instance/System/EquipmentSystem/Definition/States/BBBCharacterEquipmentStates.h"
#include "BBBWork/UBBBNexus/Character/Instance/System/NetworkSystem/Definition/BBBNetworkRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/Instance/System/NetworkSystem/Definition/Packets/BBBEquipmentNetworkPacket.h"
#include "BBBWork/UBBBNexus/Equipment/BBBEquipment.h"

void FBBBEquipmentUploadProcessor::Update(
    const FBBBCharacterEquipmentState &EquipmentState,
    FBBBNetworkRuntimeData &NetworkData,
    FBBBCharacterNetworkSystem &NetworkSystem) const
{
    ABBBEquipment *ActiveEquipment = EquipmentState.GetActiveMainHandInstance();

    if (!ActiveEquipment)
    { return; }

    if (NetworkData.GetLastUploadedEquipmentInstanceId() == ActiveEquipment->GetInstanceId())
    { return; }

    FBBBEquipmentNetworkPacket Packet;
    Packet.EquipmentHandle = ActiveEquipment->GetEquipmentId();

    NetworkSystem.SubmitEquipmentPacket(MoveTemp(Packet));

    NetworkData.CommitLastUploadedEquipmentInstanceId(ActiveEquipment->GetInstanceId());
}
