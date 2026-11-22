#include "BBBWork/UBBBNexus/Player/UI/BBBPlayerItemPortrait.h"
#include "BBBWork/UBBBNexus/Player/UI/BBBPlayerItemStage.h"
#include "Components/SceneCaptureComponent2D.h"
#include "BBBWork/UBBBNexus/Player/UI/BBBPlayerItemPanelComponent.h"

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
    Panel->SetRenderTargetPixelsPerCentimeter(1.5f);
    Panel->SetBackgroundColor(FLinearColor::Transparent);
    Panel->SetDrawSize(Size);
    Panel->SetWorldTransform(FTransform(Rotation, Location, FVector(UnitsPerPixel)));
    Panel->SetSlateWidget(Widget);
    Panel->RegisterComponent();
    return Panel;
}
}

void UBBBPlayerItemPortrait::SetPanels(const TSharedPtr<SWidget>& Storage, const TSharedPtr<SWidget>& Loadout,
    const TSharedPtr<SWidget>& Navigation)
{
    const auto* Camera = GetCamera();
    if (!Camera || !Display || !Panels.IsEmpty() || !Storage || !Loadout || !Navigation)
    {
        return;
    }

    const float Units = 2.0f * PanelDepth * FMath::Tan(FMath::DegreesToRadians(Camera->FOVAngle * 0.5f)) / 1920.0f;
    const FQuat RotationAtRest = HomeCamera.GetRotation();
    const FVector Forward = RotationAtRest.GetForwardVector();
    const FVector Right = RotationAtRest.GetRightVector();
    const FVector Up = RotationAtRest.GetUpVector();
    const FVector Centre = HomeCamera.GetLocation() + Forward * PanelDepth;
    const auto AddPanel = [&](const TSharedPtr<SWidget>& Content, FVector2D Size, FVector2D Offset,
        float Lean, float Scale, const FVector& Adjustment)
    {
        const FVector Location = Centre + Units * (Right * Offset.X - Up * Offset.Y) + Adjustment;
        const FVector Normal = (-Forward).RotateAngleAxis(Lean, Up);
        const FRotator Rotation = FRotationMatrix::MakeFromXZ(Normal, Up).Rotator();
        Panels.Add(CreateRoomWidget(*Display, Content, Size, Location, Rotation, Units * Scale));
    };

    const FVector2D SideSize(520.0, 800.0);
    AddPanel(Storage, SideSize, FVector2D(-575.0, 34.0), -18.0f, 1.04f, FVector::ZeroVector);
    AddPanel(Loadout, SideSize, FVector2D(575.0, 34.0), 18.0f, 1.04f, FVector::ZeroVector);
    AddPanel(Navigation, FVector2D(880.0, 54.0), FVector2D(0.0, -502.0), 0.0f, 1.0f,
        RotationAtRest.RotateVector(Stage->GetNavigationOffset()));
}
