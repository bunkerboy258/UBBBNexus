#include "BBBWork/UBBBNexus/Character/Logic/System/AppearanceSystem/Processors/BBBCharacterAppearanceSelectionProcessor.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/AppearanceSystem/DomainData/Context/BBBCharacterAppearanceUpdateContext.h"
#include "BBBWork/UBBBNexus/Character/Logic/RuntimeData/BBBCharacterRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/Config/Appearance/BBBCharacterAppearanceConfig.h"
#include "BBBWork/UBBBNexus/Item/Catalog/BBBItemCatalog.h"

void FBBBCharacterAppearanceSelectionProcessor::Update(FBBBCharacterAppearanceUpdateContext &Context) const
{
    if (Context.bIsMirror)
    {
        return;
    }
    const auto &Inventory = Context.Data.Item.ReadCharacterItemInventoryState();
    const auto &Style = Context.Data.Appearance.ReadCharacterAppearanceStyleState();
    auto &Selection = Context.Data.Appearance.SelectionState.Snapshot;
    TArray<FBBBCharacterAppearancePart> Parts;
    TArray<FName> BaseSlots;
    Style.BaseParts.GetKeys(BaseSlots);
    BaseSlots.Sort(FNameLexicalLess());
    for (const FName Slot : BaseSlots)
    {
        FBBBCharacterAppearancePart &Part = Parts.AddDefaulted_GetRef();
        Part.Slot = Slot;
        Part.BaseResourceId = Style.BaseParts[Slot];
    }
    TSet<FName> OccupiedWearSlots;
    for (int32 Index = Inventory.BackpackSlotCount; Index < Inventory.Slots.Num(); ++Index)
    {
        if (Inventory.Slots[Index].Definition)
        {
            OccupiedWearSlots.Add(Inventory.WearSlots[Index - Inventory.BackpackSlotCount]);
        }
    }
    for (int32 Index = Inventory.BackpackSlotCount; Index < Inventory.Slots.Num(); ++Index)
    {
        const FBBBCharacterItem &Item = Inventory.Slots[Index];
        if (!Item.Definition)
        {
            continue;
        }
        const FName WearSlot = Inventory.WearSlots[Index - Inventory.BackpackSlotCount];
        const FBBBItemCatalogEntry *Entry = Context.Catalog ? Context.Catalog->FindItem(Item.Definition->ItemId) : nullptr;
        const FName Slot = WearSlot == TEXT("Gloves") ? FName(TEXT("Arms")) : WearSlot;
        FBBBCharacterAppearancePart *Part = Parts.FindByPredicate(
            [Slot](const FBBBCharacterAppearancePart &Value)
            {
                return Value.Slot == Slot;
            });
        if (!Part)
        {
            Part = &Parts.AddDefaulted_GetRef();
            Part->Slot = Slot;
        }
        Part->ItemId = Item.Definition->ItemId;
        Part->InstanceId = Item.InstanceId;
        if (Entry && !Entry->Appearance.RequiredWearSlot.IsNone())
        {
            Part->bVisible = OccupiedWearSlots.Contains(Entry->Appearance.RequiredWearSlot);
        }
    }
    const auto ResourceFor = [&Context](const FBBBCharacterAppearancePart &Part) -> const FBBBAppearanceResource *
    {
        if (!Part.ItemId.IsNone())
        {
            const FBBBItemCatalogEntry *Entry = Context.Catalog ? Context.Catalog->FindItem(Part.ItemId) : nullptr;
            return Entry && Entry->Appearance.Part == Part.Slot ? &Entry->Appearance : nullptr;
        }
        return Context.Config.BaseResources.Find(Part.BaseResourceId);
    };
    for (FBBBCharacterAppearancePart &Part : Parts)
    {
        const FBBBCharacterAppearanceStyle *Parameters = Style.Styles.FindByPredicate(
            [&Part](const FBBBCharacterAppearanceStyle &Value)
            {
                return Value.InstanceId == Part.InstanceId && (Part.InstanceId.IsValid() || Value.Slot == Part.Slot);
            });
        if (Parameters)
        {
            Part.Colors = Parameters->Colors;
            Part.bCamouflage = Parameters->bCamouflage;
        }
    }
    FBBBCharacterAppearancePart *Legs = Parts.FindByPredicate(
        [](const FBBBCharacterAppearancePart &Part)
        {
            return Part.Slot == TEXT("Legs");
        });
    const FBBBCharacterAppearancePart *Boots = Parts.FindByPredicate(
        [](const FBBBCharacterAppearancePart &Part)
        {
            return Part.Slot == TEXT("Boots");
        });
    if (Legs && Boots)
    {
        const FBBBAppearanceResource *LegResource = ResourceFor(*Legs);
        const FBBBAppearanceResource *BootResource = ResourceFor(*Boots);
        Legs->bAlternateLegs = LegResource && BootResource && !BootResource->RequiredLegStyle.IsNone()
            && LegResource->LegStyle != BootResource->RequiredLegStyle && !LegResource->AlternateLegMesh.IsNull();
    }
    FBBBCharacterAppearancePart *Flag = Parts.FindByPredicate(
        [](const FBBBCharacterAppearancePart &Part)
        {
            return Part.Slot == TEXT("Flag");
        });
    if (Flag && !Flag->ItemId.IsNone() && Flag->bVisible)
    {
        const FBBBAppearanceResource *FlagResource = ResourceFor(*Flag);
        Flag->bVisible = false;
        const bool bValidPatch = FlagResource && !FlagResource->Patch.ContainsNaN()
            && FlagResource->Patch.X >= 0.0 && FlagResource->Patch.X <= 0.875
            && FlagResource->Patch.Y >= 0.0 && FlagResource->Patch.Y <= 0.875
            && FMath::IsNearlyEqual(FlagResource->Patch.X * 8.0, FMath::RoundToDouble(FlagResource->Patch.X * 8.0))
            && FMath::IsNearlyEqual(FlagResource->Patch.Y * 8.0, FMath::RoundToDouble(FlagResource->Patch.Y * 8.0));
        if (FlagResource && !bValidPatch)
        {
            UE_LOG(LogTemp, Error, TEXT("国旗目录坐标无效 已隐藏 Item=%s"), *Flag->ItemId.ToString());
        }
        for (const FName TargetSlot : {FName(TEXT("Vest")), FName(TEXT("Body"))})
        {
            FBBBCharacterAppearancePart *Target = Parts.FindByPredicate(
                [TargetSlot](const FBBBCharacterAppearancePart &Part)
                {
                    return Part.Slot == TargetSlot;
                });
            const FBBBAppearanceResource *TargetResource = Target ? ResourceFor(*Target) : nullptr;
            if (bValidPatch && Target && Target->bVisible && TargetResource && TargetResource->bSupportsPatch)
            {
                Flag->bVisible = true;
                Target->bPatchEnabled = true;
                Target->Patch = FlagResource->Patch;
                break;
            }
        }
    }
    if (Flag && Flag->ItemId.IsNone())
    {
        Flag->bVisible = false;
    }
    if (Selection.Revision == 0 || Parts != Selection.Parts
        || Style.Dirt != Selection.Dirt || Style.Weathering != Selection.Weathering)
    {
        Selection.Parts = MoveTemp(Parts);
        Selection.Dirt = Style.Dirt;
        Selection.Weathering = Style.Weathering;
        ++Selection.Revision;
    }
}
