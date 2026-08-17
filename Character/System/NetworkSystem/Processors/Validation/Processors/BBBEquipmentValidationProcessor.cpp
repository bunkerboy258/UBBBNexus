#include "BBBWork/UBBBNexus/Character/System/NetworkSystem/Processors/Validation/Processors/BBBEquipmentValidationProcessor.h"
#include "BBBWork/UBBBNexus/Character/System/NetworkSystem/Definition/Packets/BBBEquipmentNetworkPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Catalog/BBBEquipmentCatalog.h"

bool FBBBEquipmentValidationProcessor::Update(
    const FBBBEquipmentNetworkPacket &Packet,
    const UBBBEquipmentCatalog &EquipmentCatalog) const
{
    if (Packet.EquipmentHandle.IsNone())
    {
        return false;
    }

    return EquipmentCatalog.FindDefinition(Packet.EquipmentHandle) != nullptr;
}
