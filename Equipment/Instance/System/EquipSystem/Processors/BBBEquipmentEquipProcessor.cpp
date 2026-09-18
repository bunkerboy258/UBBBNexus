#include "BBBWork/UBBBNexus/Equipment/Instance/System/EquipSystem/Processors/BBBEquipmentEquipProcessor.h"

#include "BBBWork/UBBBNexus/Character/Input/BBBCharacterInput.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/System/EquipSystem/Definition/BBBEquipmentEquipRuntimeData.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/System/FireSystem/Definition/BBBEquipmentFireRuntimeData.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/System/EquipSystem/Fragment/BBBEquipmentEquipFragment.h"

void FBBBEquipmentEquipProcessor::Update(
    FBBBEquipmentEquipRuntimeData &Data, const FBBBEquipmentFireRuntimeData &Fire,
    const FBBBEquipmentEquipFragment &Fragment, FBBBCharacterInput &CharacterAPI,
    const FName EquipmentId, const bool bIsMirror) const
{
    // 按输入顺序提交装备切换表现
    for (const FBBBEquipmentInput &Input : Data.Inputs)
    {
        if (!Fragment.SubmitMontage(CharacterAPI, Input.Sequence))
        {
            UE_LOG(LogTemp, Warning, TEXT("[UBBBE]Equip presentation failed Equipment=%s Sequence=%d"),
                *EquipmentId.ToString(), Input.Sequence);
            continue;
        }

        if (bIsMirror)
        {
            // 镜像装备只执行表现不发布本地事件
            continue;
        }

        // 发布本地装备切换事件
        FBBBEquipmentActionEvent Event;
        Event.EquipmentId = EquipmentId;
        Event.Sequence = Input.Sequence;
        Event.ActionType = EBBBCharacterActionType::Equip;
        Event.LoadedAmmo = Fire.LoadedAmmo;
        FBBBCharacterDiscreteInput CharacterInput;
        CharacterInput.EquipmentEvent = Event;
        CharacterAPI.Submit(CharacterInput);
    }
}
