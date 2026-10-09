#include "BBBWork/UBBBNexus/Character/Logic/System/AppearanceSystem/Processors/BBBCharacterAppearanceResourceProcessor.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/AppearanceSystem/DomainData/Context/BBBCharacterAppearanceUpdateContext.h"
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "BBBWork/UBBBNexus/Item/Catalog/BBBItemCatalog.h"
#include "Engine/SkeletalMesh.h"

namespace
{
    bool PreparePart(FBBBCharacterAppearanceDisplayPart &Output, const FBBBAppearanceResource *Resource,
        const bool bAlternate, const bool bPatchEnabled)
    {
        if (!Resource)
        {
            Output.Mesh = nullptr;
            Output.Materials.Reset();
            Output.Attachments.Reset();
            return false;
        }
        const auto &Reference = bAlternate ? Resource->AlternateLegMesh : Resource->Mesh;
        USkeletalMesh *Mesh = Reference.LoadSynchronous();
        if (Output.Mesh != Mesh)
        {
            Output.Materials.Reset();
        }
        Output.Mesh = Mesh;
        bool bReady = Reference.IsNull() || Mesh;
        if (!bPatchEnabled && !Resource->PatchMaterialSlot.IsNone())
        {
            bReady &= Resource->HiddenPatchMaterial.LoadSynchronous() != nullptr;
        }
        TMap<FName, FBBBCharacterAppearanceDisplayAttachment> Previous = MoveTemp(Output.Attachments);
        for (const auto &Attachment : Resource->Attachments)
        {
            USkeletalMesh *AttachedMesh = Attachment.Value.LoadSynchronous();
            bReady &= Attachment.Value.IsNull() || AttachedMesh;
            if (!AttachedMesh)
            {
                continue;
            }
            auto &Attached = Output.Attachments.Add(Attachment.Key);
            if (const auto *Cached = Previous.Find(Attachment.Key); Cached && Cached->Mesh == AttachedMesh)
            {
                Attached.Materials = Cached->Materials;
            }
            Attached.Mesh = AttachedMesh;
        }
        return bReady;
    }

    const FBBBAppearanceResource *BaseResource(const FBBBCharacterAppearanceConfig &Config, const FName Slot)
    {
        const FName *Id = Config.DefaultBaseParts.Find(Slot);
        const auto *Resource = Id ? Config.BaseResources.Find(*Id) : nullptr;
        return Resource && Resource->Part == Slot ? Resource : nullptr;
    }

    bool BaseAlternateLegs(const FBBBCharacterAppearanceConfig &Config)
    {
        const auto *Legs = BaseResource(Config, TEXT("Legs"));
        const auto *Boots = BaseResource(Config, TEXT("Boots"));
        return Legs && Boots && !Boots->RequiredLegStyle.IsNone()
            && Legs->LegStyle != Boots->RequiredLegStyle && !Legs->AlternateLegMesh.IsNull();
    }
}

void FBBBCharacterAppearanceResourceProcessor::Update(FBBBCharacterAppearanceUpdateContext &Context) const
{
    const auto &Snapshot = Context.Data.Appearance.ReadCharacterAppearanceSelectionState().Snapshot;
    auto &Display = Context.Data.Appearance.DisplayState;
    const float Now = Context.Data.External.ReadWorldState().WorldTimeSeconds;
    if (Snapshot.Revision == 0 || (Display.bApplied && Display.AppliedRevision == Snapshot.Revision)
        || (Display.PreparedRevision == Snapshot.Revision && Now < Display.RetryAfterTime))
    {
        return;
    }
    Context.bPrepareDisplay = true;
    Context.bReportFailure = Display.AttemptedRevision != Snapshot.Revision;
    Display.bApplied = false;
    Display.PreparedRevision = Snapshot.Revision;
    if (Context.Character.GetNetMode() == NM_DedicatedServer)
    {
        return;
    }
    TArray<FBBBCharacterAppearanceDisplayPart> Previous = MoveTemp(Display.Parts);
    for (const auto &Part : Snapshot.Parts)
    {
        auto &Output = Display.Parts.AddDefaulted_GetRef();
        if (const auto *Cached = Previous.FindByPredicate([&Part](const auto &Value) { return Value.Slot == Part.Slot; }))
        {
            Output = *Cached;
        }
        Output.Slot = Part.Slot;
        Output.bFallback = false;
        const FBBBAppearanceResource *Resource = Context.Config.BaseResources.Find(Part.BaseResourceId);
        if (!Part.ItemId.IsNone())
        {
            const auto *Entry = Context.Catalog ? Context.Catalog->FindItem(Part.ItemId) : nullptr;
            Resource = Entry ? &Entry->Appearance : nullptr;
        }
        if (Resource && Resource->Part != Part.Slot)
        {
            Resource = nullptr;
        }
        if (!Part.bVisible)
        {
            Output.Mesh = nullptr;
            Output.Materials.Reset();
            Output.Attachments.Reset();
            Context.Resources.Add(nullptr);
            continue;
        }
        if (!PreparePart(Output, Resource, Part.bAlternateLegs, Part.bPatchEnabled))
        {
            Context.bResourcesReady = false;
            Resource = BaseResource(Context.Config, Part.Slot);
            PreparePart(Output, Resource, Part.Slot == TEXT("Legs") && BaseAlternateLegs(Context.Config), false);
            Output.bFallback = true;
            if (Context.bReportFailure)
            {
                UE_LOG(LogTemp, Error, TEXT("外观资源不完整 已准备基础回退 Part=%s Item=%s"),
                    *Part.Slot.ToString(), *Part.ItemId.ToString());
            }
        }
        Context.Resources.Add(Resource);
    }
    Previous = MoveTemp(Display.FallbackParts);
    TArray<FName> Slots;
    Context.Config.DefaultBaseParts.GetKeys(Slots);
    Slots.Sort(FNameLexicalLess());
    for (const FName Slot : Slots)
    {
        auto &Output = Display.FallbackParts.AddDefaulted_GetRef();
        if (const auto *Cached = Previous.FindByPredicate([Slot](const auto &Value) { return Value.Slot == Slot; }))
        {
            Output = *Cached;
        }
        Output.Slot = Slot;
        Output.bFallback = true;
        const auto *Resource = BaseResource(Context.Config, Slot);
        PreparePart(Output, Resource, Slot == TEXT("Legs") && BaseAlternateLegs(Context.Config), false);
        Context.FallbackResources.Add(Resource);
    }
}
