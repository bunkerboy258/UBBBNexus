#include "BBBWork/UBBBNexus/Equipment/System/EquipSystem/BBBEquipmentEquipSystem.h"

void FBBBEquipmentEquipSystem::Initialize(FBBBEquipmentEquipRuntimeData &InData, const FBBBEquipmentFireRuntimeData &InFire,
        const FBBBEquipmentEquipFragment &InFragment, FBBBCharacterExternalAPI &InCharacterAPI,
        FName InEquipmentId, bool bInIsMirror)
{
    Data = &InData;
    Fire = &InFire;
    Fragment = &InFragment;
    CharacterAPI = &InCharacterAPI;
    EquipmentId = InEquipmentId;
    bIsMirror = bInIsMirror;
}

void FBBBEquipmentEquipSystem::Update() const
{
    if (!ensureMsgf(Data && Fire && Fragment && CharacterAPI,
        TEXT("[UBBBE]Equip update dependencies are invalid")))
    {
        return;
    }

    Processor.Update(*Data, *Fire, *Fragment, *CharacterAPI, EquipmentId, bIsMirror);
}
