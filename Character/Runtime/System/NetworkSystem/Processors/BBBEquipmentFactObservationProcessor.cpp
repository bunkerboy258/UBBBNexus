#include "BBBWork/UBBBNexus/Character/Runtime/System/NetworkSystem/Processors/BBBEquipmentFactObservationProcessor.h"

#include "BBBWork/UBBBNexus/Character/Runtime/Controller/EquipmentController/Definition/Events/BBBCharacterEquipmentEvents.h"
#include "BBBWork/UBBBNexus/Character/Runtime/System/NetworkSystem/BBBCharacterNetworkSystem.h"

void FBBBEquipmentFactObservationProcessor::Update(
    const FBBBCharacterEquipmentEvents &EquipmentEvents,
    FBBBCharacterNetworkSystem &NetworkSystem) const
{
    // 将本帧装备事件逐条转换为网络动作包
    for (const FBBBEquipmentActionFact &Event : EquipmentEvents.ActionEvents)
    {
        NetworkSystem.TransmitEquipmentFact(Event);
    }
}
