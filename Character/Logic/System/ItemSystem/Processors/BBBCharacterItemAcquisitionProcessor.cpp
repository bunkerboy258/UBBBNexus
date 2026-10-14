#include "BBBWork/UBBBNexus/Character/Logic/System/ItemSystem/Processors/BBBCharacterItemAcquisitionProcessor.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/ItemSystem/DomainData/Context/BBBCharacterItemUpdateContext.h"
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Catalog/BBBEquipmentCatalog.h"
#include "BBBWork/UBBBNexus/Item/Wearable/Catalog/BBBWearableCatalog.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Catalog/BBBConsumableCatalog.h"
#include "BBBWork/UBBBNexus/Item/Misc/Catalog/BBBMiscCatalog.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Base/BBBEquipment.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Base/BBBConsumable.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Base/Config/BBBEquipmentDefinition.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Base/Config/BBBConsumableDefinition.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"

void FBBBCharacterItemAcquisitionProcessor::Update(FBBBCharacterItemUpdateContext &Context) const
{
    auto &Inventory = Context.RuntimeData.Item.ItemInventoryState;
    auto &Operations = Context.RuntimeData.Item.ItemOperationState;
    for (int32 Request = 0; Request < Operations.PendingItemIds.Num(); ++Request)
    {
        const FName Id = Operations.PendingItemIds[Request];
        UBBBItemDefinition *Definition = nullptr;
        TSubclassOf<AActor> ActorClass;
        int32 Candidates = 0;
        const auto Accept = [&Definition, &Candidates](const UBBBItemDefinition *Candidate)
        {
            if (Candidate)
            {
                Definition = const_cast<UBBBItemDefinition *>(Candidate);
                ++Candidates;
            }
        };
        const auto Equipment = Context.Config.EquipmentCatalog ? Context.Config.EquipmentCatalog->FindItem(Id) : nullptr;
        const auto *Wearable = Context.Config.WearableCatalog ? Context.Config.WearableCatalog->FindItem(Id) : nullptr;
        const auto Consumable = Context.Config.ConsumableCatalog ? Context.Config.ConsumableCatalog->FindItem(Id) : nullptr;
        Accept(Equipment ? Equipment.GetDefaultObject()->GetDefinition() : nullptr);
        Accept(Wearable ? Wearable->Definition.Get() : nullptr);
        Accept(Consumable ? Consumable.GetDefaultObject()->GetDefinition() : nullptr);
        Accept(Context.Config.MiscCatalog ? Context.Config.MiscCatalog->FindItem(Id) : nullptr);
        if (Equipment)
        {
            ActorClass = Equipment;
        }
        if (Consumable)
        {
            ActorClass = Consumable;
        }
        ++Operations.Revision;
        if (Candidates != 1 || !Definition || Definition->MaxStack < 1)
        {
            ++Operations.RejectedCount;
            continue;
        }
        int32 Start = 0;
        int32 Count = Inventory.EquipmentSlotCount;
        if (Definition->ItemType == EBBBItemType::Wearable)
        {
            Start = Inventory.EquipmentSlotCount;
            Count = Inventory.WearableSlotCount;
        }
        if (Definition->ItemType == EBBBItemType::Consumable)
        {
            Start = Inventory.EquipmentSlotCount + Inventory.WearableSlotCount;
            Count = Inventory.ConsumableSlotCount;
        }
        if (Definition->ItemType == EBBBItemType::Misc)
        {
            Start = Inventory.EquipmentSlotCount + Inventory.WearableSlotCount + Inventory.ConsumableSlotCount;
            Count = Inventory.MiscSlotCount;
        }
        int32 Remaining = Operations.PendingQuantities[Request];
        TArray<int32> Slots;
        TArray<int32> Added;
        for (int32 Pass = 0; Pass < 2; ++Pass)
        {
            for (int32 Slot = Start; Slot < Start + Count && Remaining > 0; ++Slot)
            {
                const auto &Item = Inventory.Slots[Slot];
                const bool bExisting = Item.Definition == Definition && Item.Quantity > 0;
                if ((Pass == 0 && !bExisting) || (Pass == 1 && Item.Definition))
                {
                    continue;
                }
                const int32 Amount = FMath::Min(Remaining, Definition->MaxStack - (bExisting ? Item.Quantity : 0));
                if (Amount > 0)
                {
                    Slots.Add(Slot);
                    Added.Add(Amount);
                    Remaining -= Amount;
                }
            }
        }
        if (Remaining > 0)
        {
            ++Operations.RejectedCount;
            continue;
        }
        TArray<AActor *> Created;
        Created.SetNumZeroed(Slots.Num());
        bool bPrepared = true;
        for (int32 Index = 0; Index < Slots.Num(); ++Index)
        {
            if (Inventory.Slots[Slots[Index]].Definition || !ActorClass)
            {
                continue;
            }
            UWorld *World = Context.Character.GetWorld();
            AActor *Actor = World ? World->SpawnActorDeferred<AActor>(ActorClass, FTransform::Identity,
                &Context.Character, &Context.Character, ESpawnActorCollisionHandlingMethod::AlwaysSpawn) : nullptr;
            Created[Index] = Actor;
            if (Actor)
            {
                Actor->SetActorHiddenInGame(true);
                UGameplayStatics::FinishSpawningActor(Actor, FTransform::Identity);
                Actor->SetActorTickEnabled(false);
            }
            const auto *EquipmentActor = Cast<ABBBEquipment>(Actor);
            const auto *ConsumableActor = Cast<ABBBConsumable>(Actor);
            bPrepared = IsValid(Actor) && ((EquipmentActor && EquipmentActor->IsInitialized())
                || (ConsumableActor && ConsumableActor->IsInitialized()));
            if (!bPrepared)
            {
                break;
            }
        }
        if (!bPrepared)
        {
            for (AActor *Actor : Created)
            {
                if (IsValid(Actor))
                {
                    Actor->Destroy();
                }
            }
            ++Operations.RejectedCount;
            continue;
        }
        for (int32 Index = 0; Index < Slots.Num(); ++Index)
        {
            auto &Item = Inventory.Slots[Slots[Index]];
            if (!Item.Definition)
            {
                Item.InstanceId = FGuid::NewGuid();
                Item.Definition = Definition;
                Item.HeldInstance = Created[Index];
            }
            Item.Quantity += Added[Index];
        }
        ++Inventory.Revision;
        ++Operations.SucceededCount;
    }
    Operations.PendingItemIds.Reset();
    Operations.PendingQuantities.Reset();
}
