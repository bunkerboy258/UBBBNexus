#include "BBBWork/UBBBNexus/Character/Logic/System/EquipmentSystem/Processors/BBBCharacterEquipmentSelectionProcessor.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/EquipmentSystem/DomainData/Context/BBBCharacterEquipmentUpdateContext.h"
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "BBBWork/UBBBNexus/Item/Catalog/BBBItemCatalog.h"
#include "BBBWork/UBBBNexus/Equipment/Base/BBBEquipment.h"

void FBBBCharacterEquipmentSelectionProcessor::Update(FBBBCharacterEquipmentUpdateContext &Context) const
{
    auto &Selection = Context.RuntimeData.Equipment.EquipmentSelectionState;
    if (!Context.bIsMirror)
    {
        Context.DesiredEquipment = Cast<ABBBEquipment>(
            Context.RuntimeData.Item.ReadItemBarState().DesiredMainHandItem.Get());
        Context.bHasSelectionResult = true;
        return;
    }

    Context.DesiredEquipment = IsValid(Selection.ActiveMainHandInstance)
        ? Selection.ActiveMainHandInstance.Get() : nullptr;
    Context.bHasSelectionResult = true;
    if (!Selection.bHasEquipmentRequest)
    {
        return;
    }

    Context.DesiredGeneration = Selection.PendingGeneration;
    Selection.bHasEquipmentRequest = false;
    Context.DesiredEquipment = nullptr;
    if (Selection.PendingEquipmentId.IsNone())
    {
        return;
    }
    const UBBBItemCatalog *Catalog = Context.Character.GetCharacterConfig().Item.Catalog;
    Context.PendingEquipmentClass = Catalog ? Catalog->FindEquipmentClass(Selection.PendingEquipmentId) : nullptr;
    Context.bHasSelectionResult = Context.PendingEquipmentClass != nullptr;
}
