#include "BBBWork/UBBBNexus/Character/Logic/System/AppearanceSystem/Processors/BBBCharacterAppearanceDisplayProcessor.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/AppearanceSystem/DomainData/Context/BBBCharacterAppearanceUpdateContext.h"
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "BBBWork/UBBBNexus/Item/Catalog/BBBItemCatalog.h"
#include "Engine/SkeletalMesh.h"
#include "Components/SkeletalMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"

void FBBBCharacterAppearanceDisplayProcessor::Update(FBBBCharacterAppearanceUpdateContext &Context) const
{
    const auto &Selection = Context.Data.Appearance.ReadAppearanceSelectionState().Snapshot;
    auto &Display = Context.Data.Appearance.DisplayState;
    if (Selection.Revision == 0 || Display.AppliedRevision == Selection.Revision)
    {
        return;
    }
    if (Context.Character.GetNetMode() == NM_DedicatedServer)
    {
        Display.Parts.Reset();
        Display.AppliedRevision = Selection.Revision;
        Display.bApplied = true;
        return;
    }
    Display.Parts.Reset();
    for (const FBBBCharacterAppearancePart &Part : Selection.Parts)
    {
        FBBBCharacterAppearanceDisplayPart &Output = Display.Parts.AddDefaulted_GetRef();
        Output.Slot = Part.Slot;
        const FBBBAppearanceResource *Resource = Context.Config.BaseResources.Find(Part.BaseResourceId);
        if (!Part.ItemId.IsNone())
        {
            const FBBBItemCatalogEntry *Entry = Context.Catalog ? Context.Catalog->FindItem(Part.ItemId) : nullptr;
            Resource = Entry && Entry->Appearance.Part == Part.Slot ? &Entry->Appearance : nullptr;
        }
        if (!Part.bVisible)
        {
            continue;
        }
        if (Resource)
        {
            const TSoftObjectPtr<USkeletalMesh> &Mesh =
                Part.bAlternateLegs ? Resource->AlternateLegMesh : Resource->Mesh;
            Output.Mesh = Mesh.LoadSynchronous();
        }
        const bool bMissingHiddenPatch = Resource && !Part.bPatchEnabled && !Resource->PatchMaterialSlot.IsNone() &&
                                         !Resource->HiddenPatchMaterial.LoadSynchronous();
        const bool bMissing = !Resource || (!Resource->Mesh.IsNull() && !Output.Mesh) || bMissingHiddenPatch;
        if (bMissing)
        {
            const FName *DefaultId = Context.Config.DefaultBaseParts.Find(Part.Slot);
            Resource = DefaultId ? Context.Config.BaseResources.Find(*DefaultId) : nullptr;
            Output.Mesh = Resource ? Resource->Mesh.LoadSynchronous() : nullptr;
            Output.bFallback = true;
            UE_LOG(LogTemp, Error, TEXT("外观资源缺失 已采用基础回退 Part=%s Item=%s"), *Part.Slot.ToString(),
                   *Part.ItemId.ToString());
        }
        if (!Resource)
        {
            continue;
        }
        for (const auto &Attachment : Resource->Attachments)
        {
            if (USkeletalMesh *Mesh = Attachment.Value.LoadSynchronous())
            {
                Output.Attachments.Add(Attachment.Key, Mesh);
            }
            else if (!Attachment.Value.IsNull())
            {
                UE_LOG(LogTemp, Error, TEXT("外观附件加载失败 Part=%s Attachment=%s"), *Part.Slot.ToString(),
                       *Attachment.Key.ToString());
            }
        }
        if (!Output.Mesh)
        {
            continue;
        }
        for (const FSkeletalMaterial &Material : Output.Mesh->GetMaterials())
        {
            UMaterialInterface *Parent = Material.MaterialInterface.Get();
            if (!Part.bPatchEnabled && !Resource->PatchMaterialSlot.IsNone() &&
                Material.MaterialSlotName == Resource->PatchMaterialSlot)
            {
                Parent = Resource->HiddenPatchMaterial.LoadSynchronous();
                if (!Parent)
                {
                    UE_LOG(LogTemp, Error, TEXT("隐藏国旗材质缺失 Part=%s"), *Part.Slot.ToString());
                }
            }
            UMaterialInstanceDynamic *Dynamic =
                Parent ? UMaterialInstanceDynamic::Create(Parent, &Context.Character) : nullptr;
            Output.Materials.Add(Dynamic ? Dynamic : Parent);
            if (!Dynamic)
            {
                continue;
            }
            for (int32 Index = 0; Index < Part.Colors.Num() && Index < Resource->ColorParameters.Num(); ++Index)
            {
                Dynamic->SetVectorParameterValue(Resource->ColorParameters[Index], Part.Colors[Index]);
            }
            if (!Resource->CamouflageParameter.IsNone())
            {
                Dynamic->SetScalarParameterValue(Resource->CamouflageParameter, Part.bCamouflage ? 1.0f : 0.0f);
            }
            if (!Resource->PatchEnabledParameter.IsNone())
            {
                Dynamic->SetScalarParameterValue(Resource->PatchEnabledParameter, Part.bPatchEnabled ? 1.0f : 0.0f);
            }
            if (!Resource->PatchParameter.IsNone())
            {
                Dynamic->SetVectorParameterValue(Resource->PatchParameter,
                                                 FLinearColor(Part.Patch.X, Part.Patch.Y, 0.0f, 0.0f));
            }
            if (!Resource->DirtParameter.IsNone())
            {
                Dynamic->SetScalarParameterValue(Resource->DirtParameter, Selection.Dirt);
            }
            if (!Resource->WeatheringParameter.IsNone())
            {
                Dynamic->SetScalarParameterValue(Resource->WeatheringParameter, Selection.Weathering);
            }
        }
    }
    Display.bApplied = Context.Character.ApplyAppearanceDisplay(Display.Parts);
    Display.AppliedRevision = Selection.Revision;
    if (!Display.bApplied)
    {
        UE_LOG(LogTemp, Error, TEXT("蓝图外观显示失败 物品位置保持不变 Character=%s"), *Context.Character.GetName());
    }
}

void FBBBCharacterAppearanceDisplayProcessor::Shutdown(FBBBCharacterAppearanceUpdateContext &Context)
{
    auto &Display = Context.Data.Appearance.DisplayState;
    Display.Parts.Reset();
    Display.AppliedRevision = 0;
    Display.bApplied = false;
}

bool FBBBCharacterAppearanceDisplayProcessor::ApplyDisplay(ABBBCharacter &Character,
                                                           const TArray<FBBBCharacterAppearanceDisplayPart> &Parts,
                                                           const FBBBCharacterAppearanceConfig &Config)
{
    TArray<USkeletalMeshComponent *> Components;
    Character.GetComponents(Components);
    TSet<FName> Updated;
    for (const auto &Part : Parts)
    {
        const FName *Name = Config.ComponentNames.Find(Part.Slot);
        if (Name)
        {
            auto *const *Found = Components.FindByPredicate(
                [Name](const USkeletalMeshComponent *Component)
                {
                    return Component->GetFName() == *Name;
                });
            if (!ensureMsgf(Found, TEXT("外观显示组件缺失 %s"), *Name->ToString()))
            {
                return false;
            }
            USkeletalMeshComponent *Component = *Found;
            Component->SetSkeletalMesh(Part.Mesh);
            Component->SetLeaderPoseComponent(Character.GetMesh());
            Component->SetVisibility(Part.Mesh != nullptr);
            Component->SetHiddenInGame(Part.Mesh == nullptr);
            Component->EmptyOverrideMaterials();
            for (int32 Index = 0; Index < Part.Materials.Num(); ++Index)
            {
                Component->SetMaterial(Index, Part.Materials[Index]);
            }
            Updated.Add(*Name);
        }
        for (const auto &Attachment : Part.Attachments)
        {
            auto *const *Attached = Components.FindByPredicate(
                [&Attachment](const USkeletalMeshComponent *Candidate)
                {
                    return Candidate->GetFName() == Attachment.Key;
                });
            if (!ensureMsgf(Attached, TEXT("外观附件挂点缺失 %s"), *Attachment.Key.ToString()))
            {
                return false;
            }
            (*Attached)->SetSkeletalMesh(Attachment.Value);
            if (!ensureMsgf(Character.GetMesh()->DoesSocketExist(Attachment.Key),
                            TEXT("外观附件骨骼挂点缺失 %s"), *Attachment.Key.ToString()))
            {
                return false;
            }
            (*Attached)->SetLeaderPoseComponent(nullptr);
            (*Attached)->AttachToComponent(Character.GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale,
                                            Attachment.Key);
            (*Attached)->SetRelativeTransform(FTransform::Identity);
            (*Attached)->SetVisibility(Attachment.Value != nullptr);
            (*Attached)->SetHiddenInGame(Attachment.Value == nullptr);
            Updated.Add(Attachment.Key);
        }
    }
    for (const auto &Mapping : Config.ComponentNames)
    {
        if (Updated.Contains(Mapping.Value))
        {
            continue;
        }
        for (USkeletalMeshComponent *Component : Components)
        {
            if (Component->GetFName() == Mapping.Value)
            {
                Component->SetSkeletalMesh(nullptr);
                Component->SetVisibility(false);
                Component->SetHiddenInGame(true);
                Component->EmptyOverrideMaterials();
            }
        }
    }
    return true;
}
