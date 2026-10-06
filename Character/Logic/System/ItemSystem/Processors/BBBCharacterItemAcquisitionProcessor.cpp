#include "BBBWork/UBBBNexus/Character/Logic/System/ItemSystem/Processors/BBBCharacterItemAcquisitionProcessor.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/ItemSystem/DomainData/Context/BBBCharacterItemUpdateContext.h"
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "BBBWork/UBBBNexus/Equipment/Catalog/BBBEquipmentCatalog.h"
#include "BBBWork/UBBBNexus/Equipment/Base/BBBEquipment.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"

void FBBBCharacterItemAcquisitionProcessor::Update(FBBBCharacterItemUpdateContext &Context) const
{
    auto &Inventory = Context.RuntimeData.Item.ItemInventoryState;
    auto &Operations = Context.RuntimeData.Item.ItemOperationState;
    if (Operations.PendingEquipmentIds.IsEmpty())
    {
        return;
    }
    const UBBBEquipmentCatalog *Catalog = Context.Character.GetCharacterConfig().Equipment.EquipmentCatalog;
    UWorld *World = Context.Character.GetWorld();
    for (const FName EquipmentId : Operations.PendingEquipmentIds)
    {
        int32 EmptySlot = INDEX_NONE;
        for (int32 Slot = 0; Slot < Inventory.BackpackSlots.Num(); ++Slot)
        {
            if (!IsValid(Inventory.BackpackSlots[Slot].ItemActor.Get()))
            {
                EmptySlot = Slot;
                break;
            }
        }

        const TSubclassOf<ABBBEquipment> Class = Catalog ? Catalog->FindEquipmentClass(EquipmentId) : nullptr;
        ABBBEquipment *Created = nullptr;
        if (EmptySlot != INDEX_NONE && Class && World)
        {
            Created = World->SpawnActorDeferred<ABBBEquipment>(Class, FTransform::Identity,
                &Context.Character, &Context.Character, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
            if (Created)
            {
                Created->SetActorHiddenInGame(true);
                UGameplayStatics::FinishSpawningActor(Created, FTransform::Identity);
                if (!IsValid(Created) || !Created->IsInitialized())
                {
                    if (IsValid(Created))
                    {
                        Created->Destroy();
                    }
                    Created = nullptr;
                }
            }
        }

        ++Operations.Revision;
        if (!Created)
        {
            ++Operations.RejectedCount;
            continue;
        }

        Inventory.BackpackSlots[EmptySlot].ItemActor = Created;
        Created->SetActorTickEnabled(false);
        ++Inventory.Revision;
        ++Operations.SucceededCount;
    }
    Operations.PendingEquipmentIds.Reset();
}

void FBBBCharacterItemAcquisitionProcessor::Shutdown(FBBBCharacterItemUpdateContext &Context)
{
    auto &Inventory = Context.RuntimeData.Item.ItemInventoryState;
    for (FBBBCharacterItem &Item : Inventory.BackpackSlots)
    {
        if (IsValid(Item.ItemActor.Get()))
        {
            Item.ItemActor->Destroy();
        }
        Item.ItemActor = nullptr;
    }
    auto &Operations = Context.RuntimeData.Item.ItemOperationState;
    Operations.PendingEquipmentIds.Reset();
    Operations.PendingMoveSources.Reset();
    Operations.PendingMoveTargets.Reset();
    Operations.PendingSelectedSlots.Reset();
    Inventory.BackpackSlots.Reset();
    ++Inventory.Revision;
}
