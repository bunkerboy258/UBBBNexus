#include "BBBWork/UBBBNexus/Character/Runtime/System/ParseSystem/BBBCharacterParseSystem.h"
#include "BBBWork/UBBBNexus/Character/Runtime/State/BBBCharacterRuntimeData.h"
#include "BBBWork/UBBBNexus/Equipment/Catalog/BBBEquipmentCatalog.h"

void FBBBCharacterParseSystem::Initialize(
    FBBBCharacterRuntimeData &InData, UBBBEquipmentCatalog &InEquipmentCatalog)
{
    Data = &InData;
    EquipmentCatalog = &InEquipmentCatalog;
}

void FBBBCharacterParseSystem::Update(
    const bool bAuthority,
    const bool bLocallyControlled) const
{
    if (!ensureMsgf(Data && EquipmentCatalog, TEXT("[UBBBC]Parse system is uninitialized")))
    {
        return;
    }

    InputProcessor.Update(*Data, *EquipmentCatalog, bAuthority, bLocallyControlled);
}
