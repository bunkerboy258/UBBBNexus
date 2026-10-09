#include "BBBWork/UBBBNexus/Player/UI/BBBPlayerItemView.h"
#include "BBBWork/UBBBNexus/Player/UI/BBBPlayerItemStyle.h"
#include "BBBWork/UBBBNexus/Player/BBBPlayerController.h"
#include "BBBWork/UBBBNexus/Player/BBBPlayerItemDisplayData.h"
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
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
        SOverlay::Slot()[SNew(SOverlay).Visibility_Lambda(
                             [this]()
                             {
                                 return bBackpackOpen ? EVisibility::Visible : EVisibility::Collapsed;
                             }) +
                         SOverlay::Slot()[SNew(SBackgroundBlur).BlurStrength(16.0f)] + SOverlay::Slot()[MakeBackpack()]];
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
          SNew(STextBlock).Text(FText::FromString(TEXT("Bag"))).Font(TitleFont(76))
              .ColorAndOpacity(FLinearColor(1.0f, 1.0f, 1.0f, 0.90f)));
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
    const auto TabButton = [this](const TCHAR *Text, bool bMisc)
    {
        return SNew(SButton).ButtonStyle(BBBItemUI::Navigation()).OnClicked_Lambda(
            [this, bMisc]()
            {
                return ShowStorage(bMisc);
            })[SNew(SOverlay) + SOverlay::Slot().VAlign(VAlign_Top)[SNew(STextBlock)
                   .Text(FText::FromString(Text))
                   .Font(Font(22))
                   .ColorAndOpacity_Lambda(
                       [this, bMisc]()
                       {
                           return bMiscPage == bMisc ? FSlateColor(FLinearColor::White) : FSlateColor(MutedText);
                       })] + SOverlay::Slot().VAlign(VAlign_Bottom).HAlign(HAlign_Left).Padding(16.0f, 0.0f, 0.0f, 0.0f)
                [SNew(SBox).WidthOverride(11.0f).HeightOverride(10.0f)
                    [SNew(SImage).Image(BBBItemUI::Brush(TEXT("/Game/_ThirdParty/UI/EditableSurvivalHorrorUI/Weapon_Customization/T_Marker.T_Marker")))
                        .Visibility_Lambda([this, bMisc]()
                        {
                            return bMiscPage == bMisc ? EVisibility::HitTestInvisible : EVisibility::Hidden;
                        })]]];
    };
    Place(60.0f, 550.0f, 156.0f, 40.0f, TabButton(TEXT("EQUIPMENT"), false));
    Place(240.0f, 550.0f, 100.0f, 40.0f, TabButton(TEXT("MISC"), true));
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
    Place(1000.0f, 160.0f, 350.0f, 710.0f, SAssignNew(EquipmentSlots, SVerticalBox));
    const auto MakeNavigation = [this](const TCHAR *Text, bool bGear)
    {
        return SNew(SButton).ButtonStyle(BBBItemUI::Navigation()).OnClicked_Lambda(
            [this, bGear]()
            {
                return ShowGear(bGear);
            })[SNew(SOverlay) + SOverlay::Slot().VAlign(VAlign_Top)[SNew(STextBlock)
                   .Text(FText::FromString(Text))
                   .Font(Font(23))
                   .ColorAndOpacity_Lambda(
                       [this, bGear]()
                       {
                           return bGearPage == bGear ? FSlateColor(FLinearColor::White) : FSlateColor(MutedText);
                       })] + SOverlay::Slot().VAlign(VAlign_Bottom).HAlign(HAlign_Left).Padding(18.0f, 0.0f, 0.0f, 0.0f)
                [SNew(SBox).WidthOverride(11.0f).HeightOverride(10.0f)
                    [SNew(SImage).Image(BBBItemUI::Brush(TEXT("/Game/_ThirdParty/UI/EditableSurvivalHorrorUI/Weapon_Customization/T_Marker.T_Marker")))
                        .Visibility_Lambda([this, bGear]()
                        {
                            return bGearPage == bGear ? EVisibility::HitTestInvisible : EVisibility::Hidden;
                        })]]];
    };
    Place(1485.0f, 41.0f, 175.0f, 50.0f, MakeNavigation(TEXT("QUICKBAR"), false));
    Place(1700.0f, 41.0f, 120.0f, 50.0f, MakeNavigation(TEXT("GEAR"), true));
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
                      return ItemCharacter ? FText::FromString(FString::Printf(
                                                 TEXT("HP  %.0f / %.0f"), ItemCharacter->GetHealth(),
                                                 ItemCharacter->GetLifePhase() == EBBBCharacterLifePhase::Downed
                                                     ? ItemCharacter->GetCharacterConfig().DownedHealth
                                                     : ItemCharacter->GetCharacterConfig().MaximumHealth))
                                           : FText::GetEmpty();
                  })
              .Font(Font(16))
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
    Place(60.0f, 1033.0f, 820.0f, 24.0f, SAssignNew(StatusText, STextBlock).Font(Font(14)).ColorAndOpacity(MutedText));
    Place(1510.0f, 931.0f, 280.0f, 82.0f,
          SNew(SButton).ButtonStyle(ExitButton()).HAlign(HAlign_Center).VAlign(VAlign_Center).OnClicked_Lambda(
              [this]()
              {
                  GetItemController()->ToggleBackpack();
                  return FReply::Handled();
              })[SNew(STextBlock).Text(FText::FromString(TEXT("EXIT"))).Font(ArtFont(31)).ColorAndOpacity(FLinearColor::White)]);
    return SNew(SOverlay) +
        SOverlay::Slot()[SNew(SBorder).BorderImage(&Dim).Padding(0.0f).Visibility(EVisibility::HitTestInvisible)] +
        SOverlay::Slot()[SNew(SScaleBox).Stretch(EStretch::ScaleToFill).Clipping(EWidgetClipping::ClipToBounds)
            .Visibility(EVisibility::HitTestInvisible)
                [SNew(SImage).Image(Brush(TEXT("/Game/_Project/UI/Bag/E01/T_E01_Backdrop.T_E01_Backdrop")))
                    .ColorAndOpacity(FLinearColor(1.0f, 1.0f, 1.0f, 0.87f))]] +
        SOverlay::Slot()[SNew(SScaleBox).Stretch(EStretch::ScaleToFit)
            [SNew(SBox).WidthOverride(1920.0f).HeightOverride(1080.0f)[Canvas]]];
}
