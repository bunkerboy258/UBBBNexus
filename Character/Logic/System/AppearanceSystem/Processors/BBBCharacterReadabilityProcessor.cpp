#include "BBBWork/UBBBNexus/Character/Logic/System/AppearanceSystem/Processors/BBBCharacterReadabilityProcessor.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/AppearanceSystem/DomainData/Context/BBBCharacterAppearanceUpdateContext.h"
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "Components/PointLightComponent.h"
#include "Components/SkeletalMeshComponent.h"

namespace
{
    /** @param Character 所属角色 @return 不参与间接光与阴影的本地专用灯 */
    UPointLightComponent *CreateLight(ABBBCharacter &Character)
    {
        auto *Light = NewObject<UPointLightComponent>(&Character, NAME_None, RF_Transient);
        Character.AddInstanceComponent(Light);
        Light->SetupAttachment(Character.GetRootComponent());
        Light->SetMobility(EComponentMobility::Movable);
        Light->SetLightingChannels(false, false, true);
        Light->SetCastShadows(false);
        Light->SetAffectGlobalIllumination(false);
        Light->SetAffectReflection(false);
        Light->SetIndirectLightingIntensity(0.0f);
        Light->SetVolumetricScatteringIntensity(0.0f);
        Light->SetIntensityUnits(ELightUnits::Candelas);
        Light->SetAttenuationRadius(260.0f);
        Light->SetSourceRadius(35.0f);
        Light->SetSoftSourceRadius(50.0f);
        Light->SetComponentTickEnabled(false);
        Light->RegisterComponent();
        return Light;
    }
}

void FBBBCharacterReadabilityProcessor::Update(FBBBCharacterAppearanceUpdateContext &Context) const
{
    auto &Character = Context.Character;
    auto &State = Context.Data.Appearance.ReadabilityState;
    const auto &Config = Context.Config;
    const bool bLocalPlayer = Character.IsPlayerControlled() && Character.IsLocallyControlled()
        && Character.GetNetMode() != NM_DedicatedServer && Character.GetLifePhase() != EBBBCharacterLifePhase::Dead;
    if (!bLocalPlayer)
    {
        Shutdown(Context);
        return;
    }
    if (Config.bReadabilityLighting && (!IsValid(State.Fill) || !IsValid(State.Rim)))
    {
        if (!IsValid(State.Fill))
        {
            State.Fill = CreateLight(Character);
        }
        if (!IsValid(State.Rim))
        {
            State.Rim = CreateLight(Character);
        }
        State.ViewRotation = Character.GetControlRotation();
    }
    if (IsValid(State.Fill) && IsValid(State.Rim))
    {
        const float Delta = Context.Data.External.ReadWorldState().FrameDeltaSeconds;
        State.ViewRotation = FMath::RInterpTo(State.ViewRotation, Character.GetControlRotation(), Delta, 5.0f);
        const FRotator Yaw(0.0f, State.ViewRotation.Yaw, 0.0f);
        const FVector Origin = Character.GetActorLocation();
        const float Top = Character.GetSimpleCollisionHalfHeight() * 0.65f;
        State.Fill->SetWorldLocation(Origin + Yaw.RotateVector(FVector(-120.0f, 65.0f, Top)));
        State.Rim->SetWorldLocation(Origin + Yaw.RotateVector(FVector(100.0f, -90.0f, Top + 35.0f)));
        State.Fill->SetLightColor(Config.ReadabilityLightColor);
        State.Rim->SetLightColor(Config.ReadabilityLightColor);
        State.Fill->SetIntensity(FMath::Max(0.0f, Config.FillIntensity));
        State.Rim->SetIntensity(FMath::Max(0.0f, Config.RimIntensity));
        State.Fill->SetVisibility(Config.bReadabilityLighting);
        State.Rim->SetVisibility(Config.bReadabilityLighting);
    }
    TArray<USkeletalMeshComponent *> Components;
    Character.GetComponents(Components);
    for (auto *Component : Components)
    {
        auto *Stored = State.Primitives.FindByPredicate([Component](const FBBBCharacterReadabilityPrimitive &Value)
        {
            return Value.Component.Get() == Component;
        });
        if (!Stored)
        {
            auto &Entry = State.Primitives.AddDefaulted_GetRef();
            Entry.Component = Component;
            Entry.Channels = Component->LightingChannels;
            Entry.bCustomDepth = Component->bRenderCustomDepth;
            Entry.Stencil = Component->CustomDepthStencilValue;
            Entry.WriteMask = Component->CustomDepthStencilWriteMask;
            Stored = &Entry;
        }
        const bool bChannel2 = Config.bReadabilityLighting || Stored->Channels.bChannel2;
        if (Component->LightingChannels.bChannel2 != bChannel2)
        {
            Component->SetLightingChannels(Stored->Channels.bChannel0, Stored->Channels.bChannel1, bChannel2);
        }
        const bool bOutline = Config.InnerOutlineOpacity > 0.0f && Config.InnerOutlineWidth > 0.0f
            && Component->bRenderInMainPass && Component->IsVisible() && !Component->bHiddenInGame
            && Component->GetSkeletalMeshAsset();
        if (bOutline && Stored->bCustomDepth && Stored->Stencil != 0
            && Stored->Stencil != Config.ReadabilityStencil)
        {
            ensureMsgf(false, TEXT("角色部件模板值已被其它表现占用 跳过轮廓标记 %s"), *Component->GetName());
            continue;
        }
        const int32 Stencil = bOutline ? FMath::Clamp(Config.ReadabilityStencil, 1, 255) : Stored->Stencil;
        const bool bDepth = bOutline || Stored->bCustomDepth;
        if (Component->bRenderCustomDepth != bDepth)
        {
            Component->SetRenderCustomDepth(bDepth);
        }
        if (Component->CustomDepthStencilValue != Stencil)
        {
            Component->SetCustomDepthStencilValue(Stencil);
        }
        const auto Mask = bOutline ? ERendererStencilMask::ERSM_Default : Stored->WriteMask;
        if (Component->CustomDepthStencilWriteMask != Mask)
        {
            Component->SetCustomDepthStencilWriteMask(Mask);
        }
    }
}

void FBBBCharacterReadabilityProcessor::Shutdown(FBBBCharacterAppearanceUpdateContext &Context) const
{
    auto &State = Context.Data.Appearance.ReadabilityState;
    for (const auto &Entry : State.Primitives)
    {
        if (auto *Component = Entry.Component.Get())
        {
            Component->SetLightingChannels(Entry.Channels.bChannel0, Entry.Channels.bChannel1, Entry.Channels.bChannel2);
            Component->SetRenderCustomDepth(Entry.bCustomDepth);
            Component->SetCustomDepthStencilValue(Entry.Stencil);
            Component->SetCustomDepthStencilWriteMask(Entry.WriteMask);
        }
    }
    State.Primitives.Reset();
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
}
