#include "BBBWork/UBBBNexus/Debug/Mass/MaodieTest/Logic/System/AppearanceSystem/Processors/BBBMaodieTestAppearanceProcessor.h"
#include "BBBWork/UBBBNexus/Debug/Mass/MaodieTest/BBBMaodieTest.h"
#include "Camera/PlayerCameraManager.h"
#include "Components/PointLightComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Kismet/GameplayStatics.h"

namespace
{
    constexpr int32 OrangeStencil = 250;

    /** @param Actor 所属耄耋 @return 不参与环境间接光和阴影的本机专用灯 */
    UPointLightComponent *CreateLight(ABBBMaodieTest &Actor)
    {
        auto *Light = NewObject<UPointLightComponent>(&Actor, NAME_None, RF_Transient);
        Actor.AddInstanceComponent(Light);
        Light->SetupAttachment(Actor.GetRootComponent());
        Light->SetMobility(EComponentMobility::Movable);
        Light->SetLightingChannels(false, false, true);
        Light->SetCastShadows(false);
        Light->SetAffectGlobalIllumination(false);
        Light->SetAffectReflection(false);
        Light->SetIndirectLightingIntensity(0.0f);
        Light->SetVolumetricScatteringIntensity(0.0f);
        Light->SetIntensityUnits(ELightUnits::Candelas);
        Light->SetAttenuationRadius(170.0f);
        Light->SetSourceRadius(25.0f);
        Light->SetSoftSourceRadius(35.0f);
        Light->SetComponentTickEnabled(false);
        Light->RegisterComponent();
        return Light;
    }
}

void FBBBMaodieTestAppearanceProcessor::Update(ABBBMaodieTest &Actor, float DeltaSeconds)
{
    if (Actor.GetNetMode() == NM_DedicatedServer)
    {
        return;
    }

    auto &State = Actor.RuntimeData.Appearance.State;
    const auto &Config = Actor.GetTestConfig().Appearance;
    auto *Mesh = Actor.GetMesh();
    if (State.Mesh.Get() != Mesh)
    {
        Shutdown(Actor);
    }
    if (!IsValid(Mesh))
    {
        return;
    }
    if (!State.Mesh.IsValid())
    {
        State.Mesh = Mesh;
        State.OriginalChannels = Mesh->LightingChannels;
        State.bOriginalCustomDepth = Mesh->bRenderCustomDepth;
        State.OriginalStencil = Mesh->CustomDepthStencilValue;
        State.OriginalWriteMask = Mesh->CustomDepthStencilWriteMask;
        State.ViewRotation = Actor.GetActorRotation();
    }

    const bool bVisible = Mesh->IsVisible() && !Mesh->bHiddenInGame && !Actor.IsHidden();
    const bool bLighting = Config.bLighting && bVisible;
    if (bLighting && !IsValid(State.Fill))
    {
        State.Fill = CreateLight(Actor);
    }
    if (bLighting && !IsValid(State.Rim))
    {
        State.Rim = CreateLight(Actor);
    }
    if (IsValid(State.Fill) && IsValid(State.Rim))
    {
        const auto *Camera = UGameplayStatics::GetPlayerCameraManager(&Actor, 0);
        const FRotator Target = Camera ? Camera->GetCameraRotation() : Actor.GetActorRotation();
        State.ViewRotation = FMath::RInterpTo(State.ViewRotation, Target, DeltaSeconds, 5.0f);
        const FRotator Yaw(0.0f, State.ViewRotation.Yaw, 0.0f);
        const FVector Origin = Actor.GetActorLocation();
        State.Fill->SetWorldLocation(Origin + Yaw.RotateVector(FVector(-90.0f, 45.0f, 18.0f)));
        State.Rim->SetWorldLocation(Origin + Yaw.RotateVector(FVector(75.0f, -65.0f, 45.0f)));
        State.Fill->SetLightColor(Config.FillColor);
        State.Rim->SetLightColor(Config.RimColor);
        State.Fill->SetIntensity(FMath::Max(0.0f, Config.FillIntensity));
        State.Rim->SetIntensity(FMath::Max(0.0f, Config.RimIntensity));
        State.Fill->SetVisibility(bLighting);
        State.Rim->SetVisibility(bLighting);
    }
    const bool bChannel2 = Config.bLighting || State.OriginalChannels.bChannel2;
    if (Mesh->LightingChannels.bChannel2 != bChannel2)
    {
        Mesh->SetLightingChannels(State.OriginalChannels.bChannel0, State.OriginalChannels.bChannel1, bChannel2);
    }

    const bool bStencilAvailable = !State.bOriginalCustomDepth || State.OriginalStencil == 0
        || State.OriginalStencil == OrangeStencil;
    const bool bOutline = Config.bInnerOutline && bVisible && Mesh->bRenderInMainPass
        && Mesh->GetSkeletalMeshAsset() && bStencilAvailable;
    const bool bDepth = bOutline || State.bOriginalCustomDepth;
    const int32 Stencil = bOutline ? OrangeStencil : State.OriginalStencil;
    const auto WriteMask = bOutline ? ERendererStencilMask::ERSM_Default : State.OriginalWriteMask;
    if (Mesh->bRenderCustomDepth != bDepth)
    {
        Mesh->SetRenderCustomDepth(bDepth);
    }
    if (Mesh->CustomDepthStencilValue != Stencil)
    {
        Mesh->SetCustomDepthStencilValue(Stencil);
    }
    if (Mesh->CustomDepthStencilWriteMask != WriteMask)
    {
        Mesh->SetCustomDepthStencilWriteMask(WriteMask);
    }
}

void FBBBMaodieTestAppearanceProcessor::Shutdown(ABBBMaodieTest &Actor)
{
    auto &State = Actor.RuntimeData.Appearance.State;
    if (auto *Mesh = State.Mesh.Get())
    {
        Mesh->SetLightingChannels(State.OriginalChannels.bChannel0, State.OriginalChannels.bChannel1, State.OriginalChannels.bChannel2);
        Mesh->SetRenderCustomDepth(State.bOriginalCustomDepth);
        Mesh->SetCustomDepthStencilValue(State.OriginalStencil);
        Mesh->SetCustomDepthStencilWriteMask(State.OriginalWriteMask);
    }
    if (IsValid(State.Fill))
    {
        State.Fill->DestroyComponent();
    }
    if (IsValid(State.Rim))
    {
        State.Rim->DestroyComponent();
    }
    State.Fill = nullptr;
    State.Rim = nullptr;
    State.Mesh.Reset();
}
