#include "BBBWork/UBBBNexus/Character/System/NetworkSystem/Processors/Upload/Processors/BBBEquipmentUploadProcessor.h"
#include "BBBWork/UBBBNexus/Character/System/NetworkSystem/BBBCharacterNetworkSystem.h"
#include "BBBWork/UBBBNexus/Character/System/EquipmentSystem/Definition/States/BBBCharacterEquipmentStates.h"
#include "BBBWork/UBBBNexus/Character/System/NetworkSystem/Definition/BBBNetworkRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/System/NetworkSystem/Definition/Packets/BBBEquipmentNetworkPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Base/BBBEquipmentDefinition.h"
#include "BBBWork/UBBBNexus/Equipment/Base/BBBEquipmentInstance.h"

void FBBBEquipmentUploadProcessor::Update(
    const FBBBCharacterEquipmentState &EquipmentState,
    FBBBNetworkRuntimeData &NetworkData,
    FBBBCharacterNetworkSystem &NetworkSystem) const
{
    UBBBEquipmentInstance *ActiveEquipment = EquipmentState.GetActiveMainHandInstance();

    if (!ActiveEquipment)
    { return; }

    if (NetworkData.GetLastUploadedEquipmentInstanceId() == ActiveEquipment->GetInstanceId())
    { return; }

    FBBBEquipmentNetworkPacket Packet;
    Packet.EquipmentHandle = ActiveEquipment->GetDefinition()->EquipmentId;

    NetworkSystem.SubmitEquipmentPacket(MoveTemp(Packet));

    NetworkData.CommitLastUploadedEquipmentInstanceId(ActiveEquipment->GetInstanceId());
}
