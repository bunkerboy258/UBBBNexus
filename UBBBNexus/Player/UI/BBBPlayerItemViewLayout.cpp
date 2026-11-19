#include "BBBWork/UBBBNexus/Player/UI/BBBPlayerItemView.h"
#include "BBBWork/UBBBNexus/Player/UI/BBBPlayerItemStyle.h"
#include "BBBWork/UBBBNexus/Player/UI/BBBPlayerItemText.h"
#include "BBBWork/UBBBNexus/Player/BBBPlayerController.h"
#include "BBBWork/UBBBNexus/Player/BBBPlayerItemDisplayData.h"
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Base/BBBConsumable.h"
#include "Widgets/Layout/SBackgroundBlur.h"
#include "Widgets/Layout/SScaleBox.h"
#include "Widgets/Layout/SScrollBox.h"
#include "Widgets/Layout/SScrollBar.h"
#include "Widgets/Layout/SConstraintCanvas.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Notifications/SProgressBar.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/SOverlay.h"
#include "Widgets/Text/STextBlock.h"
#include "Brushes/SlateColorBrush.h"
#include "Styling/CoreStyle.h"

namespace
{
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
                         SOverlay::Slot()[SNew(SBackgroundBlur).BlurStrength(16.0f)] +
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
    const auto MountWing = [this, &Canvas](TSharedRef<SConstraintCanvas> Wing, float X, float Facing)
    {
        Canvas->AddSlot().Alignment(FVector2D::ZeroVector).Offset(FMargin(X, 174, 520, 800))
            [SNew(SBox).RenderTransformPivot(FVector2D(0.5, 0.5))
                .RenderTransform_Lambda([this, Facing]() -> TOptional<FSlateRenderTransform>
                {
                    const float Lean = Facing * 0.060f;
                    return FSlateRenderTransform(FMatrix2x2(0.965f, Lean, -Facing * 0.014f, 1.0f),
                        FVector2f(MenuParallax.X * 8.0, MenuParallax.Y * 4.0));
                })[Wing]];
    };
    MountWing(LeftWing, 90, 1);
    MountWing(RightWing, 1306, -1);
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
    const auto PlaceTrim = [&Place](float Y, bool bUpper)
    {
        Place(60, Y, 1800, 22, SNew(SImage).Image(MenuTrim(bUpper)).Visibility(EVisibility::HitTestInvisible)
            .ColorAndOpacity(FLinearColor(0.87f, 0.81f, 0.71f, 0.9f)));
    };
    Place(510, 30, 300, 74, Tab(INVTEXT("INVENTORY"), EBBBItemPanelPage::Inventory));
    Place(815, 30, 290, 74, Tab(INVTEXT("CHARACTER"), EBBBItemPanelPage::Character));
    Place(1150, 33, 290, 66, SNew(STextBlock).Text(INVTEXT("EXTENSIBLE")).Font(ArtFont(32))
        .ColorAndOpacity(FLinearColor(0.6f, 0.58f, 0.53f, 0.48f)));
    PlaceTrim(108, true);
    Place(676, 187, 568, 830, SNew(SScaleBox).Stretch(EStretch::ScaleToFit)
        [SNew(SImage).Image_Lambda([this] { return GetCharacterBrush(); })]);
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
    Place(101, 330, 491, 612, SNew(SScrollBox).ScrollBarVisibility(EVisibility::Collapsed)
        + SScrollBox::Slot()[SAssignNew(BackpackSlots, SVerticalBox)]);
    Place(1324, 184, 490, 68, SNew(STextBlock).Font(ArtFont(44)).Text_Lambda([this]
        { return PanelPage == EBBBItemPanelPage::Character ? INVTEXT("GEAR") : INVTEXT("LOADOUT"); }));
    Hairline(1324, 254, 490);
    Place(1328, 275, 476, 40, SNew(STextBlock).Font(ArtFont(20)).ColorAndOpacity(MutedText)
        .Text_Lambda([this] { return PanelPage == EBBBItemPanelPage::Character ? INVTEXT("EQUIPPED") : INVTEXT("WEAPONS"); }));
    Place(1320, 330, 494, 664, SAssignNew(EquipmentSlots, SVerticalBox).Tag(TEXT("Backpack.PanelSlots")));
    PlaceTrim(984, false);
    Place(102, 1004, 160, 54, SNew(SButton).ButtonStyle(BBBItemUI::Navigation()).HAlign(HAlign_Left)
        .OnClicked_Lambda([this] { GetItemController()->ToggleBackpack(); return FReply::Handled(); })
        [Label(INVTEXT("B    CLOSE"), 22)]);
    Place(340, 1011, 780, 36, SAssignNew(StatusText, STextBlock).Font(Font(17)).ColorAndOpacity(MutedText));
    return SNew(SOverlay)
        + SOverlay::Slot()[SNew(SImage).Image(Brush(TEXT("/Game/_Project/UI/Bag/T_InventoryForest.T_InventoryForest")))]
        + SOverlay::Slot()[SNew(SScaleBox).Stretch(EStretch::ScaleToFit)
            [SNew(SBox).WidthOverride(1920).HeightOverride(1080)[Canvas]]];
}
