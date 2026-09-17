#include "BBBWork/UBBBNexus/Equipment/Instance/System/FireSystem/BBBEquipmentFireSystem.h"

void FBBBEquipmentFireSystem::Initialize(ABBBEquipment &InInstance, USkeletalMeshComponent &InWeaponMesh,
        FBBBEquipmentFireRuntimeData &InData, const FBBBEquipmentReloadRuntimeData &InReload,
        const FBBBEquipmentFireFragment &InFragment, FBBBCharacterExternalAPI &InCharacterAPI,
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
    if (!ensureMsgf(Instance && WeaponMesh && Data && Reload && Fragment && CharacterAPI,
        TEXT("[UBBBE]Fire update dependencies are invalid")))
    {
        return;
    }

    Processor.Update(*Instance, *WeaponMesh, *Data, *Reload, *Fragment, *CharacterAPI, EquipmentId, bIsMirror);
}
