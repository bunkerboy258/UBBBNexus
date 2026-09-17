#include "BBBWork/UBBBNexus/Equipment/System/ReloadSystem/BBBEquipmentReloadSystem.h"

void FBBBEquipmentReloadSystem::Initialize(FBBBEquipmentReloadRuntimeData &InData, FBBBEquipmentFireRuntimeData &InFire,
        const FBBBEquipmentReloadFragment &InFragment, FBBBCharacterExternalAPI &InCharacterAPI,
        FName InEquipmentId, bool bInIsMirror)
{
    Data = &InData;
    Fire = &InFire;
    Fragment = &InFragment;
    CharacterAPI = &InCharacterAPI;
    EquipmentId = InEquipmentId;
    bIsMirror = bInIsMirror;
}

void FBBBEquipmentReloadSystem::Update() const
{
    if (!ensureMsgf(Data && Fire && Fragment && CharacterAPI,
        TEXT("[UBBBE]Reload update dependencies are invalid")))
    {
        return;
    }

    Processor.Update(*Data, *Fire, *Fragment, *CharacterAPI, EquipmentId, bIsMirror);
}
