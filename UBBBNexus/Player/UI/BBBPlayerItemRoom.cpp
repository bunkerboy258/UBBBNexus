#include "BBBWork/UBBBNexus/Player/UI/BBBPlayerItemPortrait.h"
#include "Components/RectLightComponent.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Components/SpotLightComponent.h"
#include "Components/StaticMeshComponent.h"
#include "BBBWork/UBBBNexus/Player/UI/BBBPlayerItemPanelComponent.h"
#include "Engine/SceneCapture2D.h"
#include "Engine/StaticMesh.h"
#include "Materials/MaterialInterface.h"

namespace
{
constexpr float PanelDepth = 240.0f;

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
    Panel->SetRenderTargetPixelsPerCentimeter(1.0f);
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
    auto* Concrete = LoadObject<UMaterialInterface>(nullptr,
        TEXT("/Game/_Project/UI/Bag/Materials/M_InventorySurveyFloor.M_InventorySurveyFloor"));
    auto* Floor = NewObject<UStaticMeshComponent>(Display);
    Display->AddInstanceComponent(Floor);
    Floor->SetupAttachment(&Root);
    Floor->SetStaticMesh(Block);
    Floor->SetMaterial(0, Concrete);
    Floor->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    Floor->SetRelativeLocation(FVector(-1110, 0, -15));
    Floor->SetRelativeScale3D(FVector(35, 16, 0.3));
    Floor->RegisterComponent();

    auto* Key = NewObject<USpotLightComponent>(Display, TEXT("ItemRoomKey"));
    RoomKey = Key;
    Display->AddInstanceComponent(Key);
    Key->SetupAttachment(&Root);
    const FVector FloorAim(-12.0, 0.0, 0.0);
    Key->SetRelativeLocation(FVector(102, 161, 341));
    Key->SetRelativeRotation((FloorAim - Key->GetRelativeLocation()).Rotation());
    Key->SetIntensityUnits(ELightUnits::Candelas);
    Key->SetIntensity(97.0f);
    Key->SetLightColor(FLinearColor(1.0f, 0.83f, 0.65f));
    Key->SetInnerConeAngle(12.5f);
    Key->SetOuterConeAngle(25.0f);
    Key->SetSourceRadius(37.0f);
    Key->SetSoftSourceRadius(18.0f);
    Key->SetAttenuationRadius(535.0f);
    Key->SetCastShadows(true);
    Key->ContactShadowLength = 0.10f;
    Key->ShadowResolutionScale = 2.0f;
    Key->RegisterComponent();

    auto* Rim = NewObject<USpotLightComponent>(Display, TEXT("ItemRoomCoolRim"));
    Display->AddInstanceComponent(Rim);
    Rim->SetupAttachment(&Root);
    const FVector RimAim(0.0, 0.0, 114.0);
    Rim->SetRelativeLocation(FVector(-179, -118, 226));
    Rim->SetRelativeRotation((RimAim - Rim->GetRelativeLocation()).Rotation());
    Rim->SetLightingChannels(false, true, false);
    Rim->SetIntensityUnits(ELightUnits::Candelas);
    Rim->SetIntensity(165.0f);
    Rim->SetLightColor(FLinearColor(0.67f, 0.24f, 1.0f));
    Rim->SetInnerConeAngle(16.0f);
    Rim->SetOuterConeAngle(30.0f);
    Rim->SetAttenuationRadius(355.0f);
    Rim->SetSourceRadius(22.0f);
    Rim->SetCastShadows(true);
    Rim->SetVolumetricScatteringIntensity(0.0f);
    Rim->RegisterComponent();

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
    WarmMist->SetRelativeLocation(FVector(28, 86, 219));
    WarmMist->SetRelativeScale3D(FVector(5.8, 5.2, 5.4));
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
    RoomMist->SetCustomPrimitiveDataVector4(8, FVector4(InnerCos, Key.AttenuationRadius, 6.5, 0.48));
}

void UBBBPlayerItemPortrait::SetPanels(const TArray<TSharedPtr<SWidget>>& Content)
{
    if (!Capture || !Display || !Panels.IsEmpty() || Content.Num() != 3)
    {
        return;
    }

    const auto* Camera = Capture->GetCaptureComponent2D();
    const float Units = 2.0f * PanelDepth * FMath::Tan(FMath::DegreesToRadians(Camera->FOVAngle * 0.5f)) / 1920.0f;
    const FQuat RotationAtRest = HomeCamera.GetRotation();
    const FVector Forward = RotationAtRest.GetForwardVector();
    const FVector Right = RotationAtRest.GetRightVector();
    const FVector Up = RotationAtRest.GetUpVector();
    const FVector Centre = HomeCamera.GetLocation() + Forward * PanelDepth - Origin;
    const FVector2D Centres[] = {FVector2D(960, 540), FVector2D(385, 574), FVector2D(1535, 574)};
    const FVector2D Sizes[] = {FVector2D(1920, 1080), FVector2D(520, 800), FVector2D(520, 800)};
    const float Lean[] = {0.0f, -18.0f, 18.0f};

    for (int32 Index = 1; Index < Content.Num(); ++Index)
    {
        const FVector2D ScreenOffset = Centres[Index] - FVector2D(960, 540);
        const FVector Location = Centre + Units * (Right * ScreenOffset.X - Up * ScreenOffset.Y);
        const FVector Normal = (-Forward).RotateAngleAxis(Lean[Index], Up);
        const FRotator Rotation = FRotationMatrix::MakeFromXZ(Normal, Up).Rotator();
        Panels.Add(CreateRoomWidget(*Display, Content[Index], Sizes[Index], Location, Rotation, Units * 1.04f));
        Panels.Last()->SetTintColorAndOpacity(FLinearColor(1.8f, 1.8f, 1.8f, 1.0f));
    }
}
