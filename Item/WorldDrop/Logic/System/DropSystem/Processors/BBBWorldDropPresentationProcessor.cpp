#include "BBBWork/UBBBNexus/Item/WorldDrop/Logic/System/DropSystem/Processors/BBBWorldDropPresentationProcessor.h"
#include "BBBWork/UBBBNexus/Item/WorldDrop/Logic/System/DropSystem/DomainData/Context/BBBWorldDropUpdateContext.h"
#include "BBBWork/UBBBNexus/Item/WorldDrop/BBBWorldDropItem.h"
#include "BBBWork/UBBBNexus/Item/BBBItemDefinition.h"
#include "Components/MaterialBillboardComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Engine/Texture2D.h"
void FBBBWorldDropPresentationProcessor::Update(FBBBWorldDropUpdateContext &Context) const
{
    auto &Drop = Context.Drop;
    const auto &Snapshot = Context.Data.Drop.ReadWorldDropState().Snapshot;
    auto &Presentation = Context.Data.Drop.PresentationState;
    const auto &Config = Drop.Config;
    if (Presentation.RenderedDefinition != Snapshot.Definition)
    {
        Presentation.RenderedDefinition = Snapshot.Definition;
        Presentation.IconMaterial = nullptr;
        Drop.Icon->Elements.Reset();
        if (Snapshot.Definition && Snapshot.Definition->Icon)
        {
            UMaterialInterface *Base = Config.IconMaterial.LoadSynchronous();
            if (ensureMsgf(Base, TEXT("世界掉落物缺少正式图标材质 %s"), *Drop.GetPathName()))
            {
                Presentation.IconMaterial = UMaterialInstanceDynamic::Create(Base, &Drop);
                Presentation.IconMaterial->SetTextureParameterValue(TEXT("IconTexture"), Snapshot.Definition->Icon);
                const UTexture2D *Texture = Snapshot.Definition->Icon;
                const float Aspect = float(Texture->GetSizeX()) / FMath::Max(1, Texture->GetSizeY());
                Drop.Icon->AddElement(Presentation.IconMaterial, nullptr, false,
                    Config.IconHeight * Aspect * 0.5f, Config.IconHeight * 0.5f, nullptr);
            }
        }
        Drop.Icon->MarkRenderStateDirty();
    }
    const bool bVisible = Snapshot.Definition && Snapshot.bLanded && !Snapshot.bConsumed;
    Drop.Icon->SetVisibility(bVisible && Presentation.IconMaterial);
    Drop.Beam->SetVisibility(bVisible);
    const FVector Location = Drop.GetActorLocation();
    Drop.Icon->SetWorldLocation(Location + FVector(0, 0, Config.IconLift));
    Drop.Beam->SetWorldLocationAndRotation(Location + FVector(0, 0, Config.BeamHeight * 0.5f), FRotator::ZeroRotator);
    Drop.Beam->SetWorldScale3D(FVector(Config.BeamRadius / 50.0f, Config.BeamRadius / 50.0f, Config.BeamHeight / 100.0f));
}
