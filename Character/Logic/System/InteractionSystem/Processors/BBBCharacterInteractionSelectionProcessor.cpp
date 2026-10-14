#include "BBBWork/UBBBNexus/Character/Logic/System/InteractionSystem/Processors/BBBCharacterInteractionSelectionProcessor.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/InteractionSystem/DomainData/Context/BBBCharacterInteractionUpdateContext.h"
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Catalog/BBBEquipmentCatalog.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Catalog/BBBConsumableCatalog.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Base/BBBEquipment.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Base/BBBConsumable.h"

void FBBBCharacterInteractionSelectionProcessor::Update(FBBBCharacterInteractionUpdateContext &Context) const
{
    auto &Selection = Context.RuntimeData.Interaction.InteractionSelectionState;
    Context.bHasSelectionResult = true;
    if (!Context.bIsMirror)
    {
        Context.DesiredItem = Context.RuntimeData.Item.ReadCharacterItemBarState().DesiredMainHandItem;
        return;
    }
    Context.DesiredItem = Selection.ActiveMainHandInstance;
    if (!Selection.bHasInteractionRequest)
    {
        return;
    }
    Selection.bHasInteractionRequest = false;
    Context.DesiredGeneration = Selection.PendingGeneration;
    if (IsValid(Selection.ActiveMainHandInstance) && Selection.PendingItemId == Selection.ActiveItemId
        && Selection.PendingGeneration == Selection.ActiveGeneration)
    {
        return;
    }
    Context.DesiredItem = nullptr;
    if (Selection.PendingItemId.IsNone())
    {
        return;
    }
    const auto &Config = Context.Character.GetCharacterConfig().Item;
    const auto Equipment = Config.EquipmentCatalog ? Config.EquipmentCatalog->FindItem(Selection.PendingItemId) : nullptr;
    const auto Consumable = Config.ConsumableCatalog ? Config.ConsumableCatalog->FindItem(Selection.PendingItemId) : nullptr;
    if (Equipment && !Consumable)
    {
        Context.PendingItemClass = Equipment;
    }
    if (Consumable && !Equipment)
    {
        Context.PendingItemClass = Consumable;
    }
    Context.bHasSelectionResult = Context.PendingItemClass != nullptr;
}
