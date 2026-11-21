#include "BBBWork/UBBBNexus/Player/UI/BBBPlayerItemPortrait.h"
#include "Components/RectLightComponent.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Components/SpotLightComponent.h"
#include "Components/StaticMeshComponent.h"
#include "BBBWork/UBBBNexus/Player/UI/BBBPlayerItemPanelComponent.h"
#include "Engine/SceneCapture2D.h"
#include "Engine/StaticMesh.h"
#include "Materials/MaterialInstanceDynamic.h"

namespace
{
UWidgetComponent* CreateRoomWidget(AActor& Owner, const TSharedPtr<SWidget>& Widget, FVector2D Size,
    FVector Location, FRotator Rotation, float UnitsPerPixel)
{
    auto* Panel = NewObject<UBBBPlayerItemPanelComponent>(&Owner);
    Owner.AddInstanceComponent(Panel);
    Panel->SetupAttachment(Owner.GetRootComponent());
    Panel->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    Panel->SetCastShadow(false);
    Panel->SetVisibleInSceneCaptureOnly(true);
    Panel->SetTickWhenOffscreen(true);
    Panel->SetBlendMode(EWidgetBlendMode::Transparent);
    Panel->SetOutputDepthAndVelocity(true);
    Panel->SetRenderTargetPixelsPerCentimeter(1.5f);
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

    auto* Key = NewObject<USpotLightComponent>(Display, TEXT("ItemRoomKey"));
    RoomKey = Key;
    Display->AddInstanceComponent(Key);
    Key->SetupAttachment(&Root);
    const FVector FloorAim(-38.0, -32.0, 0.0);
    Key->SetRelativeLocation(FVector(78, 65, 327));
    Key->SetRelativeRotation((FloorAim - Key->GetRelativeLocation()).Rotation());
    Key->SetIntensityUnits(ELightUnits::Candelas);
    Key->SetIntensity(58.0f);
    Key->SetLightColor(FLinearColor(1.0f, 0.84f, 0.68f));
    Key->SetInnerConeAngle(12.0f);
    Key->SetOuterConeAngle(30.0f);
    Key->SetSourceRadius(53.0f);
    Key->SetSoftSourceRadius(26.5f);
    Key->SetAttenuationRadius(625);
    Key->SetCastShadows(true);
    Key->ContactShadowLength = 0.10f;
    Key->ShadowResolutionScale = 2.0f;
    Key->RegisterComponent();

    const auto AddRim = [&](FName Name, FVector Location, FVector Target, FLinearColor Color, float Intensity, float Cone, float Radius)
    {
        auto* Light = NewObject<USpotLightComponent>(Display, Name);
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
        Light->SetSourceRadius(Radius);
        Light->SetCastShadows(false);
        Light->SetVolumetricScatteringIntensity(0.0f);
        Light->RegisterComponent();
    };
    AddRim(TEXT("ItemRoomCoolRim"), FVector(-14, -137, 214), FVector(0, 0, 114),
        FLinearColor(0.68f, 0.19f, 1.0f), 118.0f, 38.0f, 42.0f);
    AddRim(TEXT("ItemRoomWarmRim"), FVector(-41, 126, 222), FVector(0, 0, 130),
        FLinearColor(1.0f, 0.80f, 0.60f), 37.0f, 36.0f, 30.0f);

    auto* Bounce = NewObject<URectLightComponent>(Display, TEXT("ItemRoomFill"));
    Display->AddInstanceComponent(Bounce);
    Bounce->SetupAttachment(&Root);
    Bounce->SetRelativeLocation(FVector(135, 78, 145));
    Bounce->SetRelativeRotation((FVector(12, 0, 109) - Bounce->GetRelativeLocation()).Rotation());
    Bounce->SetIntensityUnits(ELightUnits::Candelas);
    Bounce->SetIntensity(2.6f);
    Bounce->SetSourceWidth(105.0f);
    Bounce->SetSourceHeight(50.0f);
    Bounce->SetAttenuationRadius(275.0f);
    Bounce->SetLightColor(FLinearColor(1.0f, 0.89f, 0.76f));
    Bounce->SetCastShadows(false);
    Bounce->SetVolumetricScatteringIntensity(0.0f);
    Bounce->RegisterComponent();

    auto* WarmMist = NewObject<UStaticMeshComponent>(Display);
    RoomMist = WarmMist;
    Display->AddInstanceComponent(WarmMist);
    WarmMist->SetupAttachment(&Root);
    WarmMist->SetStaticMesh(LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Sphere.Sphere")));
    WarmMist->SetMaterial(0, LoadObject<UMaterialInterface>(nullptr,
        TEXT("/Game/_Project/UI/Bag/Materials/M_InventoryWarmMist.M_InventoryWarmMist")));
    WarmMist->SetRelativeLocation(FVector(-50, 52, 158));
    WarmMist->SetRelativeScale3D(FVector(8.2, 8.0, 7.8));
    WarmMist->SetCastShadow(false);
    WarmMist->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    WarmMist->SetTranslucentSortPriority(-1);
    WarmMist->RegisterComponent();
    UpdateRoomMist();
}

void UBBBPlayerItemPortrait::UpdateRoomMist()
{
    // 雾束与落在人物及地面的聚光共享灯位和锥角
    USpotLightComponent& Key = *RoomKey;
    const FVector LightOrigin = Key.GetComponentLocation() - RoomMist->Bounds.Origin;
    const FVector Direction = Key.GetForwardVector();
    const double OuterCos = FMath::Cos(FMath::DegreesToRadians(Key.OuterConeAngle));
    const double InnerCos = FMath::Cos(FMath::DegreesToRadians(Key.InnerConeAngle));
    RoomMist->SetCustomPrimitiveDataVector4(0, FVector4(LightOrigin, Key.Intensity));
    RoomMist->SetCustomPrimitiveDataVector4(4, FVector4(Direction, OuterCos));
    RoomMist->SetCustomPrimitiveDataVector4(8, FVector4(InnerCos, Key.AttenuationRadius, 4.9, 0.58));
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
    const FQuat RotationAtRest = HomeCamera.GetRotation();
    const FVector Forward = RotationAtRest.GetForwardVector();
    const FVector Right = RotationAtRest.GetRightVector();
    const FVector Up = RotationAtRest.GetUpVector();
    const FVector Centre = HomeCamera.GetLocation() + Forward * Depth - Origin;
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
