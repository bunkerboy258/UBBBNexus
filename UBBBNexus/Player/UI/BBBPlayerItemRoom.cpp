#include "BBBWork/UBBBNexus/Player/UI/BBBPlayerItemPortrait.h"
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
    Panel->SetRenderTargetPixelsPerCentimeter(1.0f);
    Panel->SetBackgroundColor(FLinearColor::Transparent);
    Panel->SetDrawSize(Size);
    Panel->SetWorldTransform(FTransform(Rotation, Location, FVector(UnitsPerPixel)));
    Panel->SetSlateWidget(Widget);
    Panel->RegisterComponent();
    return Panel;
}
}

void UBBBPlayerItemPortrait::SetPanels(const TArray<TSharedPtr<SWidget>>& Content)
{
    const auto* Camera = GetCamera();
    if (!Camera || !Display || !Panels.IsEmpty() || Content.Num() != 3)
    {
        return;
    }

    const float Units = 2.0f * PanelDepth * FMath::Tan(FMath::DegreesToRadians(Camera->FOVAngle * 0.5f)) / 1920.0f;
    const FQuat RotationAtRest = HomeCamera.GetRotation();
    const FVector Forward = RotationAtRest.GetForwardVector();
    const FVector Right = RotationAtRest.GetRightVector();
    const FVector Up = RotationAtRest.GetUpVector();
    const FVector Centre = HomeCamera.GetLocation() + Forward * PanelDepth;
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
