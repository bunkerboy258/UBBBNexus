#include "BBBWork/UBBBNexus/Character/System/NetworkSystem/Processors/Restore/Processors/BBBEquipmentActionRestoreProcessor.h"

#include "BBBWork/UBBBNexus/Character/System/EquipmentSystem/Definition/Commands/BBBCharacterEquipmentCommands.h"
#include "BBBWork/UBBBNexus/Character/System/NetworkSystem/Definition/Packets/BBBEquipmentActionNetworkPacket.h"

void FBBBEquipmentActionRestoreProcessor::Update(
    FBBBCharacterEquipmentCommands &EquipmentCommands,
    const FBBBEquipmentActionNetworkPacket &Packet) const
{
    FBBBEquipmentActionEvent Event;
    Event.ActionType = Packet.ActionType;
    Event.EquipmentId = Packet.EquipmentId;
    Event.Sequence = Packet.Sequence;
    Event.DurationSeconds = Packet.DurationSeconds;
    EquipmentCommands.SubmitRestoredAction(MoveTemp(Event));
}
