#include "BBBWork/UBBBNexus/Character/Runtime/System/ParseSystem/BBBCharacterParseSystem.h"
#include "BBBWork/UBBBNexus/Character/Runtime/System/ParseSystem/DomainData/Context/BBBCharacterInputContext.h"
#include "BBBWork/UBBBNexus/Character/Runtime/RuntimeData/BBBCharacterRuntimeData.h"
#include "BBBWork/UBBBNexus/Equipment/Catalog/BBBEquipmentCatalog.h"

void FBBBCharacterParseSystem::Initialize(
    FBBBCharacterRuntimeData &InData, UBBBEquipmentCatalog &InEquipmentCatalog)
{
    Data = &InData;
    EquipmentCatalog = &InEquipmentCatalog;
}

void FBBBCharacterParseSystem::Update() const
{
    if (!Data || !EquipmentCatalog)
    {
        return;
    }

    FBBBCharacterInputContext Context{
        Data->Parse.OperationState,
        Data->Equipment.EquipmentInventoryState,
        Data->Equipment.EquipmentSelectionState,
        Data->Equipment.EquipmentCommandState,
        Data->Equipment.EquipmentEventState,
        Data->Animation.AnimationState,
        Data->Aim.AimState,
        Data->Locomotion.LocomotionState,
        Data->Parse.ControlState,
        Data->Parse.CameraState.PendingInput,
        *EquipmentCatalog};
    InputProcessor.Update(Data->Parse.InputState, Context);
}
