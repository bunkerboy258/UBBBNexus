#include "BBBWork/UBBBNexus/Character/Instance/System/NetworkSystem/Processors/Restore/Processors/BBBEquipmentActionRestoreProcessor.h"

#include "BBBWork/UBBBNexus/Character/Instance/System/EquipmentSystem/Definition/Commands/BBBCharacterEquipmentCommands.h"
#include "BBBWork/UBBBNexus/Character/Instance/System/NetworkSystem/Definition/Packets/BBBEquipmentActionNetworkPacket.h"

void FBBBEquipmentActionRestoreProcessor::Update(
    FBBBCharacterEquipmentCommands &EquipmentCommands,
    const FBBBEquipmentActionNetworkPacket &Packet) const
{
    // 将网络动作包还原为装备动作事实
    FBBBEquipmentActionEvent Event;
    Event.ActionType = Packet.ActionType;
    Event.EquipmentId = Packet.EquipmentId;
    Event.Sequence = Packet.Sequence;
    Event.Phase = Packet.Phase;
    Event.LoadedAmmo = Packet.LoadedAmmo;
    // 统一进入装备命令队列等待当前装备消费
    EquipmentCommands.SubmitRestoredAction(MoveTemp(Event));
}
