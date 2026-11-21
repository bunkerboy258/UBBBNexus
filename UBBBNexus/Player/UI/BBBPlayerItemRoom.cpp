#include "BBBWork/UBBBNexus/Player/UI/BBBPlayerItemPortrait.h"
#include "Components/ExponentialHeightFogComponent.h"
#include "Components/RectLightComponent.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Components/SpotLightComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/WidgetComponent.h"
#include "Engine/SceneCapture2D.h"
#include "Engine/StaticMesh.h"
#include "Materials/MaterialInstanceDynamic.h"

namespace
{
UWidgetComponent* CreateRoomWidget(AActor& Owner, const TSharedPtr<SWidget>& Widget, FVector2D Size,
    FVector Location, FRotator Rotation, float UnitsPerPixel)
{
    auto* Panel = NewObject<UWidgetComponent>(&Owner);
    Owner.AddInstanceComponent(Panel);
    Panel->SetupAttachment(Owner.GetRootComponent());
    Panel->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    Panel->SetCastShadow(false);
    Panel->SetVisibleInSceneCaptureOnly(true);
    Panel->SetTickWhenOffscreen(true);
    Panel->SetBlendMode(EWidgetBlendMode::Transparent);
    Panel->SetBackgroundColor(FLinearColor::Transparent);
    Panel->SetDrawSize(Size);
    Panel->SetRelativeTransform(FTransform(Rotation, Location, FVector(UnitsPerPixel)));
    Panel->SetSlateWidget(Widget);
    Panel->RegisterComponent();
    return Panel;
}
}

void UBBBPlayerItemPortrait::BuildRoom(USceneComponent& Root)
{
    auto* Block = LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Cube.Cube"));
    auto* Matte = LoadObject<UMaterialInterface>(nullptr, TEXT("/Engine/BasicShapes/BasicShapeMaterial.BasicShapeMaterial"));
    auto* Concrete = LoadObject<UMaterialInterface>(nullptr,
        TEXT("/Game/_Project/UI/Bag/Materials/M_InventorySurveyFloor.M_InventorySurveyFloor"));
    auto* Plaster = UMaterialInstanceDynamic::Create(Matte, Display);
    Plaster->SetVectorParameterValue(TEXT("Color"), FLinearColor(0.115f, 0.107f, 0.098f));
    const auto AddSurface = [&](const FVector Location, const FVector Scale, UMaterialInterface* Finish)
    {
        auto* Surface = NewObject<UStaticMeshComponent>(Display);
        Display->AddInstanceComponent(Surface);
        Surface->SetupAttachment(&Root);
        Surface->SetStaticMesh(Block);
        Surface->SetMaterial(0, Finish);
        Surface->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        Surface->SetRelativeLocation(Location);
        Surface->SetRelativeScale3D(Scale);
        Surface->RegisterComponent();
    };
    AddSurface(FVector(-1110, 0, -15), FVector(35, 12, 0.3), Concrete);
    AddSurface(FVector(-2800, 0, 190), FVector(0.45, 12, 3.8), Plaster);
    AddSurface(FVector(600, 0, 190), FVector(0.4, 12, 3.8), Plaster);
    AddSurface(FVector(-1110, -570, 190), FVector(35, 0.4, 3.8), Plaster);
    AddSurface(FVector(-1110, 570, 190), FVector(35, 0.4, 3.8), Plaster);
    AddSurface(FVector(-1110, 0, 380), FVector(35, 12, 0.4), Plaster);

    auto* Key = NewObject<USpotLightComponent>(Display);
    Display->AddInstanceComponent(Key);
    Key->SetupAttachment(&Root);
    const FVector FloorCentre = FVector::ZeroVector;
    Key->SetRelativeLocation(FVector(65, 25, 344));
    Key->SetRelativeRotation((FloorCentre - Key->GetRelativeLocation()).Rotation());
    Key->SetIntensityUnits(ELightUnits::Candelas);
    Key->SetIntensity(51.0f);
    Key->SetLightColor(FLinearColor(1.0f, 0.85f, 0.70f));
    Key->SetInnerConeAngle(12.0f);
    Key->SetOuterConeAngle(34.0f);
    Key->SetSourceRadius(24);
    Key->SetSoftSourceRadius(12);
    Key->SetAttenuationRadius(625);
    Key->SetCastShadows(true);
    Key->ContactShadowLength = 0.15f;
    Key->ShadowResolutionScale = 2.0f;
    Key->SetVolumetricScatteringIntensity(4.0f);
    Key->RegisterComponent();

    const auto AddFill = [&](FVector Location, FVector Target, FLinearColor Color, float Intensity, float Cone)
    {
        auto* Light = NewObject<USpotLightComponent>(Display);
        Display->AddInstanceComponent(Light);
        Light->SetupAttachment(&Root);
        Light->SetRelativeLocation(Location);
        Light->SetRelativeRotation((Target - Location).Rotation());
        Light->SetInnerConeAngle(Cone * 0.58f);
        Light->SetOuterConeAngle(Cone);
        Light->SetIntensityUnits(ELightUnits::Candelas);
        Light->SetIntensity(Intensity);
        Light->SetLightColor(Color);
        Light->SetAttenuationRadius(425.0f);
        Light->SetSourceRadius(20.0f);
        Light->SetCastShadows(false);
        Light->SetVolumetricScatteringIntensity(0.0f);
        Light->RegisterComponent();
    };
    AddFill(FVector(108, -144, 157), FVector(0, 0, 123), FLinearColor(0.72f, 0.79f, 1.0f), 3.8f, 28.0f);
    AddFill(FVector(-95, -90, 202), FVector(0, 0, 124), FLinearColor(0.65f, 0.77f, 1.0f), 24.0f, 36.0f);
    AddFill(FVector(-55, 137, 230), FVector(0, 0, 149), FLinearColor(1.0f, 0.85f, 0.67f), 34.0f, 30.0f);

    auto* Bounce = NewObject<URectLightComponent>(Display);
    Display->AddInstanceComponent(Bounce);
    Bounce->SetupAttachment(&Root);
    Bounce->SetRelativeLocation(FVector(135, 78, 145));
    Bounce->SetRelativeRotation((FVector(12, 0, 109) - Bounce->GetRelativeLocation()).Rotation());
    Bounce->SetIntensityUnits(ELightUnits::Candelas);
    Bounce->SetIntensity(6.0f);
    Bounce->SetSourceWidth(105.0f);
    Bounce->SetSourceHeight(50.0f);
    Bounce->SetAttenuationRadius(275.0f);
    Bounce->SetLightColor(FLinearColor(1.0f, 0.89f, 0.76f));
    Bounce->SetCastShadows(false);
    Bounce->SetVolumetricScatteringIntensity(0.0f);
    Bounce->RegisterComponent();

    auto* Haze = NewObject<UExponentialHeightFogComponent>(Display);
    Display->AddInstanceComponent(Haze);
    Haze->SetupAttachment(&Root);
    Haze->SetFogDensity(0.027f);
    Haze->SetFogHeightFalloff(0.1f);
    Haze->SetFogMaxOpacity(0.18f);
    Haze->SetFogInscatteringColor(FLinearColor(0.025f, 0.019f, 0.014f));
    Haze->SetVolumetricFog(true);
    Haze->SetVolumetricFogDistance(2200.0f);
    Haze->SetVolumetricFogAlbedo(FColor(204, 174, 145));
    Haze->SetVolumetricFogExtinctionScale(0.8f);
    Haze->SetVolumetricFogScatteringDistribution(0.13f);
    Haze->RegisterComponent();

    auto* WarmMist = NewObject<UStaticMeshComponent>(Display);
    Display->AddInstanceComponent(WarmMist);
    WarmMist->SetupAttachment(&Root);
    WarmMist->SetStaticMesh(LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Sphere.Sphere")));
    WarmMist->SetMaterial(0, LoadObject<UMaterialInterface>(nullptr,
        TEXT("/Game/_Project/UI/Bag/Materials/M_InventoryWarmMist.M_InventoryWarmMist")));
    WarmMist->SetRelativeLocation(FVector(-340, 108, 272));
    WarmMist->SetRelativeScale3D(FVector(4.4, 4.0, 6.1));
    WarmMist->SetCastShadow(false);
    WarmMist->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    WarmMist->SetTranslucentSortPriority(-1);
    WarmMist->RegisterComponent();
}

void UBBBPlayerItemPortrait::SetPanels(const TArray<TSharedPtr<SWidget>>& Content)
{
    if (!Capture || !Display || !Panels.IsEmpty() || Content.Num() != 3)
    {
        return;
    }

    const auto* Camera = Capture->GetCaptureComponent2D();
    constexpr float Depth = 240.0f;
    const float Units = 2.0f * Depth * FMath::Tan(FMath::DegreesToRadians(Camera->FOVAngle * 0.5f)) / 1920.0f;
    const FVector Forward = Camera->GetForwardVector();
    const FVector Right = Camera->GetRightVector();
    const FVector Up = Camera->GetUpVector();
    const FVector Centre = Camera->GetComponentLocation() + Forward * Depth - Origin;
    const FVector2D Centres[] = {FVector2D(960, 540), FVector2D(350, 574), FVector2D(1566, 574)};
    const FVector2D Sizes[] = {FVector2D(1920, 1080), FVector2D(520, 800), FVector2D(520, 800)};
    const float Lean[] = {0.0f, -24.0f, 24.0f};

    for (int32 Index = 1; Index < Content.Num(); ++Index)
    {
        const FVector2D ScreenOffset = Centres[Index] - FVector2D(960, 540);
        const FVector Location = Centre + Units * (Right * ScreenOffset.X - Up * ScreenOffset.Y);
        const FVector Normal = (-Forward).RotateAngleAxis(Lean[Index], Up);
        const FRotator Rotation = FRotationMatrix::MakeFromXZ(Normal, Up).Rotator();
        Panels.Add(CreateRoomWidget(*Display, Content[Index], Sizes[Index], Location, Rotation, Units));
        Panels.Last()->SetTintColorAndOpacity(FLinearColor(2.1f, 2.1f, 2.1f, 1.0f));
    }
}
