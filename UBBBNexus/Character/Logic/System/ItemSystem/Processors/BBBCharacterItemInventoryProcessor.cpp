#include "BBBWork/UBBBNexus/Character/Logic/System/ItemSystem/Processors/BBBCharacterItemInventoryProcessor.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/ItemSystem/DomainData/Context/BBBCharacterItemUpdateContext.h"
#include "BBBWork/UBBBNexus/Character/Logic/RuntimeData/BBBCharacterRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/Config/Item/BBBCharacterItemConfig.h"
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "BBBWork/UBBBNexus/Character/Input/LocalControl/Item/FBBBCharacterItemMoveLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Base/Config/BBBConsumableDefinition.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Catalog/BBBConsumableCatalog.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Base/BBBEquipment.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Base/Config/BBBEquipmentDefinition.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Catalog/BBBEquipmentCatalog.h"
#include "BBBWork/UBBBNexus/Item/Misc/Catalog/BBBMiscCatalog.h"
#include "BBBWork/UBBBNexus/Item/Wearable/Catalog/BBBWearableCatalog.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"

void FBBBCharacterItemInventoryProcessor::Update(FBBBCharacterItemUpdateContext &Context) const
{
    auto &Inventory = Context.RuntimeData.Item.ItemInventoryState;
    auto &Operations = Context.RuntimeData.Item.ItemOperationState;
    if (Inventory.Slots.IsEmpty())
    {
        Inventory.QuickAccessSlotCount = Context.Config.QuickAccessSlotCount;
        Inventory.ConsumableQuickAccessSlotCount = Context.Config.ConsumableQuickAccessSlotCount;
        Inventory.EquipmentSlotCount = FMath::Max(Inventory.QuickAccessSlotCount, Context.Config.EquipmentSlotCount);
        Inventory.WearableSlotCount = FMath::Max(0, Context.Config.WearableSlotCount);
        Inventory.ConsumableSlotCount = FMath::Max(Inventory.ConsumableQuickAccessSlotCount, Context.Config.ConsumableSlotCount);
        Inventory.MiscSlotCount = FMath::Max(0, Context.Config.MiscSlotCount);
        Inventory.BackpackSlotCount = Inventory.EquipmentSlotCount + Inventory.WearableSlotCount
            + Inventory.ConsumableSlotCount + Inventory.MiscSlotCount;
        TSet<FName> Unique;
        for (FName Slot : Context.Config.WearSlots)
        {
            if (ensureMsgf(!Slot.IsNone() && !Unique.Contains(Slot), TEXT("穿戴位置必须非空且唯一")))
            {
                Unique.Add(Slot);
                Inventory.WearSlots.Add(Slot);
            }
        }
        Inventory.Slots.SetNum(Inventory.BackpackSlotCount + Inventory.WearSlots.Num());
        ++Inventory.Revision;
    }
    if (!Operations.PendingItemIds.IsEmpty() || !Operations.PendingMoveSources.IsEmpty()
        || !Operations.PendingSelectedSlots.IsEmpty() || !Operations.PendingConsumeInstances.IsEmpty()
        || !Operations.PendingFinishedInstances.IsEmpty() || Operations.bQuickUse || Operations.bPendingRestore)
    {
        Operations.SucceededCount = 0;
        Operations.RejectedCount = 0;
    }
    const auto Allows = [&Inventory](int32 Slot, const FBBBCharacterItem &Item)
    {
        if (!Inventory.Slots.IsValidIndex(Slot))
        {
            return false;
        }
        if (!Item.Definition)
        {
            return true;
        }
        const auto Type = Item.Definition->ItemType;
        if (Slot < Inventory.EquipmentSlotCount)
        {
            return Type == EBBBItemType::Equipment;
        }
        if (Slot < Inventory.EquipmentSlotCount + Inventory.WearableSlotCount)
        {
            return Type == EBBBItemType::Wearable;
        }
        if (Slot < Inventory.EquipmentSlotCount + Inventory.WearableSlotCount + Inventory.ConsumableSlotCount)
        {
            return Type == EBBBItemType::Consumable && Cast<UBBBConsumableDefinition>(Item.Definition);
        }
        if (Slot < Inventory.BackpackSlotCount)
        {
            return Type == EBBBItemType::Misc;
        }
        return Type == EBBBItemType::Wearable
            && Item.Definition->WearSlot == Inventory.WearSlots[Slot - Inventory.BackpackSlotCount];
    };
    if (Operations.bPendingRestore)
    {
        for (FBBBCharacterItem &Existing : Inventory.Slots)
        {
            if (IsValid(Existing.HeldInstance))
            {
                Existing.HeldInstance->Destroy();
            }
            Existing = FBBBCharacterItem{};
        }
        for (int32 Index = 0; Index < Operations.PendingRestoreSlots.Num(); ++Index)
        {
            const int32 TargetSlot = Operations.PendingRestoreSlots[Index];
            const FName Id = Operations.PendingRestoreItemIds[Index];
            const int32 Quantity = Operations.PendingRestoreQuantities[Index];
            if (!Inventory.Slots.IsValidIndex(TargetSlot) || Inventory.Slots[TargetSlot].Definition)
            {
                ++Operations.RejectedCount;
                continue;
            }
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
            Accept(Consumable);
            Accept(Context.Config.MiscCatalog ? Context.Config.MiscCatalog->FindItem(Id) : nullptr);
            if (Equipment)
            {
                ActorClass = Equipment;
            }
            FBBBCharacterItem Restored;
            Restored.InstanceId = Operations.PendingRestoreInstances[Index].IsValid()
                ? Operations.PendingRestoreInstances[Index] : FGuid::NewGuid();
            Restored.Definition = Definition;
            Restored.Quantity = Quantity;
            if (Candidates != 1 || !Definition || Quantity <= 0 || Quantity > Definition->MaxStack || !Allows(TargetSlot, Restored))
            {
                ++Operations.RejectedCount;
                continue;
            }
            if (ActorClass)
            {
                UWorld *World = Context.Character.GetWorld();
                AActor *Actor = World ? World->SpawnActorDeferred<AActor>(ActorClass, FTransform::Identity,
                    &Context.Character, &Context.Character, ESpawnActorCollisionHandlingMethod::AlwaysSpawn) : nullptr;
                if (Actor)
                {
                    Actor->SetActorHiddenInGame(true);
                    UGameplayStatics::FinishSpawningActor(Actor, FTransform::Identity);
                    Actor->SetActorTickEnabled(false);
                }
                const auto *EquipmentActor = Cast<ABBBEquipment>(Actor);
                if (!IsValid(Actor) || !EquipmentActor || !EquipmentActor->IsInitialized())
                {
                    if (IsValid(Actor))
                    {
                        Actor->Destroy();
                    }
                    ++Operations.RejectedCount;
                    continue;
                }
                Restored.HeldInstance = Actor;
            }
            Inventory.Slots[TargetSlot] = Restored;
            ++Operations.SucceededCount;
        }
        const int32 RestoredWeaponSlot = Context.Character.IsPlayerControlled() && Inventory.QuickAccessSlotCount > 0
            ? 0 : Operations.PendingRestoreWeaponSlot;
        if (RestoredWeaponSlot == INDEX_NONE || (RestoredWeaponSlot >= 0 && RestoredWeaponSlot < Inventory.QuickAccessSlotCount))
        {
            Operations.PendingSelectedSlots.Insert(RestoredWeaponSlot, 0);
        }
        const int32 ConsumableStart = Inventory.EquipmentSlotCount + Inventory.WearableSlotCount;
        const int32 RestoredConsumableSlot = Operations.PendingRestoreConsumableSlot;
        if (RestoredConsumableSlot >= 0 && RestoredConsumableSlot < Inventory.ConsumableQuickAccessSlotCount)
        {
            auto &Bar = Context.RuntimeData.Item.ItemBarState;
            Bar.SelectedConsumableSlot = RestoredConsumableSlot;
            const auto *Definition = Cast<UBBBConsumableDefinition>(Inventory.Slots[ConsumableStart + RestoredConsumableSlot].Definition);
            if (Definition && Definition->bThrowable)
            {
                Bar.SelectedThrowableSlot = RestoredConsumableSlot;
            }
            ++Bar.Revision;
        }
        ++Inventory.Revision;
        ++Operations.Revision;
        Operations.bPendingRestore = false;
        Operations.PendingRestoreSlots.Reset();
        Operations.PendingRestoreInstances.Reset();
        Operations.PendingRestoreItemIds.Reset();
        Operations.PendingRestoreQuantities.Reset();
        Operations.PendingRestoreWeaponSlot = INDEX_NONE;
        Operations.PendingRestoreConsumableSlot = INDEX_NONE;
    }
    for (int32 Index = 0; Index < Operations.PendingConsumeInstances.Num(); ++Index)
    {
        ++Operations.Revision;
        FBBBCharacterItem *Item = Inventory.Slots.FindByPredicate([&Operations, Index](const auto &Value)
        {
            return Value.InstanceId == Operations.PendingConsumeInstances[Index];
        });
        const int32 Quantity = Operations.PendingConsumeQuantities[Index];
        if (!Item || !Item->Definition || Item->Definition->ItemType != EBBBItemType::Consumable
            || Quantity <= 0 || Item->Quantity < Quantity)
        {
            ++Operations.RejectedCount;
            continue;
        }
        Item->Quantity -= Quantity;
        ++Inventory.Revision;
        ++Operations.SucceededCount;
    }
    Operations.PendingConsumeInstances.Reset();
    Operations.PendingConsumeQuantities.Reset();
    for (int32 Index = 0; Index < Operations.PendingMoveSources.Num(); ++Index)
    {
        const int32 Source = Operations.PendingMoveSources[Index];
        int32 Target = Operations.PendingMoveTargets[Index];
        ++Operations.Revision;
        if (!Inventory.Slots.IsValidIndex(Source) || !Inventory.Slots[Source].Definition
            || Inventory.Slots[Source].Quantity <= 0
            || Inventory.Slots[Source].InstanceId != Operations.PendingMoveInstances[Index]
            || Context.RuntimeData.Interaction.ReadInteractionUseState().bBusy)
        {
            ++Operations.RejectedCount;
            continue;
        }
        if (Target == FBBBCharacterItemMoveLocalControlPacket::FirstEmptyWeaponQuickSlot)
        {
            if (Inventory.Slots[Source].Definition->ItemType != EBBBItemType::Equipment)
            {
                ++Operations.RejectedCount;
                continue;
            }
            for (int32 Slot = 0; Slot < Inventory.QuickAccessSlotCount; ++Slot)
            {
                if (!Inventory.Slots[Slot].Definition)
                {
                    Target = Slot;
                    break;
                }
            }
        }
        else if (Target == INDEX_NONE)
        {
            for (int32 Slot = 0; Slot < Inventory.BackpackSlotCount; ++Slot)
            {
                if (!Inventory.Slots[Slot].Definition && Allows(Slot, Inventory.Slots[Source]))
                {
                    Target = Slot;
                    break;
                }
            }
        }
        if (!Allows(Target, Inventory.Slots[Source]) || !Allows(Source, Inventory.Slots[Target]))
        {
            ++Operations.RejectedCount;
            continue;
        }
        auto &From = Inventory.Slots[Source];
        auto &To = Inventory.Slots[Target];
        if (Source != Target && From.Definition == To.Definition && From.Definition->MaxStack > 1)
        {
            const int32 Added = FMath::Min(From.Quantity, From.Definition->MaxStack - To.Quantity);
            if (Added <= 0)
            {
                ++Operations.RejectedCount;
                continue;
            }
            From.Quantity -= Added;
            To.Quantity += Added;
            ++Inventory.Revision;
        }
        if (Source != Target && !(From.Definition == To.Definition && From.Definition->MaxStack > 1))
        {
            Swap(From, To);
            ++Inventory.Revision;
        }
        ++Operations.SucceededCount;
    }
    Operations.PendingMoveSources.Reset();
    Operations.PendingMoveTargets.Reset();
    Operations.PendingMoveInstances.Reset();
}
