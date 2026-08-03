#include "BBBWork/UBBBNexus/Character/System/NetworkSystem/Processors/Upload/Processors/BBBEquipmentUploadProcessor.h"
#include "BBBWork/UBBBNexus/Character/System/NetworkSystem/BBBCharacterNetworkComponent.h"
#include "BBBWork/UBBBNexus/Character/System/ItemSystem/Definition/BBBCharacterItemState.h"
#include "BBBWork/UBBBNexus/Character/System/NetworkSystem/Definition/BBBNetworkRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/System/NetworkSystem/Definition/Packets/BBBEquipmentNetworkPacket.h"
#include "BBBWork/UBBBNexus/Item/Base/BBBItemDefinition.h"

void FBBBEquipmentUploadProcessor::Update(
    const FBBBCharacterEquipmentState &EquipmentState,
    FBBBNetworkRuntimeData &NetworkData,
    UBBBCharacterNetworkComponent &NetworkComponent) const
{
    const FBBBItemInstance &ActiveItem = EquipmentState.GetActiveMainHandItem();

    if (!ActiveItem.IsValid())
    {
        return;
    }

    if (NetworkData.GetLastUploadedItemInstanceId() == ActiveItem.InstanceId)
    {
        return;
    }

    FBBBEquipmentNetworkPacket Packet;
    Packet.EquipmentHandle = ActiveItem.Definition->ItemId;

    NetworkComponent.SendEquipmentPacket(Packet);

    NetworkData.CommitLastUploadedItemInstanceId(ActiveItem.InstanceId);
}
