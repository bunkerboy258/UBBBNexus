#include "BBBWork/UBBBNexus/Character/Logic/System/AppearanceSystem/Processors/BBBCharacterAppearanceInputProcessor.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/AppearanceSystem/DomainData/Context/BBBCharacterAppearanceUpdateContext.h"
#include "BBBWork/UBBBNexus/Character/Logic/RuntimeData/BBBCharacterRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/Config/Appearance/BBBCharacterAppearanceConfig.h"

void FBBBCharacterAppearanceInputProcessor::Update(FBBBCharacterAppearanceUpdateContext &Context) const
{
    auto &Input = Context.Data.Appearance.InputState;
    auto &Style = Context.Data.Appearance.StyleState;
    auto &Selection = Context.Data.Appearance.SelectionState.Snapshot;
    if (Style.Revision == 0)
    {
        Style.BaseParts = Context.Config.DefaultBaseParts;
        Style.Dirt = Context.Config.Dirt;
        Style.Weathering = Context.Config.Weathering;
        ++Style.Revision;
    }
    if (Context.bIsMirror)
    {
        for (const FBBBCharacterAppearanceSnapshot &Received : Input.PendingSelections)
        {
            if (Received.IsValid() && Received.Revision > Selection.Revision)
            {
                Selection = Received;
            }
        }
        Input.PendingSelections.Reset();
        return;
    }
    const auto &Inventory = Context.Data.Item.ReadCharacterItemInventoryState();
    for (int32 Index = Style.Styles.Num() - 1; Index >= 0; --Index)
    {
        const FGuid Id = Style.Styles[Index].InstanceId;
        if (Id.IsValid() && !Inventory.Slots.ContainsByPredicate(
            [&Id](const FBBBCharacterItem &Item)
            {
                return Item.InstanceId == Id;
            }))
        {
            Style.Styles.RemoveAt(Index);
            ++Style.Revision;
        }
    }
    const auto FindStyle = [&Inventory, &Style](const FGuid Id, const FName Slot) -> FBBBCharacterAppearanceStyle *
    {
        const bool bOwned = Id.IsValid()
            ? Inventory.Slots.ContainsByPredicate([Id](const FBBBCharacterItem &Item)
                {
                    return Item.Definition && Item.InstanceId == Id;
                })
            : Style.BaseParts.Contains(Slot);
        if (!bOwned)
        {
            return nullptr;
        }
        FBBBCharacterAppearanceStyle *Stored = Style.Styles.FindByPredicate(
            [Id, Slot](const FBBBCharacterAppearanceStyle &Value)
            {
                return Value.InstanceId == Id && (Id.IsValid() || Value.Slot == Slot);
            });
        if (!Stored)
        {
            Stored = &Style.Styles.AddDefaulted_GetRef();
            Stored->InstanceId = Id;
            Stored->Slot = Slot;
        }
        return Stored;
    };
    for (int32 Index = 0; Index < Input.PendingColors.Num(); ++Index)
    {
        FBBBCharacterAppearanceStyle *Stored = FindStyle(Input.PendingColorInstances[Index], Input.PendingColorSlots[Index]);
        if (Stored)
        {
            Stored->Colors = Input.PendingColors[Index];
            ++Style.Revision;
        }
    }
    Input.PendingColorInstances.Reset();
    Input.PendingColorSlots.Reset();
    Input.PendingColors.Reset();
    for (int32 Index = 0; Index < Input.PendingCamouflage.Num(); ++Index)
    {
        FBBBCharacterAppearanceStyle *Stored = FindStyle(Input.PendingCamouflageInstances[Index], Input.PendingCamouflageSlots[Index]);
        if (Stored)
        {
            Stored->bCamouflage = Input.PendingCamouflage[Index];
            ++Style.Revision;
        }
    }
    Input.PendingCamouflageInstances.Reset();
    Input.PendingCamouflageSlots.Reset();
    Input.PendingCamouflage.Reset();
    for (int32 Index = 0; Index < Input.PendingBaseSlots.Num(); ++Index)
    {
        if (!Input.PendingBaseResources.IsValidIndex(Index))
        {
            continue;
        }
        const FName Slot = Input.PendingBaseSlots[Index];
        const FName Id = Input.PendingBaseResources[Index];
        const FBBBAppearanceResource *Resource = Context.Config.BaseResources.Find(Id);
        if (Style.BaseParts.Contains(Slot) && Resource && Resource->Part == Slot)
        {
            Style.BaseParts[Slot] = Id;
            ++Style.Revision;
        }
    }
    Input.PendingBaseSlots.Reset();
    Input.PendingBaseResources.Reset();
    for (const float Dirt : Input.PendingDirt)
    {
        Style.Dirt = Dirt;
        ++Style.Revision;
    }
    for (const float Weathering : Input.PendingWeathering)
    {
        Style.Weathering = Weathering;
        ++Style.Revision;
    }
    Input.PendingDirt.Reset();
    Input.PendingWeathering.Reset();
    Input.PendingSelections.Reset();
}

void FBBBCharacterAppearanceInputProcessor::Shutdown(FBBBCharacterAppearanceUpdateContext &Context)
{
    auto &Input = Context.Data.Appearance.InputState;
    Input.PendingColorInstances.Reset();
    Input.PendingColorSlots.Reset();
    Input.PendingColors.Reset();
    Input.PendingCamouflageInstances.Reset();
    Input.PendingCamouflageSlots.Reset();
    Input.PendingCamouflage.Reset();
    Input.PendingBaseSlots.Reset();
    Input.PendingBaseResources.Reset();
    Input.PendingDirt.Reset();
    Input.PendingWeathering.Reset();
    Input.PendingSelections.Reset();
    auto &Style = Context.Data.Appearance.StyleState;
    Style.Styles.Reset();
    Style.BaseParts.Reset();
    Style.Revision = 0;
    auto &Selection = Context.Data.Appearance.SelectionState.Snapshot;
    Selection.Parts.Reset();
    Selection.Revision = 0;
}
