#include "BBBWork/UBBBNexus/Character/Logic/System/AppearanceSystem/Processors/BBBCharacterAppearanceLoadProcessor.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/AppearanceSystem/DomainData/Context/BBBCharacterAppearanceUpdateContext.h"
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "BBBWork/UBBBNexus/Item/Wearable/Catalog/BBBWearableCatalog.h"
#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"

namespace
{
    void Gather(TArray<FSoftObjectPath> &Paths, const FBBBAppearanceResource &Resource,
        const bool bAlternate, const bool bPatchEnabled)
    {
        const auto Add = [&Paths](const FSoftObjectPath &Path)
        {
            if (!Path.IsNull())
            {
                Paths.AddUnique(Path);
            }
        };
        Add((bAlternate ? Resource.AlternateLegMesh : Resource.Mesh).ToSoftObjectPath());
        if (!bPatchEnabled && !Resource.PatchMaterialSlot.IsNone())
        {
            Add(Resource.HiddenPatchMaterial.ToSoftObjectPath());
        }
        for (const auto &Attachment : Resource.Attachments)
        {
            Add(Attachment.Value.ToSoftObjectPath());
        }
    }
}

void FBBBCharacterAppearanceLoadProcessor::Update(FBBBCharacterAppearanceUpdateContext &Context) const
{
    const auto &Snapshot = Context.Data.Appearance.ReadCharacterAppearanceSelectionState().Snapshot;
    const auto &Display = Context.Data.Appearance.ReadCharacterAppearanceDisplayState();
    if (Context.Character.GetNetMode() == NM_DedicatedServer || Snapshot.Revision == 0
        || (Display.bApplied && Display.AppliedRevision == Snapshot.Revision))
    {
        return;
    }
    auto &Load = Context.Data.Appearance.LoadState;
    const float Now = Context.Data.External.ReadWorldState().WorldTimeSeconds;
    const bool bRetry = Display.PreparedRevision == Snapshot.Revision && !Display.bApplied
        && Display.RetryAfterTime > 0 && Display.RetryAfterTime != Load.RequestedRetryTime && Now >= Display.RetryAfterTime
        && (!Load.Handle || Load.Handle->HasLoadCompleted());
    if (Load.RequestedRevision != Snapshot.Revision || bRetry)
    {
        if (Load.Handle)
        {
            Load.Handle->CancelHandle();
        }
        Load.RequestedRevision = Snapshot.Revision;
        Load.RequestedRetryTime = bRetry ? Display.RetryAfterTime : 0.0f;
        TArray<FSoftObjectPath> Paths;
        for (const auto &Part : Snapshot.Parts)
        {
            if (!Part.bVisible)
            {
                continue;
            }
            const auto *Resource = Context.Config.BaseResources.Find(Part.BaseResourceId);
            if (!Part.ItemId.IsNone())
            {
                const auto *Entry = Context.Catalog ? Context.Catalog->FindItem(Part.ItemId) : nullptr;
                Resource = Entry ? &Entry->Appearance : nullptr;
            }
            if (Resource && Resource->Part == Part.Slot)
            {
                Gather(Paths, *Resource, Part.bAlternateLegs, Part.bPatchEnabled);
            }
        }
        for (const auto &Part : Context.Config.DefaultBaseParts)
        {
            if (const auto *Resource = Context.Config.BaseResources.Find(Part.Value))
            {
                Gather(Paths, *Resource, false, false);
                if (!Resource->AlternateLegMesh.IsNull())
                {
                    Paths.AddUnique(Resource->AlternateLegMesh.ToSoftObjectPath());
                }
            }
        }
        Load.Handle = Paths.IsEmpty() ? nullptr : UAssetManager::GetStreamableManager().RequestAsyncLoad(Paths);
    }
    Context.bWaitingForResources = Load.Handle && !Load.Handle->HasLoadCompleted();
}

void FBBBCharacterAppearanceLoadProcessor::Shutdown(FBBBCharacterAppearanceUpdateContext &Context) const
{
    auto &Load = Context.Data.Appearance.LoadState;
    if (Load.Handle)
    {
        Load.Handle->CancelHandle();
    }
    Load.Handle.Reset();
    Load.RequestedRevision = 0;
    Load.RequestedRetryTime = 0.0f;
}
