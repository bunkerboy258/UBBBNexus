#include "BBBWork/UBBBNexus/Character/Instance/System/NetworkSystem/Processors/Restore/Processors/BBBEquipmentActionRestoreProcessor.h"

#include "BBBWork/UBBBNexus/Character/Instance/System/EquipmentSystem/Definition/Commands/BBBCharacterEquipmentCommands.h"
#include "BBBWork/UBBBNexus/Character/Instance/System/NetworkSystem/Definition/Packets/BBBEquipmentActionNetworkPacket.h"

void FBBBEquipmentActionRestoreProcessor::Update(
    FBBBCharacterEquipmentCommands &EquipmentCommands,
    const FBBBEquipmentActionNetworkPacket &Packet) const
{
    FBBBEquipmentActionEvent Event;
    Event.ActionType = Packet.ActionType;
    Event.EquipmentId = Packet.EquipmentId;
    Event.Sequence = Packet.Sequence;
    Event.Phase = Packet.Phase;
    Event.LoadedAmmo = Packet.LoadedAmmo;
    EquipmentCommands.SubmitRestoredAction(MoveTemp(Event));
}
