#include "BBBWork/UBBBNexus/Equipment/Instance/System/EquipSystem/Processors/BBBEquipmentEquipProcessor.h"

#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "BBBWork/UBBBNexus/Character/Input/Packets/Fact/BBBEquipFactPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/System/EquipSystem/Definition/BBBEquipmentEquipRuntimeData.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/System/FireSystem/Definition/BBBEquipmentFireRuntimeData.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/System/EquipSystem/Fragment/BBBEquipmentEquipFragment.h"

void FBBBEquipmentEquipProcessor::Update(
    FBBBEquipmentEquipRuntimeData &Data, const FBBBEquipmentFireRuntimeData &Fire,
    const FBBBEquipmentEquipFragment &Fragment, ABBBCharacter &CharacterAPI,
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

        // 发布本地装备切换事实
        FBBBEquipFactPacket Fact;
        Fact.EquipmentId = EquipmentId;
        Fact.Sequence = Input.Sequence;
        Fact.LoadedAmmo = Fire.LoadedAmmo;
        CharacterAPI.SubmitInput(Fact);
    }
}
