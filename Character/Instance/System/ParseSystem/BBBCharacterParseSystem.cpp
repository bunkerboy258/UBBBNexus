#include "BBBWork/UBBBNexus/Character/Instance/System/ParseSystem/BBBCharacterParseSystem.h"
#include "BBBWork/UBBBNexus/Character/Instance/Runtime/BBBCharacterRuntimeData.h"
#include "BBBWork/UBBBNexus/Equipment/Catalog/BBBEquipmentCatalog.h"

void FBBBCharacterParseSystem::Initialize(
    FBBBCharacterRuntimeData &InData, UBBBEquipmentCatalog &InEquipmentCatalog)
{
    Data = &InData;
    EquipmentCatalog = &InEquipmentCatalog;
}

void FBBBCharacterParseSystem::Update(const bool bRestoreMode) const
{
    if (!ensureMsgf(Data && EquipmentCatalog, TEXT("[UBBBC]Parse system is uninitialized")))
    {
        return;
    }

    InputProcessor.Update(*Data, *EquipmentCatalog, bRestoreMode);
}
