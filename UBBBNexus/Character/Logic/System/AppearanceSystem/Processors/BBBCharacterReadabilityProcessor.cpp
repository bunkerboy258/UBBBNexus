#include "BBBWork/UBBBNexus/Character/Logic/System/AppearanceSystem/Processors/BBBCharacterReadabilityProcessor.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/AppearanceSystem/DomainData/Context/BBBCharacterAppearanceUpdateContext.h"
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "Components/SkeletalMeshComponent.h"

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
            Entry.bCustomDepth = Component->bRenderCustomDepth;
            Entry.Stencil = Component->CustomDepthStencilValue;
            Entry.WriteMask = Component->CustomDepthStencilWriteMask;
            Stored = &Entry;
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
            Component->SetRenderCustomDepth(Entry.bCustomDepth);
            Component->SetCustomDepthStencilValue(Entry.Stencil);
            Component->SetCustomDepthStencilWriteMask(Entry.WriteMask);
        }
    }

    State.Primitives.Reset();
}
