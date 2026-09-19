#include "BBBWork/UBBBNexus/Character/Runtime/System/NetworkSystem/Processors/Observe/Processors/BBBEquipmentActionUploadProcessor.h"

#include "BBBWork/UBBBNexus/Character/Runtime/Controller/EquipmentController/Definition/Events/BBBCharacterEquipmentEvents.h"
#include "BBBWork/UBBBNexus/Character/Runtime/System/NetworkSystem/BBBCharacterNetworkSystem.h"
#include "BBBWork/UBBBNexus/Character/Runtime/System/NetworkSystem/Definition/Packets/BBBEquipmentActionNetworkPacket.h"

void FBBBEquipmentActionUploadProcessor::Update(
    const FBBBCharacterEquipmentEvents &EquipmentEvents,
    FBBBCharacterNetworkSystem &NetworkSystem) const
{
    // 将本帧装备事件逐条转换为网络动作包
    for (const FBBBEquipmentActionFact &Event : EquipmentEvents.GetActionEvents())
    {
        FBBBEquipmentActionNetworkPacket Packet;
        Packet.PacketId = Event.PacketId;
        Packet.EquipmentId = Event.EquipmentId;
        Packet.Sequence = Event.Sequence;
        Packet.LoadedAmmo = Event.LoadedAmmo;
        // 通过网络系统统一处理权威端和非权威端路径
        NetworkSystem.SubmitEquipmentActionPacket(MoveTemp(Packet));
    }
}
