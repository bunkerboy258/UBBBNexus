#include "BBBWork/UBBBNexus/Character/Instance/System/NetworkSystem/Processors/Restore/Processors/BBBEquipmentRestoreProcessor.h"
#include "BBBWork/UBBBNexus/Character/Instance/System/EquipmentSystem/Definition/Commands/BBBCharacterEquipmentCommands.h"
#include "BBBWork/UBBBNexus/Character/Instance/System/NetworkSystem/Definition/Packets/BBBEquipmentNetworkPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Core/Config/BBBEquipmentDefinition.h"
#include "BBBWork/UBBBNexus/Equipment/Catalog/BBBEquipmentCatalog.h"

void FBBBEquipmentRestoreProcessor::Update(
    FBBBCharacterEquipmentCommands &EquipmentCommands,
    const FBBBEquipmentNetworkPacket &Packet,
    UBBBEquipmentCatalog &EquipmentCatalog) const
{
    // 根据网络句柄解析本地装备定义
    UBBBEquipmentDefinition *Definition = EquipmentCatalog.FindDefinition(Packet.EquipmentHandle);
    if (!ensureMsgf(Definition, TEXT("[UBBBC]Equipment restore handle has no matching definition")))
    { return; }

    // 将解析后的定义交给装备系统创建恢复实例
    EquipmentCommands.SubmitRestoredEquipment(*Definition);
}
