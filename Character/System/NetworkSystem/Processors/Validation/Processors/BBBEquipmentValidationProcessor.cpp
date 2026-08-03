#include "BBBWork/UBBBNexus/Character/System/NetworkSystem/Processors/Validation/Processors/BBBEquipmentValidationProcessor.h"
#include "BBBWork/UBBBNexus/Character/System/NetworkSystem/Context/Packets/BBBEquipmentNetworkPacket.h"

bool FBBBEquipmentValidationProcessor::Update(const FBBBEquipmentNetworkPacket &Packet) const
{
    return !Packet.EquipmentHandle.IsNone();
}
