#include "BBBWork/UBBBNexus/Character/System/NetworkSystem/Processors/Upload/Processors/BBBEquipmentUploadProcessor.h"
#include "BBBWork/UBBBNexus/Character/System/NetworkSystem/BBBCharacterNetworkComponent.h"
#include "BBBWork/UBBBNexus/Character/System/ItemSystem/Definition/BBBCharacterItemState.h"
#include "BBBWork/UBBBNexus/Character/System/NetworkSystem/Definition/BBBNetworkRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/System/NetworkSystem/Definition/Packets/BBBEquipmentNetworkPacket.h"
#include "BBBWork/UBBBNexus/Item/Base/BBBItemDefinition.h"
#include "BBBWork/UBBBNexus/Item/Base/Equipment/BBBEquipmentInstance.h"

void FBBBEquipmentUploadProcessor::Update(
    const FBBBCharacterEquipmentState &EquipmentState,
    FBBBNetworkRuntimeData &NetworkData,
    UBBBCharacterNetworkComponent &NetworkComponent) const
{
    UBBBEquipmentInstance *ActiveItem = EquipmentState.GetActiveMainHandInstance();

    if (!ActiveItem || !ActiveItem->IsValid())
    { return; }

    if (NetworkData.GetLastUploadedItemInstanceId() == ActiveItem->GetInstanceId())
    { return; }

    FBBBEquipmentNetworkPacket Packet;
    Packet.EquipmentHandle = ActiveItem->GetDefinition()->ItemId;

    NetworkComponent.SendEquipmentPacket(Packet);

    NetworkData.CommitLastUploadedItemInstanceId(ActiveItem->GetInstanceId());
}
