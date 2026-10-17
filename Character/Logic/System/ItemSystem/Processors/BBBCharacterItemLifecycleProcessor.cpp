#include "BBBWork/UBBBNexus/Character/Logic/System/ItemSystem/Processors/BBBCharacterItemLifecycleProcessor.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/ItemSystem/DomainData/Context/BBBCharacterItemUpdateContext.h"
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Base/BBBConsumable.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Base/Config/BBBConsumableDefinition.h"

void FBBBCharacterItemLifecycleProcessor::Update(FBBBCharacterItemUpdateContext &Context) const
{
    auto &Inventory = Context.RuntimeData.Item.ItemInventoryState;
    auto &Use = Context.RuntimeData.Item.ItemUseState;
    const auto &Interaction = Context.RuntimeData.Interaction.ReadInteractionUseState();
    if (Use.Runtime && !Use.Runtime->IsUsing() && !Interaction.bBusy)
    {
        Use.Runtime->ShutdownUse();
        Use.Runtime = nullptr;
        Use.Instance.Invalidate();
    }
    const auto &Bar = Context.RuntimeData.Item.ReadCharacterItemBarState();
    const auto *Definition = Bar.DesiredConsumableDefinition.Get();
    if (Bar.bStartUse && Definition && !Use.Runtime && !Interaction.bBusy &&
        !Context.RuntimeData.Interaction.ReadInteractionInputState().bQuickMelee && Definition->RuntimeClass)
    {
        auto *Runtime = NewObject<UBBBConsumable>(&Context.Character, Definition->RuntimeClass);
        if (Runtime && Runtime->InitializeUse(*const_cast<UBBBConsumableDefinition *>(Definition)))
        {
            Use.Runtime = Runtime;
            Use.Instance = Bar.DesiredConsumableIdentity;
            Use.Serial = Interaction.UseSerial + 1;
            Runtime->SubmitInput(FBBBConsumablePrimaryLocalControlPacket<>{});
            Runtime->UpdateUse();
            if (!Runtime->IsUsing())
            {
                Runtime->ShutdownUse();
                Use.Runtime = nullptr;
                Use.Instance.Invalidate();
            }
        }
    }
    for (auto &Item : Inventory.Slots)
    {
        if (!Item.Definition)
        {
            continue;
        }
        if (!IsValid(Item.Definition) || !Item.InstanceId.IsValid() ||
            (Item.Definition->ItemType == EBBBItemType::Equipment && !IsValid(Item.HeldInstance)))
        {
            Item.Quantity = 0;
        }
        if (Item.Quantity > 0 || (Item.HeldInstance && Item.HeldInstance == Context.Character.GetHeldItem()) ||
            (Use.Runtime && Use.Instance == Item.InstanceId))
        {
            continue;
        }
        if (IsValid(Item.HeldInstance))
        {
            Item.HeldInstance->Destroy();
        }
        Item = {};
        ++Inventory.Revision;
    }
}

void FBBBCharacterItemLifecycleProcessor::Shutdown(FBBBCharacterItemUpdateContext &Context) const
{
    auto &Use = Context.RuntimeData.Item.ItemUseState;
    if (Use.Runtime)
    {
        Use.Runtime->ShutdownUse();
        Use.Runtime = nullptr;
    }
    Use.Instance.Invalidate();
    auto &Inventory = Context.RuntimeData.Item.ItemInventoryState;
    for (auto &Item : Inventory.Slots)
    {
        if (IsValid(Item.HeldInstance))
        {
            Item.HeldInstance->Destroy();
        }
    }
    Inventory.Slots.Reset();
    Inventory.WearSlots.Reset();
    Inventory.BackpackSlotCount = 0;
    ++Inventory.Revision;
    auto &Operations = Context.RuntimeData.Item.ItemOperationState;
    Operations.PendingItemIds.Reset();
    Operations.PendingQuantities.Reset();
    Operations.PendingMoveSources.Reset();
    Operations.PendingMoveTargets.Reset();
    Operations.PendingMoveInstances.Reset();
    Operations.PendingSelectedSlots.Reset();
    Operations.PendingConsumeInstances.Reset();
    Operations.PendingConsumeQuantities.Reset();
    Operations.PendingFinishedInstances.Reset();
    Operations.bQuickUse = false;
    Operations.Revision = 0;
    Operations.SucceededCount = 0;
    Operations.RejectedCount = 0;
}
