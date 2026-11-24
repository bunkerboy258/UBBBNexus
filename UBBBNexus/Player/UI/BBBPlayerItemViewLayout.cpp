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
#include "Engine/TextureRenderTarget2D.h"
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
#include "Brushes/SlateRoundedBoxBrush.h"
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
                const auto* RenderTarget = Panel->GetRenderTarget();
                const auto Point = Project(Geometry, *Panel, DesktopPosition);
                if (RenderTarget && Point.IsSet())
                {
                    if (Point->X >= 0 && Point->Y >= 0 && Point->X < RenderTarget->SizeX && Point->Y < RenderTarget->SizeY)
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
                const auto* RenderTarget = Panel->GetRenderTarget();
                const FVector2D DrawSize = Panel->GetCurrentDrawSize();
                const auto Window = Panel->GetSlateWindow();
                if (Window && RenderTarget && DrawSize.X > 0)
                {
                    const float PaintScale = RenderTarget->SizeX / DrawSize.X;
                    Children.AddWidget(FArrangedWidget(Window.ToSharedRef(),
                        FGeometry::MakeRoot(DrawSize, FSlateLayoutTransform(PaintScale))));
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
        const auto* Camera = Preview ? Preview->GetCamera() : nullptr;
        const FVector2D Size = Geometry.GetLocalSize();
        if (!Camera || Size.X <= 0 || Size.Y <= 0)
        {
            return {};
        }
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
        if (Distance <= 0.0)
        {
            return {};
        }
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
    if (CharacterPreview)
    {
        CharacterPreview->Close();
        CharacterPreview = nullptr;
        CharacterBrush.SetResourceObject(nullptr);
    }
    StorageSlotViews.Reset();
    PanelSlotViews.Reset();
    BuiltStorageStart = INDEX_NONE;
    BuiltPanelStart = INDEX_NONE;
    // 根节点不拦截命中 关闭背包后点击才能回到游戏视口并恢复鼠标捕获
    TSharedRef<SWidget> Result =
        SNew(SOverlay).Visibility(EVisibility::SelfHitTestInvisible)
        + SOverlay::Slot()[MakeGameplayHud()]
        + SOverlay::Slot()[SNew(SOverlay).Visibility_Lambda(
                               [this]
                               {
                                   return bBackpackOpen ? EVisibility::Visible : EVisibility::Collapsed;
                               })
                           + SOverlay::Slot()[MakeBackpack()]]
        + SOverlay::Slot()[MakeExplorationHud()];
    RefreshItems();
    return Result;
}

TSharedRef<SWidget> UBBBPlayerItemView::MakeBackpack()
{
    using namespace BBBItemUI;
    // 顶层铬层只承接真实控件命中 空白处放行给整备间空间面板
    const auto Canvas = SNew(SConstraintCanvas).Visibility(EVisibility::SelfHitTestInvisible);
    const auto LeftWing = SNew(SConstraintCanvas).Visibility(EVisibility::SelfHitTestInvisible);
    const auto RightWing = SNew(SConstraintCanvas).Visibility(EVisibility::SelfHitTestInvisible);
    const auto Place = [](SConstraintCanvas& Panel, float X, float Y, float W, float H, TSharedRef<SWidget> Widget)
    {
        Panel.AddSlot().Alignment(FVector2D::ZeroVector).Offset(FMargin(X, Y, W, H))[Widget];
    };
    const auto Label = [](const FText &Text, int32 Size)
    {
        return SNew(STextBlock).Text(Text).Font(ArtFont(Size)).ColorAndOpacity(FLinearColor::White)
            .ShadowOffset(FVector2D(1, 2)).ShadowColorAndOpacity(FLinearColor(0, 0, 0, 0.8f));
    };
    static const FSlateRoundedBoxBrush TabFrame(FLinearColor(0.035f, 0.03f, 0.024f, 0.32f), 1.0f,
        FLinearColor(0.72f, 0.69f, 0.64f, 0.83f), 2.0f);
    static const FSlateRoundedBoxBrush ActiveTab(FLinearColor(0.41f, 0.18f, 0.012f, 0.94f), 1.0f,
        FLinearColor(0.90f, 0.48f, 0.05f, 0.95f), 2.0f);
    const auto Tab = [this](const FText& Text, TOptional<EBBBItemPanelPage> Page)
    {
        const auto Face = SNew(SBorder).Visibility(EVisibility::HitTestInvisible).Padding(FMargin(0.0f, 3.0f, 0.0f, 0.0f))
            .HAlign(HAlign_Center).VAlign(VAlign_Center)
            .BorderImage_Lambda([this, Page]
            {
                return Page.IsSet() && PanelPage == Page.GetValue() ? &ActiveTab : &TabFrame;
            })
            [SNew(STextBlock).Text(Text).Font(ArtFont(26))
                .ColorAndOpacity(Page.IsSet() ? FLinearColor::White : MutedText)];
        if (!Page.IsSet())
        {
            return TSharedRef<SWidget>(Face);
        }

        const EBBBItemPanelPage Target = Page.GetValue();
        return TSharedRef<SWidget>(SNew(SButton)
            .Tag(FName(*FString::Printf(TEXT("Backpack.Panel.%d"), static_cast<int32>(Target))))
            .ButtonStyle(BBBItemUI::Navigation()).ContentPadding(0.0f)
            .HAlign(HAlign_Fill).VAlign(VAlign_Fill)
            .OnClicked_Lambda([this, Target] { return ShowPanel(Target); })
            [Face]);
    };
    const auto Hairline = [&Place](SConstraintCanvas& Panel, float X, float Y, float W)
    {
        static const FSlateRoundedBoxBrush Divider(FLinearColor(0.72f, 0.37f, 0.05f, 0.88f), 1.0f);
        const auto Line = SNew(SImage).Image(&Divider).Visibility(EVisibility::HitTestInvisible);
        Line->SetPixelSnapping(EWidgetPixelSnapping::Disabled);
        Place(Panel, X, Y, W, 3.0f, Line);
    };
    static const FSlateRoundedBoxBrush Keycap(FLinearColor::Transparent, 1.0f,
        FLinearColor(0.84f, 0.81f, 0.76f, 0.74f), 1.5f);
    const auto Fit = [](TSharedRef<SWidget> Content, float DesignHeight)
    {
        return SNew(SScaleBox).Stretch(EStretch::ScaleToFit).Visibility(EVisibility::SelfHitTestInvisible)
            [SNew(SBox).WidthOverride(1920).HeightOverride(DesignHeight).Visibility(EVisibility::SelfHitTestInvisible)[Content]];
    };
    const auto NavigationRow = SNew(SBox).HAlign(HAlign_Center).VAlign(VAlign_Center)
        .Visibility(EVisibility::SelfHitTestInvisible)
        [SNew(SHorizontalBox)
            + SHorizontalBox::Slot().AutoWidth().Padding(0, 0, 16, 0)
                [SNew(SBox).WidthOverride(246).HeightOverride(40)[Tab(INVTEXT("INVENTORY"), EBBBItemPanelPage::Inventory)]]
            + SHorizontalBox::Slot().AutoWidth().Padding(0, 0, 16, 0)
                [SNew(SBox).WidthOverride(246).HeightOverride(40)[Tab(INVTEXT("CHARACTER"), EBBBItemPanelPage::Character)]]
            + SHorizontalBox::Slot().AutoWidth()
                [SNew(SBox).WidthOverride(246).HeightOverride(40)[Tab(INVTEXT("EXTENSIBLE"), {})]]];
    constexpr float VitalsTop = 70.0f;
    Place(*Canvas, 676, VitalsTop, 568, 46, SNew(SBox).HAlign(HAlign_Center).VAlign(VAlign_Center)
        .Visibility(EVisibility::HitTestInvisible)
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
    Place(*Canvas, 676, VitalsTop + 45.0f, 568, 25, SNew(SBox).HAlign(HAlign_Center).VAlign(VAlign_Center)
        .Visibility(EVisibility::HitTestInvisible)
        [SNew(STextBlock)
            .Font(ArtFont(19))
            .ColorAndOpacity(FLinearColor(0.86f, 0.78f, 0.63f))
            .ShadowOffset(FVector2D(1, 1))
            .ShadowColorAndOpacity(FLinearColor(0, 0, 0, 0.9f))
            .Text_Lambda([this, LastCurrent = int32{INDEX_NONE}, LastMaximum = int32{INDEX_NONE}, Text = FText{}]() mutable
            {
                const auto* Controller = GetItemController();
                const auto* Player = Controller ? Cast<ABBBCharacter>(Controller->GetPawn()) : nullptr;
                if (!Player)
                {
                    return FText::GetEmpty();
                }
                const int32 Current = FMath::RoundToInt(Player->GetStamina());
                const int32 Maximum = FMath::RoundToInt(Player->GetMaximumStamina());
                if (Current != LastCurrent || Maximum != LastMaximum)
                {
                    LastCurrent = Current;
                    LastMaximum = Maximum;
                    Text = FText::Format(INVTEXT("STAMINA   {0}  /  {1}"), FText::AsNumber(Current), FText::AsNumber(Maximum));
                }
                return Text;
            })]);
    Place(*LeftWing, 15, 10, 475, 68, Label(INVTEXT("BACKPACK"), 44));
    Hairline(*LeftWing, 15, 80, 480);
    const auto Filter = [this](const FText &Text, EBBBItemType Type)
    {
        return SNew(SButton).Tag(FName(*FString::Printf(TEXT("Backpack.Storage.%d"), static_cast<int>(Type))))
            .ButtonStyle(BBBItemUI::Navigation()).OnClicked_Lambda([this, Type] { return ShowStorage(Type); })
            [SNew(STextBlock).Text(Text).Font(ArtFont(18)).ColorAndOpacity_Lambda([this, Type]
            { return Type == StoragePage ? FLinearColor::White : FLinearColor(0.69f, 0.65f, 0.57f); })];
    };
    Place(*LeftWing, 18, 101, 476, 42, SNew(SHorizontalBox).Visibility_Lambda([this]
        { return PanelPage == EBBBItemPanelPage::Inventory ? EVisibility::Visible : EVisibility::Collapsed; })
        + SHorizontalBox::Slot().AutoWidth().Padding(0, 0, 23, 0)[Filter(INVTEXT("WEAPONS"), EBBBItemType::Equipment)]
        + SHorizontalBox::Slot().AutoWidth().Padding(0, 0, 23, 0)[Filter(INVTEXT("CONSUMABLES"), EBBBItemType::Consumable)]
        + SHorizontalBox::Slot().AutoWidth()[Filter(INVTEXT("MISC"), EBBBItemType::Misc)]);
    Place(*LeftWing, 18, 101, 470, 42, SNew(STextBlock).Text(INVTEXT("WEARABLES")).Font(ArtFont(18))
        .Visibility_Lambda([this] { return PanelPage == EBBBItemPanelPage::Character ? EVisibility::HitTestInvisible : EVisibility::Hidden; }));
    static const FScrollBoxStyle StorageScroll = FScrollBoxStyle()
        .SetTopShadowBrush(FSlateNoResource()).SetBottomShadowBrush(FSlateNoResource());
    Place(*LeftWing, 11, 156, 491, InventoryGridHeight, SNew(SScrollBox).Style(&StorageScroll).ScrollBarVisibility(EVisibility::Collapsed)
        + SScrollBox::Slot()[SAssignNew(BackpackSlots, SVerticalBox)]);
    Place(*RightWing, 18, 10, 490, 68, SNew(SBox).HAlign(HAlign_Right)
        [SNew(STextBlock).Font(ArtFont(44)).Text_Lambda([this]
            { return PanelPage == EBBBItemPanelPage::Character ? INVTEXT("GEAR") : INVTEXT("LOADOUT"); })]);
    Hairline(*RightWing, 18, 80, 490);
    Place(*RightWing, 18, 101, 490, 40, SNew(SBox).HAlign(HAlign_Right)
        [SNew(STextBlock).Font(ArtFont(20)).ColorAndOpacity(MutedText)
            .Text_Lambda([this] { return PanelPage == EBBBItemPanelPage::Character ? INVTEXT("EQUIPPED") : INVTEXT("WEAPONS"); })]);
    Place(*RightWing, 14, 156, 494, InventoryGridHeight, SAssignNew(EquipmentSlots, SVerticalBox).Tag(TEXT("Backpack.PanelSlots")));
    Place(*Canvas, 96, 1030, 160, 42, SNew(SButton).ButtonStyle(BBBItemUI::Navigation()).HAlign(HAlign_Left)
        .OnClicked_Lambda([this] { GetItemController()->ToggleBackpack(); return FReply::Handled(); })
        [SNew(SHorizontalBox)
            + SHorizontalBox::Slot().AutoWidth().VAlign(VAlign_Center)
                [SNew(SBox).WidthOverride(29).HeightOverride(29)
                    [SNew(SOverlay) + SOverlay::Slot()[SNew(SImage).Image(&Keycap)]
                        + SOverlay::Slot().HAlign(HAlign_Center).VAlign(VAlign_Center)[Label(INVTEXT("B"), 16)]]]
            + SHorizontalBox::Slot().AutoWidth().VAlign(VAlign_Center).Padding(10, 0)[Label(INVTEXT("CLOSE"), 18)]]);
    Place(*Canvas, 345, 1037, 700, 32, SAssignNew(StatusText, STextBlock).Font(Font(17)).ColorAndOpacity(MutedText)
        .Visibility(EVisibility::HitTestInvisible));
    BackpackChrome = Fit(Canvas, 1080);
    StoragePanel = LeftWing;
    LoadoutPanel = RightWing;
    NavigationPanel = NavigationRow;
    static const FSlateColorBrush Matte(FLinearColor::Black);
    const auto CaptureImage = SNew(SBBBInventoryRoomImage)
        .Room_Lambda([this] { return CharacterPreview.Get(); })
        .Image_Lambda([this] { return GetCharacterBrush(); });
    RoomImage = CaptureImage;
    return SNew(SOverlay)
        + SOverlay::Slot()[SNew(SImage).Image(&Matte).Visibility(EVisibility::HitTestInvisible)]
        + SOverlay::Slot()[Fit(CaptureImage, 1080)]
        + SOverlay::Slot()[BackpackChrome.ToSharedRef()];
}
