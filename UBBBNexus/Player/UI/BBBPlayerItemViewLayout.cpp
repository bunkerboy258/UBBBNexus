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

/** @param bUpper	是否为菜单上包边 @return 从素材包磨损边框切出的横向画刷 */
const FSlateBrush *MenuTrim(bool bUpper)
{
    const auto CropEdge = [](float Top, float Bottom)
    {
        FSlateBrush Edge = *BBBItemUI::Brush(TEXT("/Game/_ThirdParty/UI/EditableSurvivalHorrorUI/Weapon_Customization/T_Attachment_Slot.T_Attachment_Slot"));
        Edge.SetUVRegion(FBox2f(FVector2f(0.0f, Top), FVector2f(1.0f, Bottom)));
        return Edge;
    };
    // 让磨损边沿朝向页面内容
    static const FSlateBrush Upper = CropEdge(372.0f / 394.0f, 1.0f);
    static const FSlateBrush Lower = CropEdge(0.0f, 22.0f / 394.0f);
    return bUpper ? &Upper : &Lower;
}
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
    const auto TopActions = SNew(SConstraintCanvas);
    const auto BottomActions = SNew(SConstraintCanvas);
    const auto Place = [&Canvas, &LeftWing, &RightWing, &TopActions, &BottomActions](float X, float Y, float W, float H, TSharedRef<SWidget> Widget)
    {
        const bool Left = X < 600 && Y >= 180 && Y < 980;
        const bool Right = X >= 1300 && Y >= 180 && Y < 980;
        auto Target = Left ? LeftWing : Right ? RightWing : Y < 108 ? TopActions : Y >= 984 ? BottomActions : Canvas;
        Target->AddSlot().Alignment(FVector2D::ZeroVector)
            .Offset(FMargin(X - (Left ? 90 : Right ? 1306 : 0), Y - (Left || Right ? 174 : Y >= 984 ? 984 : 0), W, H))[Widget];
    };
    const auto Label = [](const FText &Text, int32 Size)
    {
        return SNew(STextBlock).Text(Text).Font(ArtFont(Size)).ColorAndOpacity(FLinearColor::White)
            .ShadowOffset(FVector2D(1, 2)).ShadowColorAndOpacity(FLinearColor(0, 0, 0, 0.8f));
    };
    const auto Tab = [this, &Label](const FText &Text, EBBBItemPanelPage Page)
    {
        return SNew(SButton).Tag(FName(*FString::Printf(TEXT("Backpack.Panel.%d"), static_cast<int>(Page))))
            .ButtonStyle(BBBItemUI::Navigation()).HAlign(HAlign_Center).OnClicked_Lambda([this, Page] { return ShowPanel(Page); })
            [SNew(SVerticalBox)
                + SVerticalBox::Slot().AutoHeight()[Label(Text, 34)]
                + SVerticalBox::Slot().AutoHeight().Padding(0, 3)
                    [SNew(SBox).HeightOverride(3)[SNew(SImage).Image(Brush(TEXT("/Game/_ThirdParty/UI/EditableSurvivalHorrorUI/Settings_Screen/T_SeparatorLine.T_SeparatorLine")))
                        .ColorAndOpacity(Accent).Visibility_Lambda([this, Page]
                        { return PanelPage == Page ? EVisibility::HitTestInvisible : EVisibility::Hidden; })]]];
    };
    const auto Hairline = [&Place](float X, float Y, float W)
    {
        Place(X, Y, W, 3, SNew(SImage).Image(Brush(TEXT("/Game/_ThirdParty/UI/EditableSurvivalHorrorUI/Settings_Screen/T_SeparatorLine.T_SeparatorLine")))
            .ColorAndOpacity(FLinearColor(0.84f, 0.78f, 0.65f, 0.7f)));
    };
    static const FSlateColorBrush BarBase(FLinearColor(0.024f, 0.022f, 0.024f));
    static const FSlateBrush BarGrain = []()
    {
        FSlateBrush Grain = *Brush(TEXT("/Game/_ThirdParty/UI/EditableSurvivalHorrorUI/Inventory/T_Inventory_ContextMenuBG.T_Inventory_ContextMenuBG"));
        Grain.SetUVRegion(FBox2f(FVector2f(0.04f, 0.1f), FVector2f(0.96f, 0.9f)));
        return Grain;
    }();
    const auto Fit = [](TSharedRef<SWidget> Content, float DesignHeight)
    {
        return SNew(SScaleBox).Stretch(EStretch::ScaleToFit).Visibility(EVisibility::SelfHitTestInvisible)
            [SNew(SBox).WidthOverride(1920).HeightOverride(DesignHeight).Visibility(EVisibility::SelfHitTestInvisible)[Content]];
    };
    const auto MenuBar = [this, &Fit](TSharedRef<SWidget> Actions, bool bUpper)
    {
        const float Height = bUpper ? 108.0f : 96.0f;
        const auto Scale = [this]
        {
            const FVector2D Size = GetCachedGeometry().GetLocalSize();
            return FMath::Min(Size.X / 1920.0, Size.Y / 1080.0);
        };
        return SNew(SBox).HeightOverride_Lambda([Scale, Height] { return FOptionalSize(Scale() * Height); })
            [SNew(SOverlay)
                + SOverlay::Slot()[SNew(SImage).Image(&BarBase).Visibility(EVisibility::HitTestInvisible)]
                + SOverlay::Slot()[SNew(SImage).Image(&BarGrain).Visibility(EVisibility::HitTestInvisible)
                    .ColorAndOpacity(FLinearColor(0.29f, 0.26f, 0.23f, 0.8f))]
                + SOverlay::Slot().VAlign(bUpper ? VAlign_Bottom : VAlign_Top)
                    [SNew(SBox).HeightOverride_Lambda([Scale] { return FOptionalSize(Scale() * 9.0); })
                        [SNew(SImage).Image(MenuTrim(bUpper)).Visibility(EVisibility::HitTestInvisible)
                            .ColorAndOpacity(FLinearColor(0.87f, 0.81f, 0.71f, 0.9f))]]
                + SOverlay::Slot()[Fit(Actions, Height)]];
    };
    Place(510, 30, 300, 74, Tab(INVTEXT("INVENTORY"), EBBBItemPanelPage::Inventory));
    Place(815, 30, 290, 74, Tab(INVTEXT("CHARACTER"), EBBBItemPanelPage::Character));
    Place(1150, 33, 290, 66, SNew(STextBlock).Text(INVTEXT("EXTENSIBLE")).Font(ArtFont(32))
        .ColorAndOpacity(FLinearColor(0.6f, 0.58f, 0.53f, 0.48f)));
    Place(676, 155, 568, 48, SNew(SBox).HAlign(HAlign_Center).VAlign(VAlign_Center)
        [
            SNew(SHorizontalBox)
            + SHorizontalBox::Slot().AutoWidth().VAlign(VAlign_Center).Padding(0, 0, 11, 0)
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
    Place(1324, 184, 490, 68, SNew(STextBlock).Font(ArtFont(44)).Text_Lambda([this]
        { return PanelPage == EBBBItemPanelPage::Character ? INVTEXT("GEAR") : INVTEXT("LOADOUT"); }));
    Hairline(1324, 254, 490);
    Place(1328, 275, 476, 40, SNew(STextBlock).Font(ArtFont(20)).ColorAndOpacity(MutedText)
        .Text_Lambda([this] { return PanelPage == EBBBItemPanelPage::Character ? INVTEXT("EQUIPPED") : INVTEXT("WEAPONS"); }));
    Place(1320, 330, 494, 664, SAssignNew(EquipmentSlots, SVerticalBox).Tag(TEXT("Backpack.PanelSlots")));
    Place(102, 1004, 160, 54, SNew(SButton).ButtonStyle(BBBItemUI::Navigation()).HAlign(HAlign_Left)
        .OnClicked_Lambda([this] { GetItemController()->ToggleBackpack(); return FReply::Handled(); })
        [Label(INVTEXT("B    CLOSE"), 22)]);
    Place(340, 1011, 780, 36, SAssignNew(StatusText, STextBlock).Font(Font(17)).ColorAndOpacity(MutedText));
    auto Chrome = SNew(SOverlay).Visibility(EVisibility::SelfHitTestInvisible)
        + SOverlay::Slot()[Fit(Canvas, 1080)]
        + SOverlay::Slot().VAlign(VAlign_Top)[MenuBar(TopActions, true)]
        + SOverlay::Slot().VAlign(VAlign_Bottom)[MenuBar(BottomActions, false)];
    MenuPanels = {Chrome, LeftWing, RightWing};
    static const FSlateColorBrush Matte(FLinearColor::Black);
    return SNew(SOverlay)
        + SOverlay::Slot()[SNew(SImage).Image(&Matte).Visibility(EVisibility::HitTestInvisible)]
        + SOverlay::Slot()[Fit(SNew(SBBBInventoryRoomImage)
            .Room_Lambda([this] { return CharacterPreview.Get(); })
            .Image_Lambda([this] { return GetCharacterBrush(); }), 1080)]
        + SOverlay::Slot()[Chrome];
}
