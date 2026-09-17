#include "BBBWork/UBBBNexus/Character/Instance/System/NetworkSystem/Processors/Upload/Processors/BBBEquipmentActionUploadProcessor.h"

#include "BBBWork/UBBBNexus/Character/Instance/System/EquipmentSystem/Definition/Events/BBBCharacterEquipmentEvents.h"
#include "BBBWork/UBBBNexus/Character/Instance/System/NetworkSystem/BBBCharacterNetworkSystem.h"
#include "BBBWork/UBBBNexus/Character/Instance/System/NetworkSystem/Definition/Packets/BBBEquipmentActionNetworkPacket.h"

void FBBBEquipmentActionUploadProcessor::Update(
    const FBBBCharacterEquipmentEvents &EquipmentEvents,
    FBBBCharacterNetworkSystem &NetworkSystem) const
{
    // 将本帧装备事件逐条转换为网络动作包
    for (const FBBBEquipmentActionEvent &Event : EquipmentEvents.GetActionEvents())
    {
        FBBBEquipmentActionNetworkPacket Packet;
        Packet.ActionType = Event.ActionType;
        Packet.EquipmentId = Event.EquipmentId;
        Packet.Sequence = Event.Sequence;
        Packet.Phase = Event.Phase;
        Packet.LoadedAmmo = Event.LoadedAmmo;
        // 通过网络系统统一处理权威端和非权威端路径
        NetworkSystem.SubmitEquipmentActionPacket(MoveTemp(Packet));
    }
}
