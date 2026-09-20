#include "BBBWork/UBBBNexus/Character/Runtime/System/NetworkSystem/Processors/BBBEquipmentStateObservationProcessor.h"
#include "BBBWork/UBBBNexus/Character/Runtime/System/NetworkSystem/BBBCharacterNetworkSystem.h"
#include "BBBWork/UBBBNexus/Character/Runtime/Controller/EquipmentController/Definition/States/BBBCharacterEquipmentStates.h"
#include "BBBWork/UBBBNexus/Character/Runtime/System/NetworkSystem/State/BBBNetworkState.h"
#include "BBBWork/UBBBNexus/Equipment/Base/BBBEquipment.h"

void FBBBEquipmentStateObservationProcessor::Update(
    const FBBBCharacterEquipmentState &EquipmentState,
    FBBBNetworkState &NetworkData,
    FBBBCharacterNetworkSystem &NetworkSystem) const
{
    // 只有当前存在主手装备时才上传装备状态
    ABBBEquipment *ActiveEquipment = EquipmentState.GetActiveMainHandInstance();

    if (!ActiveEquipment)
    { return; }

    // 实例没有变化时不重复广播装备状态
    if (NetworkData.GetLastUploadedEquipmentInstanceId() == ActiveEquipment->GetInstanceId())
    { return; }

    NetworkSystem.TransmitEquipmentState(ActiveEquipment->GetEquipmentId());

    NetworkData.CommitLastUploadedEquipmentInstanceId(ActiveEquipment->GetInstanceId());
}
