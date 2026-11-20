#include "BBBWork/UBBBNexus/Player/UI/BBBPlayerItemPortrait.h"
#include "BBBWork/UBBBNexus/Player/UI/BBBPlayerItemStyle.h"
#include "Components/ExponentialHeightFogComponent.h"
#include "Components/PointLightComponent.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Components/SpotLightComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/WidgetComponent.h"
#include "Engine/SceneCapture2D.h"
#include "Engine/StaticMesh.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Components/RectLightComponent.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Text/STextBlock.h"

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
    auto* Concrete = UMaterialInstanceDynamic::Create(Matte, Display);
    Concrete->SetVectorParameterValue(TEXT("Color"), FLinearColor(0.115f, 0.108f, 0.1f));
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
    AddSurface(FVector(-60, 0, -15), FVector(14, 12, 0.3), Concrete);
    AddSurface(FVector(-420, 0, 190), FVector(0.45, 12, 3.8), Plaster);
    AddSurface(FVector(600, 0, 190), FVector(0.4, 12, 3.8), Plaster);
    AddSurface(FVector(-50, -570, 190), FVector(13, 0.4, 3.8), Plaster);
    AddSurface(FVector(-50, 570, 190), FVector(13, 0.4, 3.8), Plaster);
    AddSurface(FVector(-50, 0, 380), FVector(13, 12, 0.4), Plaster);


    auto* Key = NewObject<USpotLightComponent>(Display);
    Display->AddInstanceComponent(Key);
    Key->SetupAttachment(&Root);
    Key->SetRelativeLocation(FVector(166, 124, 294));
    Key->SetRelativeRotation((FVector(-25, -8, 58) - Key->GetRelativeLocation()).Rotation());
    Key->SetIntensityUnits(ELightUnits::Candelas);
    Key->SetIntensity(62.0f);
    Key->SetLightColor(FLinearColor(1.0f, 0.86f, 0.69f));
    Key->SetInnerConeAngle(19.0f);
    Key->SetOuterConeAngle(34.0f);
    Key->SetSourceRadius(24);
    Key->SetSoftSourceRadius(12);
    Key->SetAttenuationRadius(625);
    Key->SetCastShadows(true);
    Key->ContactShadowLength = 0.15f;
    Key->ShadowResolutionScale = 2.0f;
    Key->SetVolumetricScatteringIntensity(2.6f);
    Key->RegisterComponent();

    const auto AddFill = [&](FVector Location, FVector Target, FLinearColor Color, float Intensity, float Cone, float Scatter)
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
        Light->SetVolumetricScatteringIntensity(Scatter);
        Light->RegisterComponent();
    };
    AddFill(FVector(108, -144, 157), FVector(0, 0, 123), FLinearColor(0.72f, 0.79f, 1.0f), 6.0f, 28.0f, 0.0f);
    AddFill(FVector(-89, -70, 188), FVector(0, 0, 140), FLinearColor(0.64f, 0.76f, 1.0f), 14.0f, 25.0f, 0.0f);
    AddFill(FVector(165, -158, 152), FVector(-35, 0, 8), FLinearColor(0.97f, 0.93f, 0.87f), 11.0f, 36.0f, 0.0f);
    AddFill(FVector(-90, 129, 231), FVector(-394, 54, 228), FLinearColor(1.0f, 0.83f, 0.63f), 48.0f, 50.0f, 5.0f);

    auto* Bounce = NewObject<URectLightComponent>(Display);
    Display->AddInstanceComponent(Bounce);
    Bounce->SetupAttachment(&Root);
    Bounce->SetRelativeLocation(FVector(135, 78, 145));
    Bounce->SetRelativeRotation((FVector(12, 0, 109) - Bounce->GetRelativeLocation()).Rotation());
    Bounce->SetIntensityUnits(ELightUnits::Candelas);
    Bounce->SetIntensity(8.0f);
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
    Haze->SetVolumetricFogDistance(1050.0f);
    Haze->SetVolumetricFogAlbedo(FColor(204, 174, 145));
    Haze->SetVolumetricFogExtinctionScale(0.8f);
    Haze->SetVolumetricFogScatteringDistribution(0.13f);
    Haze->RegisterComponent();

    const FLinearColor Amber(1.0f, 0.38f, 0.028f);
    const FRotator FloorFacing = FRotationMatrix::MakeFromXZ(FVector::UpVector, -FVector::ForwardVector).Rotator();
    RoomSigns.Add(CreateRoomWidget(*Display,
        SNew(SBox).HAlign(HAlign_Center).VAlign(VAlign_Center)
        [SNew(STextBlock).Text(INVTEXT("B")).Font(BBBItemUI::ArtFont(83)).ColorAndOpacity(Amber)],
        FVector2D(115, 126), FVector(8, 76, 1.1), FloorFacing, 0.11f));
    RoomSigns.Last()->SetTintColorAndOpacity(FLinearColor(1.65f, 1.45f, 1.2f, 1.0f));

    const FSlateBrush* Mark = BBBItemUI::Brush(
        TEXT("/Game/_ThirdParty/UI/EditableSurvivalHorrorUI/Weapon_Customization/T_Scrollbar_Handler.T_Scrollbar_Handler"));
    const FVector Positions[] = {FVector(69, -121, 1.3), FVector(-21, 147, 1.3), FVector(-146, -114, 1.3)};
    for (const FVector& Location : Positions)
    {
        RoomSigns.Add(CreateRoomWidget(*Display, SNew(SImage).Image(Mark).ColorAndOpacity(Amber),
            FVector2D(30, 5), Location, FloorFacing, 0.17f));
        RoomSigns.Last()->SetTintColorAndOpacity(FLinearColor(1.35f, 1.22f, 1.1f, 1.0f));
    }
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
