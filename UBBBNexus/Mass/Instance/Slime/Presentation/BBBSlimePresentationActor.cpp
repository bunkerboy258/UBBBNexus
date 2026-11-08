#include "BBBSlimePresentationActor.h"

#include "Components/StaticMeshComponent.h"
#include "Components/SceneComponent.h"
#include "Components/DecalComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Engine/StaticMesh.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "UObject/ConstructorHelpers.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Slime/Fragments/BBBSlimeFact.h"

namespace
{
    /** @param Component 当前粒子载体 @param Settings 共享表现配置 @return 写入统一颜色与预算 */
    void SlimeParameters(UNiagaraComponent& Component, const FBBBSlimePresentation& Settings)
    {
        Component.SetVariableLinearColor(TEXT("User.Color"), Settings.MainColor);
        Component.SetVariableLinearColor(TEXT("User.SecondaryColor"), Settings.SecondaryColor);
        Component.SetVariableFloat(TEXT("User.SpawnRate"), Settings.ParticleScale);
    }
}

ABBBSlimePresentationActor::ABBBSlimePresentationActor()
{
    PrimaryActorTick.bCanEverTick = false;
    bReplicates = false;
    SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SlimeRoot"));
    SetRootComponent(SceneRoot);
    Body = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SlimeBody"));
    Body->SetupAttachment(SceneRoot);
    Body->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    Body->SetGenerateOverlapEvents(false);
    Body->SetCanEverAffectNavigation(false);
    Body->SetCastShadow(false);
    static ConstructorHelpers::FObjectFinder<UStaticMesh> Sphere(TEXT("/Engine/BasicShapes/Sphere.Sphere"));
    Body->SetStaticMesh(Sphere.Object);
    Trail = CreateDefaultSubobject<UNiagaraComponent>(TEXT("LiquidTrail"));
    Trail->SetupAttachment(SceneRoot);
    Trail->SetAutoActivate(false);
    Bubbles = CreateDefaultSubobject<UNiagaraComponent>(TEXT("GroundBubbles"));
    Bubbles->SetupAttachment(SceneRoot);
    Bubbles->SetAutoActivate(false);
    Puddle = CreateDefaultSubobject<UDecalComponent>(TEXT("SlimePuddle"));
    Puddle->SetupAttachment(SceneRoot);
    Puddle->SetVisibility(false);
}

bool ABBBSlimePresentationActor::Configure(const FBBBSlimeFact& Fact)
{
    if (Definition == Fact.Definition)
    {
        return Definition && Definition->IsPresentationReady();
    }
    Definition = Fact.Definition;
    if (!Definition || !ensureMsgf(Definition->IsPresentationReady(), TEXT("[BBBSlime] 黏液表现配置不完整")))
    {
        SetActorHiddenInGame(true);
        return false;
    }
    const auto& Settings = Definition->Presentation;
    Body->SetMaterial(0, Settings.FlightMaterial);
    Body->SetCustomPrimitiveDataVector3(0, FVector(Settings.MainColor.R, Settings.MainColor.G, Settings.MainColor.B));
    Body->SetCustomPrimitiveDataVector3(3, FVector(Settings.SecondaryColor.R, Settings.SecondaryColor.G, Settings.SecondaryColor.B));
    Body->SetCullDistance(Settings.MaximumDistance);
    Puddle->SetDecalMaterial(Settings.GroundMaterial);
    PuddleMaterial = Puddle->CreateDynamicMaterialInstance();
    PuddleMaterial->SetVectorParameterValue(TEXT("MainColor"), Settings.MainColor);
    PuddleMaterial->SetVectorParameterValue(TEXT("SecondaryColor"), Settings.SecondaryColor);
    Trail->SetAsset(Settings.FlightSystem);
    Bubbles->SetAsset(Settings.GroundSystem);
    SlimeParameters(*Trail, Settings);
    SlimeParameters(*Bubbles, Settings);
    Bubbles->SetVariableFloat(TEXT("User.Seed"), GetTypeHash(Fact.InstanceId) % 65536);
    Trail->SetVariableFloat(TEXT("User.TrailLifetime"), Settings.TrailLifetime);
    Trail->SetVariableFloat(TEXT("User.Size"), Settings.TrailWidth);
    AppliedPhase = 255;
    return true;
}

void ABBBSlimePresentationActor::ApplyFact(const FBBBSlimeFact& Fact)
{
    if (!Configure(Fact))
    {
        return;
    }
    const auto& Settings = Definition->Presentation;
    const float Now = GetWorld()->GetTimeSeconds();
    const float Remaining = FMath::Max(0.0f, Fact.EndsAt - Now);
    bool bNear = true;
    if (const auto* Controller = GetWorld()->GetFirstPlayerController())
    {
        FVector Eye;
        FRotator View;
        Controller->GetPlayerViewPoint(Eye, View);
        bNear = FVector::DistSquared(Eye, Fact.Location) <= FMath::Square(Settings.MaximumDistance);
    }
    SetActorHiddenInGame(!bNear);
    const bool bGround = Fact.Phase == 1;
    SetActorLocation(Fact.Location + (bGround ? Fact.Normal * 2 : FVector::ZeroVector));
    Body->SetVisibility(!bGround);
    Puddle->SetVisibility(bGround && bNear);
    if (!bGround)
    {
        SetActorRotation(Fact.Velocity.ToOrientationQuat());
        const float Stretch = FMath::Clamp(Fact.Velocity.Size() / FMath::Max(Definition->Speed, 1.0f), .75f, 1.5f);
        Body->SetRelativeScale3D(Settings.FlightShape * FVector(Stretch, 1, 1) * (Definition->FlightRadius / 50.0f));
        if (AppliedPhase == 255 && bNear && Definition->FlightLifetime - Remaining <= .2f)
        {
            auto* Mouth = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), Settings.MouthSystem,
                Fact.Location, Fact.Velocity.Rotation(), FVector::OneVector, true, false, ENCPoolMethod::AutoRelease);
            if (Mouth)
            {
                SlimeParameters(*Mouth, Settings);
                Mouth->SetVariableVec3(TEXT("User.Direction"), Fact.Velocity.GetSafeNormal());
                Mouth->SetVariableFloat(TEXT("User.Seed"), GetTypeHash(Fact.InstanceId) % 65536);
                Mouth->Activate(true);
            }
        }
        if (bNear && !Trail->IsActive())
        {
            Trail->Activate(true);
        }
        if (!bNear && Trail->IsActive())
        {
            Trail->DeactivateImmediate();
        }
    }
    if (bGround)
    {
        const float Age = FMath::Max(0.0f, Definition->Lifetime - Remaining);
        SetActorRotation(FRotationMatrix::MakeFromZ(Fact.Normal).ToQuat());
        FRotator PuddleRotation = FRotationMatrix::MakeFromX(-Fact.Normal).Rotator();
        PuddleRotation.Roll += GetTypeHash(Fact.InstanceId) % 360;
        Puddle->SetWorldRotation(PuddleRotation);
        const float Spread = FMath::SmoothStep(0.0f, Settings.SpreadSeconds, Age);
        const float Radius = Definition->Radius * FMath::Lerp(.45f, 1.0f, Spread);
        const FVector Size(24, Radius, Radius);
        if (!Puddle->DecalSize.Equals(Size, .05f))
        {
            Puddle->DecalSize = Size;
            Puddle->MarkRenderStateDirty();
        }
        if (AppliedPhase != 1)
        {
            Trail->DeactivateImmediate();
            Puddle->SetFadeOut(FMath::Max(0.0f, Remaining - Settings.FadeSeconds), FMath::Min(Remaining, Settings.FadeSeconds), false);
            if (bNear && (AppliedPhase == 0 || Age <= .15f))
            {
                auto* Impact = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), Settings.ImpactSystem,
                    Fact.Location + Fact.Normal * 3, FRotationMatrix::MakeFromX(Fact.Normal).Rotator(),
                    FVector::OneVector, true, false, ENCPoolMethod::AutoRelease);
                if (Impact)
                {
                    SlimeParameters(*Impact, Settings);
                    Impact->SetVariableVec3(TEXT("User.Direction"), Fact.Normal);
                    Impact->SetVariableFloat(TEXT("User.Seed"), GetTypeHash(Fact.InstanceId) % 65536);
                    Impact->Activate(true);
                }
            }
            Bubbles->SetVariableFloat(TEXT("User.Size"), Definition->Radius / 250.0f);
            Bubbles->SetVariableVec3(TEXT("User.Direction"), Fact.Normal);
        }
        if (bNear && Remaining > Settings.FadeSeconds && !Bubbles->IsActive())
        {
            Bubbles->Activate(true);
        }
        if ((!bNear || Remaining <= Settings.FadeSeconds) && Bubbles->IsActive())
        {
            Bubbles->Deactivate();
        }
    }
    AppliedPhase = Fact.Phase;
    if (FMath::Abs(GetLifeSpan() - (Remaining + 1)) > .05f)
    {
        SetLifeSpan(Remaining + 1);
    }
}
