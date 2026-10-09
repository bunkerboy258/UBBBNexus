#include "BBBWork/UBBBNexus/Character/Logic/System/AppearanceSystem/Processors/BBBCharacterAppearanceMaterialProcessor.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/AppearanceSystem/DomainData/Context/BBBCharacterAppearanceUpdateContext.h"
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "Engine/SkeletalMesh.h"
#include "Materials/MaterialInstanceDynamic.h"

namespace
{
    bool UpdateMaterials(ABBBCharacter &Character, USkeletalMesh *Mesh,
        TArray<TObjectPtr<UMaterialInterface>> &Materials, const FBBBAppearanceResource &Resource,
        const FBBBCharacterAppearancePart *Parameters, const float Dirt, const float Weathering)
    {
        if (!Mesh)
        {
            Materials.Reset();
            return true;
        }
        const bool bPatchEnabled = Parameters && Parameters->bPatchEnabled;
        Materials.SetNum(Mesh->GetMaterials().Num());
        bool bReady = true;
        for (int32 Index = 0; Index < Materials.Num(); ++Index)
        {
            const auto &Slot = Mesh->GetMaterials()[Index];
            UMaterialInterface *Parent = Slot.MaterialInterface.Get();
            if (!bPatchEnabled && !Resource.PatchMaterialSlot.IsNone() && Slot.MaterialSlotName == Resource.PatchMaterialSlot)
            {
                Parent = Resource.HiddenPatchMaterial.LoadSynchronous();
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
            bReady &= UpdateMaterials(Context.Character, Part.Mesh, Part.Materials, Resource, Parameters,
                Snapshot.Dirt, Snapshot.Weathering);
            for (auto &Attachment : Part.Attachments)
            {
                bReady &= UpdateMaterials(Context.Character, Attachment.Value.Mesh, Attachment.Value.Materials,
                    Resource, Parameters, Snapshot.Dirt, Snapshot.Weathering);
            }
        }
        return bReady;
    };
    Context.bResourcesReady &= UpdateParts(Display.Parts, Context.Resources, false);
    UpdateParts(Display.FallbackParts, Context.FallbackResources, true);
}
