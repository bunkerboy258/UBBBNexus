#include "BBBWork/UBBBNexus/Character/System/NetworkSystem/Processors/Upload/Processors/BBBEquipmentActionUploadProcessor.h"

#include "BBBWork/UBBBNexus/Character/System/EquipmentSystem/Definition/Events/BBBCharacterEquipmentEvents.h"
#include "BBBWork/UBBBNexus/Character/System/NetworkSystem/BBBCharacterNetworkSystem.h"
#include "BBBWork/UBBBNexus/Character/System/NetworkSystem/Definition/Packets/BBBEquipmentActionNetworkPacket.h"

void FBBBEquipmentActionUploadProcessor::Update(
    const FBBBCharacterEquipmentEvents &EquipmentEvents,
    FBBBCharacterNetworkSystem &NetworkSystem) const
{
    for (const FBBBEquipmentActionEvent &Event : EquipmentEvents.GetActionEvents())
    {
        FBBBEquipmentActionNetworkPacket Packet;
        Packet.ActionType = Event.ActionType;
        Packet.EquipmentId = Event.EquipmentId;
        Packet.Sequence = Event.Sequence;
        Packet.DurationSeconds = Event.DurationSeconds;
        NetworkSystem.SubmitEquipmentActionPacket(MoveTemp(Packet));
    }
}
