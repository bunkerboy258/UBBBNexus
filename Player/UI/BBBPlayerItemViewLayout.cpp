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
                         SOverlay::Slot()[SNew(SBackgroundBlur).BlurStrength(8.0f)] + SOverlay::Slot()[MakeBackpack()]];
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
    static const FSlateColorBrush Dim(FLinearColor(0.006f, 0.008f, 0.010f, 0.77f));
    Place(0.0f, 0.0f, 1920.0f, 1080.0f, SNew(SBorder).BorderImage(&Dim));
    Place(80.0f, 38.0f, 360.0f, 76.0f,
          SNew(STextBlock).Text(FText::FromString(TEXT("Bag"))).Font(ArtFont(60)).ColorAndOpacity(FLinearColor::White));
    Place(80.0f, 145.0f, 830.0f, 55.0f,
          SNew(STextBlock)
              .Text_Lambda(
                  [this]()
                  {
                      auto *Controller = GetItemController();
                      return Controller ? Controller->GetItemDisplayData(InspectedSlot).Name : FText::GetEmpty();
                  })
              .Font(Font(24))
              .ColorAndOpacity(FLinearColor::White));
    Place(95.0f, 215.0f, 795.0f, 270.0f,
          SAssignNew(DetailImage, SBox)[SNew(SScaleBox).Stretch(EStretch::ScaleToFit)[SNew(SImage).Image_Lambda(
              [this]()
              {
                  return GetDetailBrush();
              })]]);
    const auto TabButton = [this](const TCHAR *Text, bool bMisc)
    {
        return SNew(SButton).ButtonStyle(Button()).OnClicked_Lambda(
            [this, bMisc]()
            {
                return ShowStorage(bMisc);
            })[SNew(STextBlock)
                   .Text(FText::FromString(Text))
                   .Font(ArtFont(24))
                   .ColorAndOpacity_Lambda(
                       [this, bMisc]()
                       {
                           return bMiscPage == bMisc ? Accent : FLinearColor::White;
                       })];
    };
    Place(80.0f, 500.0f, 190.0f, 50.0f, TabButton(TEXT("EQUIPMENT"), false));
    Place(285.0f, 500.0f, 190.0f, 50.0f, TabButton(TEXT("MISC"), true));
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
        SNew(SScrollBar).Style(&Scroll).Orientation(Orient_Horizontal).AlwaysShowScrollbar(true);
    Place(80.0f, 1018.0f, 835.0f, 7.0f, Bar);
    Place(
        75.0f, 570.0f, 850.0f, 440.0f,
        SNew(SScrollBox).ExternalScrollbar(Bar).Orientation(Orient_Vertical).ScrollBarThickness(FVector2D(7.0f, 7.0f)) +
            SScrollBox::Slot()[SAssignNew(BackpackSlots, SVerticalBox)]);
    Place(1020.0f, 148.0f, 340.0f, 690.0f, SAssignNew(EquipmentSlots, SVerticalBox));
    const auto MakeNavigation = [this](const TCHAR *Text, bool bGear)
    {
        return SNew(SButton).ButtonStyle(Button()).OnClicked_Lambda(
            [this, bGear]()
            {
                return ShowGear(bGear);
            })[SNew(STextBlock)
                   .Text(FText::FromString(Text))
                   .Font(ArtFont(35))
                   .ColorAndOpacity_Lambda(
                       [this, bGear]()
                       {
                           return bGearPage == bGear ? Accent : FLinearColor::White;
                       })];
    };
    Place(1450.0f, 54.0f, 220.0f, 58.0f, MakeNavigation(TEXT("QUICKBAR"), false));
    Place(1682.0f, 54.0f, 155.0f, 58.0f, MakeNavigation(TEXT("GEAR"), true));
    Place(1420.0f, 145.0f, 435.0f, 735.0f,
          SNew(SScaleBox).Stretch(EStretch::ScaleToFit)[SNew(SImage).Image_Lambda(
              [this]()
              {
                  return GetCharacterBrush();
              })]);
    Place(1020.0f, 855.0f, 340.0f, 34.0f,
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
              .Font(Font(18))
              .ColorAndOpacity(FLinearColor::White));
    static const FProgressBarStyle Health = []()
    {
        FProgressBarStyle Result;
        Result.SetBackgroundImage(*Brush(TEXT(
            "/Game/_ThirdParty/UI/EditableSurvivalHorrorUI/Weapon_Customization/T_Scrollbar_Big.T_Scrollbar_Big")));
        Result.SetFillImage(*Brush(TEXT("/Game/_ThirdParty/UI/EditableSurvivalHorrorUI/Weapon_Customization/"
                                        "T_Scrollbar_Handler.T_Scrollbar_Handler")));
        return Result;
    }();
    Place(1020.0f, 900.0f, 340.0f, 9.0f,
          SNew(SProgressBar)
              .Style(&Health)
              .FillColorAndOpacity(FLinearColor::White)
              .Percent_Lambda(
                  [this]()
                  {
                      return GetItemController() ? GetItemController()->GetHudHealthFraction() : 0.0f;
                  }));
    Place(80.0f, 1032.0f, 820.0f, 28.0f, SAssignNew(StatusText, STextBlock).Font(Font(14)).ColorAndOpacity(Accent));
    Place(1490.0f, 956.0f, 330.0f, 68.0f,
          SNew(SButton).ButtonStyle(Button()).OnClicked_Lambda(
              [this]()
              {
                  GetItemController()->ToggleBackpack();
                  return FReply::Handled();
              })[SNew(STextBlock).Text(FText::FromString(TEXT("EXIT"))).Font(ArtFont(32)).ColorAndOpacity(Accent)]);
    return SNew(SScaleBox).Stretch(
        EStretch::ScaleToFit)[SNew(SBox).WidthOverride(1920.0f).HeightOverride(1080.0f)[Canvas]];
}
