#include "BBBWork/UBBBNexus/Equipment/Instance/System/FireSystem/Processors/BBBEquipmentFireProcessor.h"

#include "BBBWork/UBBBNexus/Character/Input/BBBCharacterInput.h"
#include "BBBWork/UBBBNexus/Equipment/BBBEquipment.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/System/FireSystem/Definition/BBBEquipmentFireRuntimeData.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/System/FireSystem/Definition/BBBEquipmentFireContext.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/System/ReloadSystem/Definition/BBBEquipmentReloadRuntimeData.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/System/FireSystem/Fragment/BBBEquipmentFireFragment.h"
#include "Engine/World.h"

void FBBBEquipmentFireProcessor::Update(
    ABBBEquipment &Instance, USkeletalMeshComponent &WeaponMesh,
    FBBBEquipmentFireRuntimeData &Data, const FBBBEquipmentReloadRuntimeData &Reload,
    const FBBBEquipmentFireFragment &Fragment, FBBBCharacterInput &CharacterAPI,
    const FName EquipmentId, const bool bIsMirror) const
{
    // 开火处理需要有效世界对象
    UWorld *World = Instance.GetWorld();
    if (!ensureMsgf(World, TEXT("[UBBBE]Fire update requires a world")))
    {
        return;
    }

    // 按输入顺序处理开火请求
    for (const FBBBEquipmentInput &Input : Data.Inputs)
    {
        if (!bIsMirror && (Reload.bIsReloading
            || !Fragment.CanFire(Data.LoadedAmmo, Data.LastFireTimeSeconds, World->GetTimeSeconds())))
        {
            continue;
        }

        FBBBEquipmentFireContext Context{
            Instance, *World, WeaponMesh, CharacterAPI, Data.LoadedAmmo,
            Data.LastFireTimeSeconds, Input.Sequence, bIsMirror};
        // 由装备片段执行具体开火行为
        if (!Fragment.Fire(Context))
        {
            UE_LOG(LogTemp, Warning, TEXT("[UBBBE]Fire failed Equipment=%s Sequence=%d"),
                *EquipmentId.ToString(), Input.Sequence);
            continue;
        }

        // 记录成功开火并同步本地事件
        Data.FireSequence++;
        if (bIsMirror)
        {
            // 镜像实例不重复发布网络事件
            continue;
        }

        FBBBEquipmentActionEvent Event;
        Event.EquipmentId = EquipmentId;
        Event.Sequence = Input.Sequence;
        Event.ActionType = EBBBCharacterActionType::Fire;
        Event.LoadedAmmo = Data.LoadedAmmo;
        FBBBCharacterDiscreteInput CharacterInput;
        CharacterInput.EquipmentEvent = Event;
        CharacterAPI.Submit(CharacterInput);
    }
}
