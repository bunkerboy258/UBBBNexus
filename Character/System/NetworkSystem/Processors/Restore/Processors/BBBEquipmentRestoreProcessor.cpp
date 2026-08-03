#include "BBBWork/UBBBNexus/Character/System/NetworkSystem/Processors/Restore/Processors/BBBEquipmentRestoreProcessor.h"
#include "BBBWork/UBBBNexus/Character/System/NetworkSystem/Definition/Packets/BBBEquipmentNetworkPacket.h"
#include "BBBWork/UBBBNexus/Character/System/ItemSystem/Definition/BBBCharacterItemState.h"

void FBBBEquipmentRestoreProcessor::Update(
    FBBBCharacterEquipmentState &EquipmentState,
    const FBBBEquipmentNetworkPacket &Packet) const
{
    EquipmentState.RestoreDesiredMainHandMirror(Packet.EquipmentHandle);
}
