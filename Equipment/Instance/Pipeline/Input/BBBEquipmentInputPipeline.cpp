#include "BBBWork/UBBBNexus/Equipment/Instance/Pipeline/Input/BBBEquipmentInputPipeline.h"

void FBBBEquipmentInputPipeline::Initialize(FBBBEquipmentInputRuntimeData &InInput, FBBBEquipmentEquipRuntimeData &InEquip,
        FBBBEquipmentFireRuntimeData &InFire, FBBBEquipmentReloadRuntimeData &InReload,
        FBBBCharacterExternalAPI &InCharacterAPI, FName InEquipmentId, bool bInIsMirror)
{
    Input = &InInput;
    Equip = &InEquip;
    Fire = &InFire;
    Reload = &InReload;
    CharacterAPI = &InCharacterAPI;
    EquipmentId = InEquipmentId;
    bIsMirror = bInIsMirror;
}

void FBBBEquipmentInputPipeline::Update() const
{
    if (!ensureMsgf(Input && Equip && Fire && Reload && CharacterAPI,
        TEXT("[UBBBE]Input update dependencies are invalid")))
    {
        return;
    }

    Processor.Update(*Input, *Equip, *Fire, *Reload, *CharacterAPI, EquipmentId, bIsMirror);
}
