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
    UBBBEquipmentDefinition *Definition = EquipmentCatalog.FindDefinition(Packet.EquipmentHandle);
    if (!ensureMsgf(Definition, TEXT("[UBBBC]Equipment restore handle has no matching definition")))
    { return; }

    EquipmentCommands.SubmitRestoredEquipment(*Definition);
}
