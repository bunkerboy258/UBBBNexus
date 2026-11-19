#include "BBBWork/UBBBNexus/Character/Logic/System/AppearanceSystem/Processors/BBBCharacterAppearanceMaterialProcessor.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/AppearanceSystem/DomainData/Context/BBBCharacterAppearanceUpdateContext.h"
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "Engine/SkeletalMesh.h"
#include "Engine/Texture.h"
#include "Materials/MaterialInstanceDynamic.h"

namespace
{
    bool ResolvePalette(const USkeletalMesh &Mesh, const FBBBCharacterAppearancePreset *Preset,
        const FBBBCharacterAppearancePart *Parameters, TMap<FName, UTexture *> &Textures)
    {
        if (!Preset)
        {
            return !Parameters || Parameters->PaletteChoices.IsEmpty();
        }
        if (Parameters && Parameters->PaletteChoices.Num() != Preset->Palettes.Num())
        {
            return false;
        }
        for (int32 Index = 0; Index < Preset->Palettes.Num(); ++Index)
        {
            const auto &Palette = Preset->Palettes[Index];
            const int32 Choice = Parameters ? Parameters->PaletteChoices[Index] : Palette.DefaultIndex;
            if (Choice == INDEX_NONE)
            {
                continue;
            }
            UTexture *Texture = Palette.Textures.IsValidIndex(Choice) ? Palette.Textures[Choice].Get() : nullptr;
            if (!Texture || !Mesh.GetMaterials().ContainsByPredicate([&Palette](const FSkeletalMaterial &Slot)
            {
                return Slot.MaterialSlotName == Palette.MaterialSlot;
            }))
            {
                return false;
            }
            Textures.Add(Palette.MaterialSlot, Texture);
        }
        return true;
    }

    bool UpdateMaterials(ABBBCharacter &Character, USkeletalMesh *Mesh,
        TArray<TObjectPtr<UMaterialInterface>> &Materials, const FBBBAppearanceResource &Resource,
        const FBBBCharacterAppearancePart *Parameters, const float Dirt, const float Weathering,
        const bool bApplyOverrides, const TMap<FName, UTexture *> &Textures)
    {
        if (!Mesh)
        {
            Materials.Reset();
            return !bApplyOverrides || Resource.MaterialOverrides.IsEmpty();
        }
        const bool bPatchEnabled = Parameters && Parameters->bPatchEnabled;
        const auto &Slots = Mesh->GetMaterials();
        Materials.SetNum(Slots.Num());
        bool bReady = true;
        if (bApplyOverrides)
        {
            for (const auto &Override : Resource.MaterialOverrides)
            {
                bReady &= IsValid(Override.Value.Get()) && Slots.ContainsByPredicate(
                    [&Override](const FSkeletalMaterial &Slot) { return Slot.MaterialSlotName == Override.Key; });
            }
        }
        for (int32 Index = 0; Index < Materials.Num(); ++Index)
        {
            const auto &Slot = Slots[Index];
            UMaterialInterface *Parent = Slot.MaterialInterface.Get();
            if (const auto *Override = bApplyOverrides ? Resource.MaterialOverrides.Find(Slot.MaterialSlotName) : nullptr)
            {
                Parent = Override->Get();
            }
            if (!bPatchEnabled && !Resource.PatchMaterialSlot.IsNone() && Slot.MaterialSlotName == Resource.PatchMaterialSlot)
            {
                Parent = Resource.HiddenPatchMaterial.Get();
            }
            UMaterialInstanceDynamic *Dynamic = Cast<UMaterialInstanceDynamic>(Materials[Index]);
            if (!Dynamic || Dynamic->Parent != Parent)
            {
                Dynamic = Parent ? UMaterialInstanceDynamic::Create(Parent, &Character) : nullptr;
            }
            Materials[Index] = Dynamic;
            if (!Dynamic)
            {
                bReady = false;
                continue;
            }
            Dynamic->ClearParameterValues();
            if (UTexture *const *Texture = Textures.Find(Slot.MaterialSlotName))
            {
                Dynamic->SetTextureParameterValue(TEXT("Color"), *Texture);
            }
            for (int32 Color = 0; Color < Resource.ColorParameters.Num(); ++Color)
            {
                FLinearColor Value = FLinearColor::White;
                if (Parameters && Parameters->Colors.IsValidIndex(Color))
                {
                    Value = Parameters->Colors[Color];
                }
                if (!(Parameters && Parameters->Colors.IsValidIndex(Color))
                    && !Parent->GetVectorParameterValue(FMaterialParameterInfo(Resource.ColorParameters[Color]), Value))
                {
                    continue;
                }
                Dynamic->SetVectorParameterValue(Resource.ColorParameters[Color], Value);
            }
            if (!Resource.CamouflageParameter.IsNone())
            {
                Dynamic->SetScalarParameterValue(Resource.CamouflageParameter,
                    Parameters && Parameters->bCamouflage ? 1.0f : 0.0f);
            }
            if (!Resource.PatchEnabledParameter.IsNone())
            {
                Dynamic->SetScalarParameterValue(Resource.PatchEnabledParameter, bPatchEnabled ? 1.0f : 0.0f);
            }
            if (!Resource.PatchParameter.IsNone())
            {
                const FVector2D Patch = Parameters ? Parameters->Patch : FVector2D::ZeroVector;
                Dynamic->SetVectorParameterValue(Resource.PatchParameter, FLinearColor(Patch.X, Patch.Y, 0.0f, 0.0f));
            }
            if (!Resource.DirtParameter.IsNone())
            {
                Dynamic->SetScalarParameterValue(Resource.DirtParameter, Dirt);
            }
            if (!Resource.WeatheringParameter.IsNone())
            {
                Dynamic->SetScalarParameterValue(Resource.WeatheringParameter, Weathering);
            }
        }
        return bReady;
    }
}

void FBBBCharacterAppearanceMaterialProcessor::Update(FBBBCharacterAppearanceUpdateContext &Context) const
{
    if (!Context.bPrepareDisplay || Context.Character.GetNetMode() == NM_DedicatedServer)
    {
        return;
    }
    const auto &Snapshot = Context.Data.Appearance.ReadCharacterAppearanceSelectionState().Snapshot;
    auto &Display = Context.Data.Appearance.DisplayState;
    const auto UpdateParts = [&Context, &Snapshot](auto &Parts, const auto &Resources, const bool bFallback)
    {
        bool bReady = true;
        for (int32 Index = 0; Index < Parts.Num(); ++Index)
        {
            if (!Resources.IsValidIndex(Index) || !Resources[Index])
            {
                continue;
            }
            auto &Part = Parts[Index];
            const auto &Resource = *Resources[Index];
            const auto *Parameters = !bFallback && !Part.bFallback && Snapshot.Parts.IsValidIndex(Index)
                ? &Snapshot.Parts[Index] : nullptr;
            const auto *BaseId = Parameters ? &Parameters->BaseResourceId : Context.Config.DefaultBaseParts.Find(Part.Slot);
            TMap<FName, UTexture *> Textures;
            if (Part.Mesh)
            {
                bReady &= ResolvePalette(*Part.Mesh, BaseId ? Context.Config.MaterialPresets.Find(*BaseId) : nullptr,
                    Parameters, Textures);
            }
            bReady &= UpdateMaterials(Context.Character, Part.Mesh, Part.Materials, Resource, Parameters,
                Snapshot.Dirt, Snapshot.Weathering, true, Textures);
            for (auto &Attachment : Part.Attachments)
            {
                bReady &= UpdateMaterials(Context.Character, Attachment.Value.Mesh, Attachment.Value.Materials,
                    Resource, Parameters, Snapshot.Dirt, Snapshot.Weathering, false, {});
            }
        }
        return bReady;
    };
    Context.bResourcesReady &= UpdateParts(Display.Parts, Context.Resources, false);
    UpdateParts(Display.FallbackParts, Context.FallbackResources, true);
}
