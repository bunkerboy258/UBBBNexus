#include "BBBWork/UBBBNexus/Character/Logic/System/EquipmentSystem/Processors/BBBCharacterEquipmentActionPermissionProcessor.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/EquipmentSystem/DomainData/Context/BBBCharacterEquipmentUpdateContext.h"
#include "BBBWork/UBBBNexus/Character/Logic/RuntimeData/BBBCharacterRuntimeData.h"
#include "BBBWork/UBBBNexus/Equipment/Base/BBBEquipment.h"
#include "BBBWork/UBBBNexus/Equipment/Base/Input/LocalControl/Equipment/FBBBEquipmentActionPermissionLocalControlPacket.h"

void FBBBCharacterEquipmentActionPermissionProcessor::Update(FBBBCharacterEquipmentUpdateContext &Context) const
{
    ABBBEquipment *Equipment = Context.RuntimeData.Equipment.ReadEquipmentSelectionState().ActiveMainHandInstance;
    if (Context.bIsMirror || !IsValid(Equipment))
    {
        return;
    }

    // 在攀爬检测后且 CMC 前发送 确保装备本帧不能抢先开火或装匣
    const bool bAllowed = Context.RuntimeData.Equipment.ReadEquipmentUseState().bUsable;
    Equipment->SubmitInput(FBBBEquipmentActionPermissionLocalControlPacket{{bAllowed}});
}
