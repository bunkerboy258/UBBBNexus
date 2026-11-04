#include "BBBWork/UBBBNexus/Item/WorldDrop/Logic/System/DropSystem/Processors/BBBWorldDropPresentationProcessor.h"
#include "BBBWork/UBBBNexus/Item/WorldDrop/Logic/System/DropSystem/DomainData/Context/BBBWorldDropUpdateContext.h"
#include "BBBWork/UBBBNexus/Item/WorldDrop/BBBWorldDropItem.h"
#include "BBBWork/UBBBNexus/Item/BBBItemDefinition.h"
#include "Components/MaterialBillboardComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Engine/Texture2D.h"
#include "Engine/World.h"
#include "Engine/StaticMesh.h"
void FBBBWorldDropPresentationProcessor::Update(FBBBWorldDropUpdateContext &Context) const
{
    auto &Drop = Context.Drop;
    const auto &Snapshot = Context.Data.Drop.ReadWorldDropState().Snapshot;
    auto &Presentation = Context.Data.Drop.PresentationState;
    const auto &Config = Drop.Config;
    const float Aspect = Snapshot.Definition && Snapshot.Definition->Icon
        ? float(Snapshot.Definition->Icon->GetSizeX()) / FMath::Max(1, Snapshot.Definition->Icon->GetSizeY()) : 1.0f;
    const float IconWidth = FMath::Min(FMath::Max(1.0f, Config.IconMaxWidth), FMath::Max(1.0f, Config.IconHeight) * Aspect);
    const float IconHeight = IconWidth / FMath::Max(0.01f, Aspect);
    if (Presentation.RenderedDefinition != Snapshot.Definition)
    {
        Presentation.RenderedDefinition = Snapshot.Definition;
        if (Drop.GetNetMode() != NM_DedicatedServer && Snapshot.Definition)
        {
            auto *Mesh = Config.AirborneMesh.LoadSynchronous();
            Drop.Airborne->SetStaticMesh(Mesh);
            if (ensureMsgf(Mesh, TEXT("世界掉落物缺少空中实体外观 %s"), *Drop.GetPathName()))
            {
                const auto Bounds = Mesh->GetBounds();
                const float Scale = FMath::Max(1.0f, Config.AirborneSize)
                    / FMath::Max(1.0f, Bounds.BoxExtent.GetMax() * 2.0f);
                Drop.Airborne->SetRelativeScale3D(FVector(Scale));
                Drop.Airborne->SetRelativeLocation(-Bounds.Origin * Scale);
            }
        }
        Presentation.IconMaterial = nullptr;
        Drop.Icon->Elements.Reset();
        if (Snapshot.Definition && Snapshot.Definition->Icon)
        {
            UMaterialInterface *Base = Config.IconMaterial.LoadSynchronous();
            if (ensureMsgf(Base, TEXT("世界掉落物缺少正式图标材质 %s"), *Drop.GetPathName()))
            {
                Presentation.IconMaterial = UMaterialInstanceDynamic::Create(Base, &Drop);
                Presentation.IconMaterial->SetTextureParameterValue(TEXT("IconTexture"), Snapshot.Definition->Icon);
                Drop.Icon->AddElement(Presentation.IconMaterial, nullptr, false,
                    IconWidth * 0.5f, IconHeight * 0.5f, nullptr);
            }
        }
        if (auto *Material = Config.BeamMaterial.LoadSynchronous())
        {
            Presentation.BeamMaterial = UMaterialInstanceDynamic::Create(Material, &Drop);
            Drop.Beam->SetMaterial(0, Presentation.BeamMaterial);
        }
        if (auto *Material = Config.GroundGlowMaterial.LoadSynchronous())
        {
            Presentation.GroundGlowMaterial = UMaterialInstanceDynamic::Create(Material, &Drop);
            Drop.GroundGlow->SetMaterial(0, Presentation.GroundGlowMaterial);
        }
        ensureMsgf(Presentation.BeamMaterial && Presentation.GroundGlowMaterial,
            TEXT("世界掉落物缺少光束或落点材质 %s"), *Drop.GetPathName());
        Drop.Icon->MarkRenderStateDirty();
    }
    const bool bVisible = Snapshot.Definition && Snapshot.bLanded && !Snapshot.bConsumed;
    const bool bAirborne = Snapshot.Definition && Snapshot.Quantity > 0 && !Snapshot.bLanded
        && !Snapshot.bConsumed && Drop.GetNetMode() != NM_DedicatedServer;
    Drop.Airborne->SetVisibility(bAirborne && Drop.Airborne->GetStaticMesh());
    const double Now = Drop.GetWorld()->GetTimeSeconds();
    if (bVisible && !Presentation.bWasVisible)
    {
        Presentation.VisibleSince = Now;
    }
    Presentation.bWasVisible = bVisible;
    float Opacity = bVisible ? 1.0f : 0.0f;
    if (bVisible && Drop.GetWorld()->IsGameWorld() && Config.FadeInDuration > 0.0f)
    {
        Opacity = FMath::SmoothStep(0.0f, 1.0f, float(Now - Presentation.VisibleSince) / Config.FadeInDuration);
    }
    for (auto *Material : {Presentation.IconMaterial.Get(), Presentation.BeamMaterial.Get(), Presentation.GroundGlowMaterial.Get()})
    {
        if (Material)
        {
            Material->SetScalarParameterValue(TEXT("DropOpacity"), Opacity);
        }
    }
    Drop.Icon->SetVisibility(bVisible && Presentation.IconMaterial);
    Drop.Beam->SetVisibility(bVisible && Presentation.BeamMaterial);
    Drop.GroundGlow->SetVisibility(bVisible && Presentation.GroundGlowMaterial);
    const FVector Location = Drop.GetActorLocation() - FVector(0, 0, Drop.Body->GetScaledSphereRadius() - 0.5f);
    Drop.Icon->SetWorldLocation(Location + FVector(0, 0, Config.IconLift));
    FVector ToCamera = FVector::ForwardVector;
    const auto &Views = Drop.GetWorld()->ViewLocationsRenderedLastFrame;
    if (!Views.IsEmpty())
    {
        ToCamera = (Views[0] - Location).GetSafeNormal2D(UE_SMALL_NUMBER, FVector::ForwardVector);
    }
    const FRotator BeamRotation = FRotationMatrix::MakeFromXZ(FVector::CrossProduct(FVector::UpVector, ToCamera), ToCamera).Rotator();
    Drop.Beam->SetWorldLocationAndRotation(Location + FVector(0, 0, Config.BeamHeight * 0.5f), BeamRotation);
    Drop.Beam->SetWorldScale3D(FVector(Config.BeamWidth / 100.0f, Config.BeamHeight / 100.0f, 1.0f));
    Drop.GroundGlow->SetWorldLocationAndRotation(Location, FRotator::ZeroRotator);
    Drop.GroundGlow->SetWorldScale3D(FVector(Config.GroundGlowDiameter / 100.0f, Config.GroundGlowDiameter / 100.0f, 1.0f));
}
