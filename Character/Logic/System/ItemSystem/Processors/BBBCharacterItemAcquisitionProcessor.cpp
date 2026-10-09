#include "BBBWork/UBBBNexus/Character/Logic/System/ItemSystem/Processors/BBBCharacterItemAcquisitionProcessor.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/ItemSystem/DomainData/Context/BBBCharacterItemUpdateContext.h"
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "BBBWork/UBBBNexus/Item/Catalog/BBBItemCatalog.h"
#include "BBBWork/UBBBNexus/Equipment/Base/BBBEquipment.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"

void FBBBCharacterItemAcquisitionProcessor::Update(FBBBCharacterItemUpdateContext &Context) const
{
    auto &Inventory = Context.RuntimeData.Item.ItemInventoryState;
    auto &Operations = Context.RuntimeData.Item.ItemOperationState;
    const UBBBItemCatalog *Catalog = Context.Config.Catalog;
    for (const FName ItemId : Operations.PendingItemIds)
    {
        const FBBBItemCatalogEntry *Entry = Catalog ? Catalog->FindItem(ItemId) : nullptr;
        int32 EmptySlot = INDEX_NONE;
        for (int32 Slot = 0; Slot < Inventory.BackpackSlotCount; ++Slot)
        {
            const bool bMiscRegion = Slot >= Inventory.QuickAccessSlotCount + Inventory.EquipmentStorageSlotCount;
            if (Entry && !Inventory.Slots[Slot].Definition
                && (Slot >= Inventory.QuickAccessSlotCount || Entry->Definition->ItemType == EBBBItemType::Equipment)
                && bMiscRegion == (Entry->Definition->ItemType == EBBBItemType::Misc))
            {
                EmptySlot = Slot;
                break;
            }
        }
        ++Operations.Revision;
        if (EmptySlot == INDEX_NONE || !Entry)
        {
            ++Operations.RejectedCount;
            continue;
        }
        ABBBEquipment *Created = nullptr;
        if (Entry->Definition->ItemType == EBBBItemType::Equipment)
        {
            UWorld *World = Context.Character.GetWorld();
            if (World)
            {
                Created = World->SpawnActorDeferred<ABBBEquipment>(Entry->EquipmentClass, FTransform::Identity,
                    &Context.Character, &Context.Character, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
            }
            if (Created)
            {
                Created->SetActorHiddenInGame(true);
                UGameplayStatics::FinishSpawningActor(Created, FTransform::Identity);
            }
            if (!IsValid(Created) || !Created->IsInitialized())
            {
                if (IsValid(Created))
                {
                    Created->Destroy();
                }
                ++Operations.RejectedCount;
                continue;
            }
            Created->SetActorTickEnabled(false);
        }
        FBBBCharacterItem &Item = Inventory.Slots[EmptySlot];
        Item.InstanceId = FGuid::NewGuid();
        Item.Definition = Entry->Definition;
        Item.EquipmentInstance = Created;
        ++Inventory.Revision;
        ++Operations.SucceededCount;
    }
    Operations.PendingItemIds.Reset();
}

void FBBBCharacterItemAcquisitionProcessor::Shutdown(FBBBCharacterItemUpdateContext &Context)
{
    auto &Inventory = Context.RuntimeData.Item.ItemInventoryState;
    for (FBBBCharacterItem &Item : Inventory.Slots)
    {
        if (IsValid(Item.EquipmentInstance))
        {
            Item.EquipmentInstance->Destroy();
        }
    }
    auto &Operations = Context.RuntimeData.Item.ItemOperationState;
    Operations.PendingItemIds.Reset();
    Operations.PendingMoveSources.Reset();
    Operations.PendingMoveTargets.Reset();
    Operations.PendingMoveInstances.Reset();
    Operations.PendingSelectedSlots.Reset();
    Inventory.Slots.Reset();
    Inventory.WearSlots.Reset();
    Inventory.BackpackSlotCount = 0;
    Inventory.QuickAccessSlotCount = 0;
    Inventory.EquipmentStorageSlotCount = 0;
    ++Inventory.Revision;
}
