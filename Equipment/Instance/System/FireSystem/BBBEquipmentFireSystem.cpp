#include "BBBWork/UBBBNexus/Equipment/Instance/System/FireSystem/BBBEquipmentFireSystem.h"

void FBBBEquipmentFireSystem::Initialize(ABBBEquipment &InInstance, USkeletalMeshComponent &InWeaponMesh,
        FBBBEquipmentFireRuntimeData &InData, const FBBBEquipmentReloadRuntimeData &InReload,
        const FBBBEquipmentFireFragment &InFragment, ABBBCharacter &InCharacterAPI,
        FName InEquipmentId, bool bInIsMirror)
{
    Instance = &InInstance;
    WeaponMesh = &InWeaponMesh;
    Data = &InData;
    Reload = &InReload;
    Fragment = &InFragment;
    CharacterAPI = &InCharacterAPI;
    EquipmentId = InEquipmentId;
    bIsMirror = bInIsMirror;
}

void FBBBEquipmentFireSystem::Update() const
{
    // 开火更新需要装备网格和换弹状态有效
    if (!ensureMsgf(Instance && WeaponMesh && Data && Reload && Fragment && CharacterAPI,
        TEXT("[UBBBE]Fire update dependencies are invalid")))
    {
        return;
    }

    // 处理开火输入并同步开火事件
    Processor.Update(*Instance, *WeaponMesh, *Data, *Reload, *Fragment, *CharacterAPI, EquipmentId, bIsMirror);
}
