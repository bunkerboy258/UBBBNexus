#include "BBBWork/UBBBNexus/Player/UI/BBBPlayerItemView.h"
#include "BBBWork/UBBBNexus/Player/UI/BBBPlayerItemStyle.h"
#include "BBBWork/UBBBNexus/Player/UI/BBBPlayerItemText.h"
#include "BBBWork/UBBBNexus/Player/BBBPlayerController.h"
#include "BBBWork/UBBBNexus/Player/BBBPlayerItemDisplayData.h"
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Base/BBBConsumable.h"
#include "Widgets/Layout/SScaleBox.h"
#include "Widgets/Layout/SScrollBox.h"
#include "Widgets/Layout/SScrollBar.h"
#include "Widgets/Layout/SConstraintCanvas.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Images/SImage.h"
#include "BBBWork/UBBBNexus/Player/UI/BBBPlayerItemPortrait.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Components/WidgetComponent.h"
#include "Engine/SceneCapture2D.h"
#include "Input/HittestGrid.h"
#include "Layout/ArrangedChildren.h"
#include "Layout/WidgetPath.h"
#include "Types/PaintArgs.h"
#include "Widgets/SWindow.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Notifications/SProgressBar.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/SOverlay.h"
#include "Widgets/Text/STextBlock.h"
#include "Brushes/SlateColorBrush.h"
#include "Brushes/SlateNoResource.h"
#include "Styling/CoreStyle.h"

namespace
{
class FBBBInventoryHitPath final : public ICustomHitTestPath
{
public:
    TAttribute<UBBBPlayerItemPortrait*> Room;
    TWeakPtr<SWidget> Surface;

    TArray<FWidgetAndPointer> GetBubblePathAndVirtualCursors(const FGeometry& Geometry,
        FVector2D DesktopPosition, bool bIgnoreEnabled) const override
    {
        const auto* Preview = Room.Get();
        if (Preview)
        {
            const auto& Panels = Preview->GetPanels();
            for (int32 Index = Panels.Num() - 1; Index >= 0; --Index)
            {
                UWidgetComponent* Panel = Panels[Index];
                const auto Point = Project(Geometry, *Panel, DesktopPosition);
                if (Point.IsSet())
                {
                    const FVector2D Size = Panel->GetCurrentDrawSize();
                    if (Point->X >= 0 && Point->Y >= 0 && Point->X < Size.X && Point->Y < Size.Y)
                    {
                        auto Path = Panel->GetHitWidgetPath(*Point, bIgnoreEnabled);
                        if (Path.Num() > 1)
                        {
                            auto Window = Panel->GetSlateWindow();
                            auto Parent = Surface.Pin();
                            if (Window && Parent && Window->GetParentWidget() != Parent)
                            {
                                Window->AssignParentWidget(Parent);
                            }
                            return Path;
                        }
                    }
                }
            }
        }
        return {};
    }

    void ArrangeCustomHitTestChildren(FArrangedChildren& Children) const override
    {
        const auto* Preview = Room.Get();
        if (Preview)
        {
            for (const UWidgetComponent* Panel : Preview->GetPanels())
            {
                if (auto Window = Panel->GetSlateWindow())
                {
                    Children.AddWidget(FArrangedWidget(Window.ToSharedRef(),
                        FGeometry::MakeRoot(FVector2D(Panel->GetCurrentDrawSize()), FSlateLayoutTransform())));
                }
            }
        }
    }

    TOptional<FVirtualPointerPosition> TranslateMouseCoordinateForCustomHitTestChild(const SWidget& Child,
        const FGeometry& Geometry, FVector2D Current, FVector2D Last) const override
    {
        const auto* Preview = Room.Get();
        if (Preview)
        {
            for (const UWidgetComponent* Panel : Preview->GetPanels())
            {
                if (Panel->GetSlateWindow().Get() == &Child)
                {
                    const auto Point = Project(Geometry, *Panel, Current);
                    const auto LastPoint = Project(Geometry, *Panel, Last);
                    if (Point.IsSet())
                    {
                        return FVirtualPointerPosition(*Point, LastPoint.Get(*Point));
                    }
                }
            }
        }
        return {};
    }

private:
    TOptional<FVector2D> Project(const FGeometry& Geometry, const UWidgetComponent& Panel, FVector2D DesktopPosition) const
    {
        const auto* Preview = Room.Get();
        const auto* CameraActor = Preview ? Preview->GetCamera() : nullptr;
        const FVector2D Size = Geometry.GetLocalSize();
        if (!CameraActor || Size.X <= 0 || Size.Y <= 0)
        {
            return {};
        }
        const auto* Camera = CameraActor->GetCaptureComponent2D();
        const FVector2D UV = Geometry.AbsoluteToLocal(DesktopPosition) / Size;
        const double HalfWidth = FMath::Tan(FMath::DegreesToRadians(Camera->FOVAngle * 0.5));
        const FVector Ray = Camera->GetForwardVector() + Camera->GetRightVector() * ((UV.X * 2.0 - 1.0) * HalfWidth)
            + Camera->GetUpVector() * ((1.0 - UV.Y * 2.0) * HalfWidth * 9.0 / 16.0);
        const FVector Start = Camera->GetComponentLocation();
        const FVector Normal = Panel.GetForwardVector();
        const double Divisor = FVector::DotProduct(Normal, Ray);
        if (Divisor >= -UE_SMALL_NUMBER)
        {
            return {};
        }
        const double Distance = FVector::DotProduct(Normal, Panel.GetComponentLocation() - Start) / Divisor;
        FVector2D Result;
        Panel.GetLocalHitLocation(Start + Ray * Distance, Result);
        return Result;
    }
};

class SBBBInventoryRoomImage final : public SImage
{
public:
    SLATE_BEGIN_ARGS(SBBBInventoryRoomImage) {}
        SLATE_ATTRIBUTE(UBBBPlayerItemPortrait*, Room)
        SLATE_ATTRIBUTE(const FSlateBrush*, Image)
    SLATE_END_ARGS()

    void Construct(const FArguments& Args)
    {
        Path = MakeShared<FBBBInventoryHitPath>();
        Path->Room = Args._Room;
        Path->Surface = AsShared();
        SImage::Construct(SImage::FArguments().Image(Args._Image));
    }

    void OnArrangeChildren(const FGeometry& Geometry, FArrangedChildren& Children) const override
    {
        if (Children.Allows3DWidgets())
        {
            Path->ArrangeCustomHitTestChildren(Children);
        }
    }

    TOptional<FVirtualPointerPosition> TranslateMouseCoordinateForCustomHitTestChild(const SWidget& Child,
        const FGeometry& Geometry, FVector2D Current, FVector2D Last) const override
    {
        return Path->TranslateMouseCoordinateForCustomHitTestChild(Child, Geometry, Current, Last);
    }

    int32 OnPaint(const FPaintArgs& Args, const FGeometry& Geometry, const FSlateRect& Culling,
        FSlateWindowElementList& Elements, int32 Layer, const FWidgetStyle& Style, bool bParentEnabled) const override
    {
        Args.GetHittestGrid().InsertCustomHitTestPath(this, Path.ToSharedRef());
        return SImage::OnPaint(Args, Geometry, Culling, Elements, Layer, Style, bParentEnabled);
    }

private:
    TSharedPtr<FBBBInventoryHitPath> Path;
};


}

TSharedRef<SWidget> UBBBPlayerItemView::RebuildWidget()
{
    QuickSlotViews.Reset();
    StorageSlotViews.Reset();
    PanelSlotViews.Reset();
    BuiltQuickStart = INDEX_NONE;
    BuiltStorageStart = INDEX_NONE;
    BuiltPanelStart = INDEX_NONE;
    TSharedRef<SWidget> Result =
        SNew(SOverlay) + SOverlay::Slot()[MakeGameplayHud()] +
        SOverlay::Slot()
            .HAlign(HAlign_Center)
            .VAlign(VAlign_Top)
            .Padding(0.0f, 80.0f)[SNew(SBorder)
                                      .Visibility(EVisibility::HitTestInvisible)
                                      .BorderImage(FCoreStyle::Get().GetBrush("NoBrush"))
                                      .Padding(0.0f)
                                      .ColorAndOpacity_Lambda(
                                          [this]()
                                          {
                                              return FLinearColor(1.0f, 1.0f, 1.0f, GetQuickSelectionOpacity());
                                          })[SNew(SBox).WidthOverride(900.0f)[SAssignNew(QuickBar, SHorizontalBox)]]] +
        SOverlay::Slot()[SNew(SOverlay).Visibility_Lambda(
                             [this]()
                             {
                                 return bBackpackOpen ? EVisibility::Visible : EVisibility::Collapsed;
                             }) +
                         SOverlay::Slot()[MakeBackpack()]] +
        SOverlay::Slot()[MakeExplorationHud()];
    RefreshItems();
    return Result;
}

TSharedRef<SWidget> UBBBPlayerItemView::MakeBackpack()
{
    using namespace BBBItemUI;
    TSharedRef<SConstraintCanvas> Canvas = SNew(SConstraintCanvas);
    const auto LeftWing = SNew(SConstraintCanvas);
    const auto RightWing = SNew(SConstraintCanvas);
    const auto Place = [&Canvas, &LeftWing, &RightWing](float X, float Y, float W, float H, TSharedRef<SWidget> Widget)
    {
        const bool Left = X < 600 && Y >= 180 && Y < 980;
        const bool Right = X >= 1300 && Y >= 180 && Y < 980;
        auto Target = Left ? LeftWing : Right ? RightWing : Canvas;
        Target->AddSlot().Alignment(FVector2D::ZeroVector)
            .Offset(FMargin(X - (Left ? 90 : Right ? 1306 : 0), Y - (Left || Right ? 174 : 0), W, H))[Widget];
    };
    const auto Label = [](const FText &Text, int32 Size)
    {
        return SNew(STextBlock).Text(Text).Font(ArtFont(Size)).ColorAndOpacity(FLinearColor::White)
            .ShadowOffset(FVector2D(1, 2)).ShadowColorAndOpacity(FLinearColor(0, 0, 0, 0.8f));
    };
    const auto Tab = [this](const FText &Text, EBBBItemPanelPage Page)
    {
        return SNew(SButton).Tag(FName(*FString::Printf(TEXT("Backpack.Panel.%d"), static_cast<int>(Page))))
            .ButtonStyle(BBBItemUI::Navigation()).HAlign(HAlign_Fill).VAlign(VAlign_Fill)
            .OnClicked_Lambda([this, Page] { return ShowPanel(Page); })
            [SNew(SOverlay)
                + SOverlay::Slot()[SNew(SImage).Image(Selection()).ColorAndOpacity(FLinearColor(1, 1, 1, 0.65f))]
                + SOverlay::Slot().Padding(2)[SNew(SImage)
                    .Image(Brush(TEXT("/Game/_ThirdParty/UI/EditableSurvivalHorrorUI/Inventory/T_Inventory_ContextMenu_HoverOpt.T_Inventory_ContextMenu_HoverOpt")))
                    .ColorAndOpacity(FLinearColor(0.54f, 0.52f, 0.50f, 0.88f))
                    .Visibility_Lambda([this, Page] { return PanelPage == Page ? EVisibility::HitTestInvisible : EVisibility::Hidden; })]
                + SOverlay::Slot().HAlign(HAlign_Center).VAlign(VAlign_Center)[SNew(STextBlock).Text(Text).Font(ArtFont(26))
                    .ColorAndOpacity(FLinearColor::White)]];
    };
    const auto Hairline = [&Place](float X, float Y, float W)
    {
        Place(X, Y, W, 3, SNew(SImage).Image(Brush(TEXT("/Game/_ThirdParty/UI/EditableSurvivalHorrorUI/Settings_Screen/T_SeparatorLine.T_SeparatorLine")))
            .ColorAndOpacity(FLinearColor(0.84f, 0.78f, 0.65f, 0.7f)));
    };
    static const FSlateBrush Keycap = []()
    {
        FSlateBrush Cap = *Brush(TEXT("/Game/_ThirdParty/UI/EditableSurvivalHorrorUI/Settings_Screen/T_Esc_Key.T_Esc_Key"));
        Cap.DrawAs = ESlateBrushDrawType::Border;
        Cap.Margin = FMargin(0.19f);
        return Cap;
    }();
    const auto Fit = [](TSharedRef<SWidget> Content, float DesignHeight)
    {
        return SNew(SScaleBox).Stretch(EStretch::ScaleToFit).Visibility(EVisibility::SelfHitTestInvisible)
            [SNew(SBox).WidthOverride(1920).HeightOverride(DesignHeight).Visibility(EVisibility::SelfHitTestInvisible)[Content]];
    };
    Place(0, 15, 1920, 46, SNew(SBox).HAlign(HAlign_Center).Visibility(EVisibility::SelfHitTestInvisible)
        [SNew(SHorizontalBox)
            + SHorizontalBox::Slot().AutoWidth().Padding(0, 0, 16, 0)
                [SNew(SBox).WidthOverride(246)[Tab(INVTEXT("INVENTORY"), EBBBItemPanelPage::Inventory)]]
            + SHorizontalBox::Slot().AutoWidth().Padding(0, 0, 25, 0)
                [SNew(SBox).WidthOverride(246)[Tab(INVTEXT("CHARACTER"), EBBBItemPanelPage::Character)]]
            + SHorizontalBox::Slot().AutoWidth().VAlign(VAlign_Center)
                [SNew(STextBlock).Text(INVTEXT("EXTENSIBLE")).Font(ArtFont(24))
                    .ColorAndOpacity(FLinearColor(0.6f, 0.58f, 0.53f, 0.48f))]]);
    Place(676, 155, 568, 48, SNew(SBox).HAlign(HAlign_Center).VAlign(VAlign_Center)
        [
            SNew(SHorizontalBox)
            + SHorizontalBox::Slot().AutoWidth().VAlign(VAlign_Center).Padding(0, 0, 11, 4)
                [SNew(SBox).WidthOverride(24).HeightOverride(24)
                    [SNew(SImage).Image(Brush(TEXT("/Game/_ThirdParty/UI/EditableSurvivalHorrorUI/HUD/T_HUD_03.T_HUD_03")))]]
            + SHorizontalBox::Slot().AutoWidth().VAlign(VAlign_Center)
                [SNew(STextBlock).Font(ArtFont(30)).ColorAndOpacity(FLinearColor::White)
                    .ShadowOffset(FVector2D(1, 2)).ShadowColorAndOpacity(FLinearColor(0, 0, 0, 0.9f))
                    .Text_Lambda([this]
                    {
                        const auto *Controller = GetItemController();
                        const auto *Player = Controller ? Cast<ABBBCharacter>(Controller->GetPawn()) : nullptr;
                        if (!Player)
                        {
                            return FText::GetEmpty();
                        }
                        const float Maximum = Player->GetLifePhase() == EBBBCharacterLifePhase::Downed
                            ? Player->GetCharacterConfig().DownedHealth : Player->GetCharacterConfig().MaximumHealth;
                        return FText::Format(INVTEXT("{0}  /  {1}"), FText::AsNumber(FMath::RoundToInt(Player->GetHealth())),
                            FText::AsNumber(FMath::RoundToInt(Maximum)));
                    })]
        ]);
    Place(105, 184, 475, 68, Label(INVTEXT("BACKPACK"), 44));
    Hairline(105, 254, 480);
    const auto Filter = [this](const FText &Text, EBBBItemType Type)
    {
        return SNew(SButton).Tag(FName(*FString::Printf(TEXT("Backpack.Storage.%d"), static_cast<int>(Type))))
            .ButtonStyle(BBBItemUI::Navigation()).OnClicked_Lambda([this, Type] { return ShowStorage(Type); })
            [SNew(STextBlock).Text(Text).Font(ArtFont(18)).ColorAndOpacity_Lambda([this, Type]
            { return Type == StoragePage ? FLinearColor::White : FLinearColor(0.69f, 0.65f, 0.57f); })];
    };
    Place(108, 275, 476, 42, SNew(SHorizontalBox).Visibility_Lambda([this]
        { return PanelPage == EBBBItemPanelPage::Inventory ? EVisibility::Visible : EVisibility::Collapsed; })
        + SHorizontalBox::Slot().AutoWidth().Padding(0, 0, 23, 0)[Filter(INVTEXT("WEAPONS"), EBBBItemType::Equipment)]
        + SHorizontalBox::Slot().AutoWidth().Padding(0, 0, 23, 0)[Filter(INVTEXT("CONSUMABLES"), EBBBItemType::Consumable)]
        + SHorizontalBox::Slot().AutoWidth()[Filter(INVTEXT("MISC"), EBBBItemType::Misc)]);
    Place(108, 275, 470, 42, SNew(STextBlock).Text(INVTEXT("WEARABLES")).Font(ArtFont(18))
        .Visibility_Lambda([this] { return PanelPage == EBBBItemPanelPage::Character ? EVisibility::HitTestInvisible : EVisibility::Hidden; }));
    static const FScrollBoxStyle StorageScroll = FScrollBoxStyle()
        .SetTopShadowBrush(FSlateNoResource()).SetBottomShadowBrush(FSlateNoResource());
    Place(101, 330, 491, 612, SNew(SScrollBox).Style(&StorageScroll).ScrollBarVisibility(EVisibility::Collapsed)
        + SScrollBox::Slot()[SAssignNew(BackpackSlots, SVerticalBox)]);
    Place(1324, 184, 490, 68, SNew(SBox).HAlign(HAlign_Right)
        [SNew(STextBlock).Font(ArtFont(44)).Text_Lambda([this]
            { return PanelPage == EBBBItemPanelPage::Character ? INVTEXT("GEAR") : INVTEXT("LOADOUT"); })]);
    Hairline(1324, 254, 490);
    Place(1324, 275, 490, 40, SNew(SBox).HAlign(HAlign_Right)
        [SNew(STextBlock).Font(ArtFont(20)).ColorAndOpacity(MutedText)
            .Text_Lambda([this] { return PanelPage == EBBBItemPanelPage::Character ? INVTEXT("EQUIPPED") : INVTEXT("WEAPONS"); })]);
    Place(1320, 330, 494, 664, SAssignNew(EquipmentSlots, SVerticalBox).Tag(TEXT("Backpack.PanelSlots")));
    Place(96, 1030, 160, 42, SNew(SButton).ButtonStyle(BBBItemUI::Navigation()).HAlign(HAlign_Left)
        .OnClicked_Lambda([this] { GetItemController()->ToggleBackpack(); return FReply::Handled(); })
        [SNew(SHorizontalBox)
            + SHorizontalBox::Slot().AutoWidth().VAlign(VAlign_Center)
                [SNew(SBox).WidthOverride(29).HeightOverride(29)
                    [SNew(SOverlay) + SOverlay::Slot()[SNew(SImage).Image(&Keycap)]
                        + SOverlay::Slot().HAlign(HAlign_Center).VAlign(VAlign_Center)[Label(INVTEXT("B"), 16)]]]
            + SHorizontalBox::Slot().AutoWidth().VAlign(VAlign_Center).Padding(10, 0)[Label(INVTEXT("CLOSE"), 18)]]);
    Place(345, 1037, 700, 32, SAssignNew(StatusText, STextBlock).Font(Font(17)).ColorAndOpacity(MutedText));
    const auto Chrome = Fit(Canvas, 1080);
    MenuPanels = {Chrome, LeftWing, RightWing};
    static const FSlateColorBrush Matte(FLinearColor::Black);
    return SNew(SOverlay)
        + SOverlay::Slot()[SNew(SImage).Image(&Matte).Visibility(EVisibility::HitTestInvisible)]
        + SOverlay::Slot()[Fit(SNew(SBBBInventoryRoomImage)
            .Room_Lambda([this] { return CharacterPreview.Get(); })
            .Image_Lambda([this] { return GetCharacterBrush(); }), 1080)]
        + SOverlay::Slot()[Chrome];
}
