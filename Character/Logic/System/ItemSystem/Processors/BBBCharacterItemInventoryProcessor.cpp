#include "BBBWork/UBBBNexus/Character/Logic/System/ItemSystem/Processors/BBBCharacterItemInventoryProcessor.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/ItemSystem/DomainData/Context/BBBCharacterItemUpdateContext.h"
#include "BBBWork/UBBBNexus/Character/Logic/RuntimeData/BBBCharacterRuntimeData.h"
#include "GameFramework/Actor.h"
#include "BBBWork/UBBBNexus/Character/Config/Item/BBBItemConfig.h"

void FBBBCharacterItemInventoryProcessor::Update(FBBBCharacterItemUpdateContext &Context) const
{
    auto &Inventory = Context.RuntimeData.Item.ItemInventoryState;
    auto &Operations = Context.RuntimeData.Item.ItemOperationState;
    if (Inventory.BackpackSlots.IsEmpty())
    {
        Inventory.BackpackSlots.SetNum(FMath::Max(1, Context.Config.InventorySlotCount));
        ++Inventory.Revision;
    }
    if (!Operations.PendingEquipmentIds.IsEmpty() || !Operations.PendingMoveSources.IsEmpty()
        || !Operations.PendingSelectedSlots.IsEmpty())
    {
        Operations.SucceededCount = 0;
        Operations.RejectedCount = 0;
    }
    for (FBBBCharacterItem &Item : Inventory.BackpackSlots)
    {
        if (Item.ItemActor && !IsValid(Item.ItemActor.Get()))
        {
            Item.ItemActor = nullptr;
            ++Inventory.Revision;
        }
    }

    for (int32 Index = 0; Index < Operations.PendingMoveSources.Num(); ++Index)
    {
        const int32 Source = Operations.PendingMoveSources[Index];
        const int32 Target = Operations.PendingMoveTargets[Index];
        ++Operations.Revision;
        if (!Inventory.BackpackSlots.IsValidIndex(Source) || !Inventory.BackpackSlots.IsValidIndex(Target)
            || !IsValid(Inventory.BackpackSlots[Source].ItemActor.Get()))
        {
            ++Operations.RejectedCount;
            continue;
        }

        if (Source != Target)
        {
            Swap(Inventory.BackpackSlots[Source], Inventory.BackpackSlots[Target]);
            ++Inventory.Revision;
        }
        ++Operations.SucceededCount;
    }
    Operations.PendingMoveSources.Reset();
    Operations.PendingMoveTargets.Reset();
}
