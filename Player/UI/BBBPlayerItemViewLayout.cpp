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

TSharedRef<SWidget> UBBBPlayerItemView::RebuildWidget()
{
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
        SOverlay::Slot()
            .HAlign(HAlign_Center)
            .VAlign(VAlign_Bottom)
            .Padding(
                0.0f, 0.0f, 0.0f,
                24.0f)[SNew(STextBlock)
                           .Visibility_Lambda(
                               [this]()
                               {
                                   const ABBBPlayerController *Controller = GetItemController();
                                   const auto *Character =
                                       Controller ? Cast<ABBBCharacter>(Controller->GetPawn()) : nullptr;
                                   const bool bPreparing =
                                       Character &&
                                       Character->RuntimeData.Interaction.ReadInteractionUseState().bBusy &&
                                       Character->RuntimeData.Interaction.ReadInteractionUseState().Fact.Stage >= 1 &&
                                       Character->RuntimeData.Interaction.ReadInteractionUseState().Fact.Stage <= 3;
                                   return !bBackpackOpen && bPreparing ? EVisibility::HitTestInvisible
                                                                       : EVisibility::Collapsed;
                               })
                           .Text(FText::FromString(TEXT("按住左键 高抛  按住右键 低抛  松开投掷")))
                           .Font(BBBItemUI::Font(14))] +
        SOverlay::Slot()[SNew(SOverlay).Visibility_Lambda(
                             [this]()
                             {
                                 return bBackpackOpen ? EVisibility::Visible : EVisibility::Collapsed;
                             }) +
                         SOverlay::Slot()[SNew(SBackgroundBlur).BlurStrength(16.0f)] +
                         SOverlay::Slot()[MakeBackpack()]];
    RefreshItems();
    return Result;
}

TSharedRef<SWidget> UBBBPlayerItemView::MakeBackpack()
{
    using namespace BBBItemUI;
    TSharedRef<SConstraintCanvas> Canvas = SNew(SConstraintCanvas);
    const auto Place = [&Canvas](float X, float Y, float W, float H, TSharedRef<SWidget> Widget)
    {
        Canvas->AddSlot().Alignment(FVector2D::ZeroVector).Offset(FMargin(X, Y, W, H))[Widget];
    };
    static const FSlateColorBrush Dim(FLinearColor(0.002f, 0.003f, 0.003f, 0.82f));
    constexpr float StorageLeft = 58.0f;
    Place(StorageLeft, 20.0f, 480.0f, 160.0f,
          SNew(STextBlock).Text(BBBItemText::Backpack).Font(ChineseArtFont(64))
              .ColorAndOpacity(FLinearColor(1.0f, 1.0f, 1.0f, 0.96f)));
    Place(StorageLeft, 150.0f, 862.0f, 365.0f,
          SAssignNew(DetailImage, SBox)
              [SNew(SOverlay) +
               SOverlay::Slot()
                   [SNew(SScaleBox).Stretch(EStretch::ScaleToFit).VAlign(VAlign_Center)
                       [SNew(SImage).Image_Lambda(
                           [this]()
                           {
                               return GetDetailBrush();
                           })]] +
               SOverlay::Slot().HAlign(HAlign_Right).VAlign(VAlign_Bottom).Padding(0.0f, 0.0f, 16.0f, 14.0f)
                   [SNew(STextBlock)
                        .Text_Lambda(
                            [this]()
                            {
                                auto *Controller = GetItemController();
                                return Controller ? Controller->GetItemDisplayData(InspectedSlot).Name : FText::GetEmpty();
                            })
                        .Font(ArtFont(52))
                        .Justification(ETextJustify::Right)
                        .WrapTextAt(620.0f)
                        .ShadowColorAndOpacity(FLinearColor(0.0f, 0.0f, 0.0f, 0.85f))
                        .ShadowOffset(FVector2D(2.0f, 2.0f))
                        .ColorAndOpacity(FLinearColor(1.0f, 1.0f, 1.0f, 0.92f))]]);
    const auto TabButton = [this](const FText &Text, EBBBItemType Type)
    {
        return SNew(SButton).Tag(FName(*FString::Printf(TEXT("Backpack.Storage.%d"), static_cast<int32>(Type))))
            .ButtonStyle(BBBItemUI::Navigation()).HAlign(HAlign_Left).OnClicked_Lambda(
            [this, Type]()
            {
                return ShowStorage(Type);
            })[SNew(SOverlay) + SOverlay::Slot().VAlign(VAlign_Top)[SNew(STextBlock)
                   .Text(Text)
                   .Font(ChineseArtFont(18))
                   .ColorAndOpacity_Lambda(
                       [this, Type]()
                       {
                           return StoragePage == Type ? FSlateColor(FLinearColor::White) : FSlateColor(MutedText);
                       })] + SOverlay::Slot().VAlign(VAlign_Bottom).HAlign(HAlign_Center)
                [SNew(SBox).WidthOverride(11.0f).HeightOverride(10.0f)
                    [SNew(SImage).Image(BBBItemUI::Brush(TEXT("/Game/_ThirdParty/UI/EditableSurvivalHorrorUI/Weapon_Customization/T_Marker.T_Marker")))
                        .Visibility_Lambda([this, Type]()
                        {
                            return StoragePage == Type ? EVisibility::HitTestInvisible : EVisibility::Hidden;
                        })]]];
    };
    Place(60.0f, 558.0f, 160.0f, 46.0f, TabButton(BBBItemText::Weapons, EBBBItemType::Equipment));
    Place(280.0f, 558.0f, 160.0f, 46.0f, TabButton(BBBItemText::Wearables, EBBBItemType::Wearable));
    Place(500.0f, 558.0f, 160.0f, 46.0f, TabButton(BBBItemText::Consumables, EBBBItemType::Consumable));
    Place(720.0f, 558.0f, 160.0f, 46.0f, TabButton(BBBItemText::Misc, EBBBItemType::Misc));
    Place(StorageLeft, 608.0f, 862.0f, 398.0f,
          SNew(SImage).Image(Brush(TEXT("/Game/_Project/UI/Bag/E01/T_E01_StorageBoundary.T_E01_StorageBoundary")))
              .ColorAndOpacity(FLinearColor(1.0f, 1.0f, 1.0f, 0.38f)));
    static const FScrollBarStyle Scroll = []()
    {
        FScrollBarStyle Result;
        const auto *Track = Brush(
            TEXT("/Game/_ThirdParty/UI/EditableSurvivalHorrorUI/Weapon_Customization/T_Scrollbar_Big.T_Scrollbar_Big"));
        const auto *Handle = Brush(TEXT("/Game/_ThirdParty/UI/EditableSurvivalHorrorUI/Weapon_Customization/"
                                        "T_Scrollbar_Handler.T_Scrollbar_Handler"));
        Result.SetHorizontalBackgroundImage(*Track).SetVerticalBackgroundImage(*Track);
        Result.SetNormalThumbImage(*Handle).SetHoveredThumbImage(*Handle).SetDraggedThumbImage(*Handle);
        return Result;
    }();
    TSharedRef<SScrollBar> Bar =
        SNew(SScrollBar).Style(&Scroll).Orientation(Orient_Horizontal).AlwaysShowScrollbar(true)
            .Padding(FMargin(0.0f)).Thickness(FVector2D(3.0f));
    Place(65.0f, 1018.0f, 845.0f, 3.0f, Bar);
    Place(
        68.0f, 620.0f, 844.0f, 373.0f,
        SNew(SScrollBox).ExternalScrollbar(Bar).Orientation(Orient_Vertical).ScrollBarThickness(FVector2D(7.0f, 7.0f)) +
            SScrollBox::Slot()[SAssignNew(BackpackSlots, SVerticalBox)]);
    Place(1000.0f, 160.0f, 350.0f, 710.0f, SAssignNew(EquipmentSlots, SVerticalBox).Tag(TEXT("Backpack.PanelSlots")));
    const auto MakeNavigation = [this](const FText &Text, EBBBItemPanelPage Page)
    {
        return SNew(SButton).Tag(FName(*FString::Printf(TEXT("Backpack.Panel.%d"), static_cast<int32>(Page))))
            .ButtonStyle(BBBItemUI::Navigation()).HAlign(HAlign_Left).OnClicked_Lambda(
            [this, Page]()
            {
                return ShowPanel(Page);
            })[SNew(SOverlay) + SOverlay::Slot().VAlign(VAlign_Top)[SNew(STextBlock)
                   .Text(Text)
                   .Font(ChineseArtFont(18))
                   .ColorAndOpacity_Lambda(
                       [this, Page]()
                       {
                           return PanelPage == Page ? FSlateColor(FLinearColor::White) : FSlateColor(MutedText);
                       })] + SOverlay::Slot().VAlign(VAlign_Bottom).HAlign(HAlign_Center)
                [SNew(SBox).WidthOverride(11.0f).HeightOverride(10.0f)
                    [SNew(SImage).Image(BBBItemUI::Brush(TEXT("/Game/_ThirdParty/UI/EditableSurvivalHorrorUI/Weapon_Customization/T_Marker.T_Marker")))
                        .Visibility_Lambda([this, Page]()
                        {
                            return PanelPage == Page ? EVisibility::HitTestInvisible : EVisibility::Hidden;
                        })]]];
    };
    Place(1000.0f, 41.0f, 180.0f, 46.0f, MakeNavigation(BBBItemText::Weapons, EBBBItemPanelPage::Weapons));
    Place(1280.0f, 41.0f, 180.0f, 46.0f, MakeNavigation(BBBItemText::Character, EBBBItemPanelPage::Character));
    Place(1560.0f, 41.0f, 180.0f, 46.0f, MakeNavigation(BBBItemText::Consumables, EBBBItemPanelPage::Consumables));
    Place(1420.0f, 115.0f, 435.0f, 765.0f,
          SNew(SScaleBox).Stretch(EStretch::ScaleToFit)[SNew(SImage).Image_Lambda(
              [this]()
              {
                  return GetCharacterBrush();
              })]);
    Place(1007.0f, 896.0f, 340.0f, 30.0f,
          SNew(STextBlock)
              .Text_Lambda(
                  [this]()
                  {
                      const auto *Controller = GetItemController();
                      const auto *ItemCharacter = Controller ? Cast<ABBBCharacter>(Controller->GetPawn()) : nullptr;
                      return ItemCharacter ? FText::Format(BBBItemText::Health,
                                                 FText::AsNumber(FMath::RoundToInt(ItemCharacter->GetHealth())),
                                                 FText::AsNumber(FMath::RoundToInt(
                                                     ItemCharacter->GetLifePhase() == EBBBCharacterLifePhase::Downed
                                                         ? ItemCharacter->GetCharacterConfig().DownedHealth
                                                         : ItemCharacter->GetCharacterConfig().MaximumHealth)))
                                           : FText::GetEmpty();
                  })
              .Font(ChineseArtFont(16))
              .ColorAndOpacity(FLinearColor::White));
    static const FProgressBarStyle Health = []()
    {
        FProgressBarStyle Result;
        Result.SetBackgroundImage(*Brush(TEXT(
            "/Game/_ThirdParty/UI/EditableSurvivalHorrorUI/Weapon_Customization/T_Scrollbar_Big.T_Scrollbar_Big")));
        Result.SetFillImage(*Brush(TEXT("/Game/_ThirdParty/UI/EditableSurvivalHorrorUI/Weapon_Customization/"
                                        "T_Scrollbar_Handler.T_Scrollbar_Handler")));
        Result.EnableFillAnimation = false;
        return Result;
    }();
    Place(1007.0f, 938.0f, 340.0f, 3.0f,
          SNew(SProgressBar)
              .Style(&Health)
              .FillColorAndOpacity(FLinearColor::White)
              .Percent_Lambda(
                  [this]()
                  {
                      return GetItemController() ? GetItemController()->GetHudHealthFraction() : 0.0f;
                  }));
    Place(60.0f, 1033.0f, 820.0f, 24.0f, SAssignNew(StatusText, STextBlock).Font(ChineseArtFont(14)).ColorAndOpacity(MutedText));
    Place(1510.0f, 931.0f, 280.0f, 82.0f,
          SNew(SButton).ButtonStyle(ExitButton()).HAlign(HAlign_Center).VAlign(VAlign_Center).OnClicked_Lambda(
              [this]()
              {
                  GetItemController()->ToggleBackpack();
                  return FReply::Handled();
              })[SNew(STextBlock).Text(BBBItemText::Return).Font(ChineseArtFont(28)).ColorAndOpacity(FLinearColor::White)]);
    return SNew(SOverlay) +
        SOverlay::Slot()[SNew(SBorder).BorderImage(&Dim).Padding(0.0f).Visibility(EVisibility::HitTestInvisible)] +
        SOverlay::Slot()[SNew(SScaleBox).Stretch(EStretch::ScaleToFill).Clipping(EWidgetClipping::ClipToBounds)
            .Visibility(EVisibility::HitTestInvisible)
                [SNew(SImage).Image(Brush(TEXT("/Game/_Project/UI/Bag/E01/T_E01_Backdrop.T_E01_Backdrop")))
                    .ColorAndOpacity(FLinearColor(1.0f, 1.0f, 1.0f, 0.87f))]] +
        SOverlay::Slot()[SNew(SScaleBox).Stretch(EStretch::ScaleToFit)
            [SNew(SBox).WidthOverride(1920.0f).HeightOverride(1080.0f)[Canvas]]];
}
