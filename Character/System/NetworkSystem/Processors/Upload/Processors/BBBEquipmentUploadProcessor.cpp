#include "BBBWork/UBBBNexus/Character/System/NetworkSystem/Processors/Upload/Processors/BBBEquipmentUploadProcessor.h"
#include "BBBWork/UBBBNexus/Character/System/NetworkSystem/BBBCharacterNetworkComponent.h"
#include "BBBWork/UBBBNexus/Character/System/ItemSystem/Definition/BBBCharacterItemState.h"
#include "BBBWork/UBBBNexus/Character/System/NetworkSystem/Definition/BBBNetworkRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/System/NetworkSystem/Definition/Packets/BBBEquipmentNetworkPacket.h"

void FBBBEquipmentUploadProcessor::Update(
    const FBBBCharacterEquipmentState &EquipmentState,
    FBBBNetworkRuntimeData &NetworkData,
    UBBBCharacterNetworkComponent &NetworkComponent) const
{
    const FBBBItemInstance &DesiredItem = EquipmentState.GetDesiredMainHandItem();

    if (!DesiredItem.IsValid())
    {
        return;
    }

    if (NetworkData.GetLastUploadedItemInstanceId() == DesiredItem.InstanceId)
    {
        return;
    }

    FBBBEquipmentNetworkPacket Packet;
    Packet.ItemInstanceId = DesiredItem.InstanceId;
    Packet.ItemDefinition = DesiredItem.Definition;

    NetworkComponent.SendEquipmentPacket(Packet);

    NetworkData.CommitLastUploadedItemInstanceId(DesiredItem.InstanceId);
}
