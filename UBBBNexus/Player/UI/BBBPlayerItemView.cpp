#include "BBBWork/UBBBNexus/Player/UI/BBBPlayerItemView.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "BBBWork/UBBBNexus/Player/UI/SBBBPlayerItemSlot.h"
#include "BBBWork/UBBBNexus/Player/UI/BBBPlayerItemStyle.h"
#include "BBBWork/UBBBNexus/Player/UI/BBBPlayerItemText.h"
#include "BBBWork/UBBBNexus/Player/UI/BBBPlayerItemPortrait.h"
#include "BBBWork/UBBBNexus/Player/BBBPlayerController.h"
#include "BBBWork/UBBBNexus/Player/BBBPlayerItemDisplayData.h"
#include "Engine/FontFace.h"
#include "Engine/Texture2D.h"
#include "Sound/SoundBase.h"
#include "Engine/TextureRenderTarget2D.h"
#include "UObject/ConstructorHelpers.h"
#include "Widgets/Layout/SUniformGridPanel.h"
#include "Framework/Application/SlateApplication.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Text/STextBlock.h"

UBBBPlayerItemView::UBBBPlayerItemView(const FObjectInitializer &ObjectInitializer) : Super(ObjectInitializer)
{
    SetIsFocusable(true);
    static ConstructorHelpers::FObjectFinder<UFontFace> Font(
        TEXT("/Game/_Project/Customization/UI/Fonts/NotoSansCJKsc_Regular.NotoSansCJKsc_Regular"));
    InterfaceFont = Font.Object;
    static ConstructorHelpers::FObjectFinder<UFontFace> ArtFont(
        TEXT("/Game/_ThirdParty/Fonts/NotoSansCJKsc/NotoSansCJKsc_Bold.NotoSansCJKsc_Bold"));
    static ConstructorHelpers::FObjectFinder<UMaterialInterface> ArtMaterial(
        TEXT("/Game/_Project/UI/Bag/Materials/M_BagChineseArt.M_BagChineseArt"));
    InterfaceArtFont = ArtFont.Object;
    InterfaceArtMaterial = ArtMaterial.Object;
    static ConstructorHelpers::FObjectFinder<UTexture2D> Avatar(
        TEXT("/Game/_Project/UI/Gameplay/T_MaodieAvatar.T_MaodieAvatar"));
    static ConstructorHelpers::FObjectFinder<USoundBase> CompletionSound(
        TEXT("/Game/_Project/UI/Gameplay/Evac.Evac"));
    MaodieAvatar = Avatar.Object;
    EvacSound = CompletionSound.Object;
}

ABBBPlayerController *UBBBPlayerItemView::GetItemController() const
{
    return Cast<ABBBPlayerController>(GetOwningPlayer());
}

bool UBBBPlayerItemView::IsBackpackOpen() const
{
    return bBackpackOpen;
}
int32 UBBBPlayerItemView::GetInspectedSlot() const
{
    return InspectedSlot;
}

void UBBBPlayerItemView::NativeConstruct()
{
    Super::NativeConstruct();
    if (ABBBPlayerController *Controller = GetItemController())
    {
        Controller->OnItemsChanged.AddUniqueDynamic(this, &UBBBPlayerItemView::RefreshItems);
    }
    RefreshItems();
}

void UBBBPlayerItemView::NativeDestruct()
{
    if (ABBBPlayerController *Controller = GetItemController())
    {
        Controller->OnItemsChanged.RemoveDynamic(this, &UBBBPlayerItemView::RefreshItems);
    }
    if (CharacterPreview)
    {
        CharacterPreview->Close();
        CharacterPreview = nullptr;
    }
    Super::NativeDestruct();
}

void UBBBPlayerItemView::SetBackpackOpen(bool bOpen)
{
    bBackpackOpen = bOpen;
    MenuParallax = FVector2D::ZeroVector;
    bQuickBarHeld = false;
    if (!bOpen)
    {
        if (CharacterPreview)
        {
            CharacterPreview->Close();
        }
        CharacterBrush.SetResourceObject(nullptr);
        return;
    }
    ABBBPlayerController *Controller = GetItemController();
    InspectedSlot = INDEX_NONE;
    if (Controller)
    {
        int32 StorageStart = 0;
        int32 StorageCount = 0;
        Controller->GetStorageRange(EBBBItemType::Equipment, StorageStart, StorageCount);
        for (int32 SlotIndex = 0; SlotIndex < StorageStart + StorageCount; ++SlotIndex)
        {
            if (Controller->GetItemDisplayData(SlotIndex).bOccupied)
            {
                InspectedSlot = SlotIndex;
                break;
            }
        }
    }
    InspectedInstance = Controller ? Controller->GetItemDisplayData(InspectedSlot).InstanceId : FGuid();
    StoragePage = EBBBItemType::Equipment;
    PanelPage = EBBBItemPanelPage::Inventory;
    if (!CharacterPreview)
    {
        CharacterPreview = NewObject<UBBBPlayerItemPortrait>(this);
    }
    CharacterPreview->Prepare(Controller ? Controller->GetPawn() : nullptr);
    CharacterPreview->SetActive(bBackpackOpen);
    CharacterBrush.SetResourceObject(CharacterPreview->GetMaterial());
    CharacterBrush.DrawAs = ESlateBrushDrawType::Image;
    CharacterBrush.ImageSize = FVector2D(640.0f, 1080.0f);
    RefreshItems();
    SetKeyboardFocus();
}

void UBBBPlayerItemView::NativeTick(const FGeometry &Geometry, float DeltaTime)
{
    Super::NativeTick(Geometry, DeltaTime);
    UpdateExplorationPresentation();
    if (!bBackpackOpen)
    {
        return;
    }
    const FVector2D Size = Geometry.GetLocalSize();
    if (Size.X > 0 && Size.Y > 0)
    {
        const FVector2D Mouse = Geometry.AbsoluteToLocal(FSlateApplication::Get().GetCursorPos());
        const FVector2D Target(FMath::Clamp(Mouse.X / Size.X * 2 - 1, -1.0, 1.0),
            FMath::Clamp(Mouse.Y / Size.Y * 2 - 1, -1.0, 1.0));
        MenuParallax = FMath::Vector2DInterpTo(MenuParallax, Target, DeltaTime, 7.0f);
    }
    if (!CharacterPreview)
    {
        CharacterPreview = NewObject<UBBBPlayerItemPortrait>(this);
    }
    auto *Controller = GetItemController();
    CharacterPreview->Prepare(Controller ? Controller->GetPawn() : nullptr);
    CharacterPreview->SetActive(bBackpackOpen);
    CharacterPreview->Update(DeltaTime);
    CharacterBrush.SetResourceObject(CharacterPreview->GetMaterial());
}

void UBBBPlayerItemView::SetQuickBarHeld(bool bHeld)
{
    bQuickBarHeld = bHeld;
    RefreshItems();
}
void UBBBPlayerItemView::NotifyQuickSelection()
{
    QuickSelectionTime = FPlatformTime::Seconds();
}
float UBBBPlayerItemView::GetQuickSelectionOpacity() const
{
    const ABBBPlayerController *Controller = GetItemController();
    if (!Controller || Controller->IsPlayerMenuOpen())
    {
        return 0.0f;
    }
    if (bQuickBarHeld)
    {
        return 1.0f;
    }
    return FMath::Clamp(static_cast<float>((1.8 - (FPlatformTime::Seconds() - QuickSelectionTime)) / 0.35), 0.0f, 1.0f);
}

void UBBBPlayerItemView::InspectSlot(int32 SlotIndex)
{
    ABBBPlayerController *Controller = GetItemController();
    if (!Controller || !Controller->GetItemDisplayData(SlotIndex).bOccupied)
    {
        return;
    }
    InspectedSlot = SlotIndex;
    InspectedInstance = Controller->GetItemDisplayData(SlotIndex).InstanceId;
}

FReply UBBBPlayerItemView::ShowStorage(EBBBItemType Type)
{
    StoragePage = Type;
    RefreshItems();
    return FReply::Handled();
}
FReply UBBBPlayerItemView::ShowPanel(EBBBItemPanelPage Page)
{
    PanelPage = Page;
    StoragePage = Page == EBBBItemPanelPage::Character ? EBBBItemType::Wearable : EBBBItemType::Equipment;
    RefreshItems();
    return FReply::Handled();
}

const FSlateBrush *UBBBPlayerItemView::GetCharacterBrush() const
{
    return &CharacterBrush;
}

namespace
{
// Project each cell about the centre of its wing. Applying the transform to the widget also projects its hit area.
void ProjectInventoryCell(const TSharedRef<SWidget>& Cell, float Inward, float RowOffset, float Facing, float Width)
{
    const float Depth = 1.0f / (1.0f + Inward * 0.095f);
    const float Slope = -Facing * RowOffset * 0.095f / 480.0f;
    Cell->SetRenderTransformPivot(FVector2D(0.5, 0.5));
    Cell->SetRenderTransform(FSlateRenderTransform(FMatrix2x2(Depth, Slope, 0.f, Depth),
        FVector2f(-Facing * Inward * (1.0f - Depth) * Width, RowOffset * (Depth - 1.0f))));
}
}

void UBBBPlayerItemView::RefreshItems()
{
    ABBBPlayerController *Controller = GetItemController();
    if (!Controller || !QuickBar || !BackpackSlots || !EquipmentSlots)
    {
        return;
    }
    const int32 Selected = Controller->GetSelectedItemSlot();
    if (ObservedPawn.Get() == Controller->GetPawn() && Selected != ObservedSelectedSlot)
    {
        NotifyQuickSelection();
    }
    ObservedPawn = Controller->GetPawn();
    ObservedSelectedSlot = Selected;
    int32 QuickStart = 0;
    int32 QuickCount = Controller->GetQuickAccessSlotCount();
    if (bQuickBarHeld)
    {
        Controller->GetConsumableQuickRange(QuickStart, QuickCount);
    }
    if (BuiltQuickStart != QuickStart || QuickSlotViews.Num() != QuickCount)
    {
        QuickBar->ClearChildren();
        QuickSlotViews.Reset();
        BuiltQuickStart = QuickStart;
        for (int32 Index = 0; Index < QuickCount; ++Index)
        {
            TSharedPtr<SBBBPlayerItemSlot> SlotWidget;
            QuickBar->AddSlot().FillWidth(1.0f).Padding(12.0f)
                [SAssignNew(SlotWidget, SBBBPlayerItemSlot)
                    .View(this).Slot(QuickStart + Index).Compact(true)
                    .Width(160.0f).Height(90.0f)];
            QuickSlotViews.Add(SlotWidget);
        }
    }
    for (const auto &SlotWidget : QuickSlotViews)
    {
        SlotWidget->Refresh();
    }
    if (!bBackpackOpen)
    {
        return;
    }
    if (InspectedInstance.IsValid())
    {
        InspectedSlot = INDEX_NONE;
        for (int32 SlotIndex = 0; SlotIndex < Controller->GetItemSlotCount(); ++SlotIndex)
        {
            if (Controller->GetItemDisplayData(SlotIndex).InstanceId == InspectedInstance)
            {
                InspectedSlot = SlotIndex;
                break;
            }
        }
    }
    int32 Start = 0;
    int32 Count = 0;
    Controller->GetStorageRange(StoragePage, Start, Count);
    if (BuiltStorageStart != Start || StorageSlotViews.Num() != Count)
    {
        BackpackSlots->ClearChildren();
        StorageSlotViews.Reset();
        BuiltStorageStart = Start;
        TSharedRef<SUniformGridPanel> Grid = SNew(SUniformGridPanel).SlotPadding(FMargin(5.0f));
        for (int32 Position = 0; Position < Count; ++Position)
        {
            TSharedPtr<SBBBPlayerItemSlot> SlotWidget;
            Grid->AddSlot(Position % 3, Position / 3)
                [SAssignNew(SlotWidget, SBBBPlayerItemSlot).View(this).Slot(Start + Position)
                    .Width(145.0f).Height(142.0f)];
            ProjectInventoryCell(SlotWidget.ToSharedRef(), (Position % 3) / 2.0f,
                (FMath::Min(Position / 3, 4) - 1.5f) * 150, 1, 145);
            StorageSlotViews.Add(SlotWidget);
        }
        BackpackSlots->AddSlot().AutoHeight()[Grid];
    }
    for (const auto &SlotWidget : StorageSlotViews)
    {
        SlotWidget->Refresh();
    }
    TArray<FName> WearNames;
    Controller->GetWearSlotRange(Start, WearNames);
    const bool bCharacterPage = PanelPage == EBBBItemPanelPage::Character;
    int32 ConsumableStart = 0;
    int32 ConsumableCount = 0;
    Controller->GetConsumableQuickRange(ConsumableStart, ConsumableCount);
    const int32 RightStart = bCharacterPage ? Start : 0;
    const int32 WeaponCount = Controller->GetQuickAccessSlotCount();
    const int32 Total = bCharacterPage ? WearNames.Num() : WeaponCount + ConsumableCount;
    if (BuiltPanelPage != PanelPage || BuiltPanelStart != RightStart || BuiltConsumableStart != ConsumableStart
        || PanelSlotViews.Num() != Total || (bCharacterPage && BuiltWearNames != WearNames))
    {
        EquipmentSlots->ClearChildren();
        PanelSlotViews.Reset();
        BuiltPanelPage = PanelPage;
        BuiltPanelStart = RightStart;
        BuiltConsumableStart = ConsumableStart;
        BuiltWearNames = MoveTemp(WearNames);
        auto AddGroup = [this](int32 First, int32 Size, int32 Columns, float Width, float Height, bool bWear)
        {
            auto Grid = SNew(SUniformGridPanel).SlotPadding(FMargin(4.0f));
            for (int32 Index = 0; Index < Size; ++Index)
            {
                TSharedPtr<SBBBPlayerItemSlot> Cell;
                Grid->AddSlot(Index % Columns, Index / Columns)
                    [SAssignNew(Cell, SBBBPlayerItemSlot).View(this).Slot(First + Index)
                        .PositionLabel(bWear ? FText::FromName(BuiltWearNames[Index]) : FText::GetEmpty())
                        .Width(Width).Height(Height)];
                const float Inward = Columns == 1 ? 0.5f : 1.0f - static_cast<float>(Index % Columns) / (Columns - 1);
                const float RowOffset = (Index / Columns - 1.5f) * Height + (!bWear && Columns > 1 ? 360.f : 0.f);
                ProjectInventoryCell(Cell.ToSharedRef(), Inward, RowOffset, -1, Width);
                PanelSlotViews.Add(Cell);
            }
            EquipmentSlots->AddSlot().AutoHeight()[Grid];
        };
        if (bCharacterPage)
        {
            AddGroup(RightStart, Total, 3, 148, 145, true);
        }
        else
        {
            AddGroup(0, WeaponCount, 1, 472, 105, false);
            EquipmentSlots->AddSlot().AutoHeight().Padding(4, 24, 0, 6)
                [SNew(STextBlock).Text(INVTEXT("CONSUMABLES")).Font(BBBItemUI::ArtFont(22))
                    .ColorAndOpacity(BBBItemUI::MutedText)];
            AddGroup(ConsumableStart, ConsumableCount, 3, 148, 145, false);
        }
    }
    for (const auto &SlotWidget : PanelSlotViews)
    {
        SlotWidget->Refresh();
    }
    int32 Revision = 0;
    int32 Succeeded = 0;
    int32 Rejected = 0;
    Controller->GetItemOperationResult(Revision, Succeeded, Rejected);
    if (StatusText && Revision != ObservedOperationRevision)
    {
        StatusText->SetText(Rejected ? BBBItemText::PlacementRejected : FText::GetEmpty());
    }
    ObservedOperationRevision = Revision;
}

bool UBBBPlayerItemView::SelectSlot(int32 SlotIndex)
{
    ABBBPlayerController *Controller = GetItemController();
    if (!Controller)
    {
        return false;
    }
    int32 ConsumableStart = 0;
    int32 ConsumableCount = 0;
    Controller->GetConsumableQuickRange(ConsumableStart, ConsumableCount);
    const bool bWeaponSlot = SlotIndex >= INDEX_NONE && SlotIndex < Controller->GetQuickAccessSlotCount();
    const bool bConsumableSlot = SlotIndex >= ConsumableStart && SlotIndex < ConsumableStart + ConsumableCount;
    return (bWeaponSlot || bConsumableSlot) && Controller->SubmitItemSelect(SlotIndex);
}

bool UBBBPlayerItemView::MoveItem(int32 Source, int32 Target, const APawn *SourcePawn, FGuid InstanceId)
{
    ABBBPlayerController *Controller = GetItemController();
    return bBackpackOpen && Controller && Controller->GetPawn() == SourcePawn && InstanceId.IsValid() &&
           Controller->GetItemDisplayData(Source).InstanceId == InstanceId &&
           Controller->SubmitItemMove(Source, Target, InstanceId);
}

bool UBBBPlayerItemView::EquipItem(int32 Source, const APawn *SourcePawn, FGuid InstanceId)
{
    ABBBPlayerController *Controller = GetItemController();
    if (!bBackpackOpen || !Controller || Controller->GetPawn() != SourcePawn || !InstanceId.IsValid())
    {
        return false;
    }
    const FBBBPlayerItemDisplayData Data = Controller->GetItemDisplayData(Source);
    if (!Data.bOccupied || Data.InstanceId != InstanceId || Data.EquipSlot == INDEX_NONE)
    {
        return false;
    }
    if (Source != Data.EquipSlot && !MoveItem(Source, Data.EquipSlot, SourcePawn, InstanceId))
    {
        return false;
    }
    int32 ConsumableStart = 0;
    int32 ConsumableCount = 0;
    Controller->GetConsumableQuickRange(ConsumableStart, ConsumableCount);
    if (Data.EquipSlot == 0 || Data.EquipSlot == ConsumableStart)
    {
        return SelectSlot(Data.EquipSlot);
    }
    return true;
}

FReply UBBBPlayerItemView::NativeOnPreviewKeyDown(const FGeometry &Geometry, const FKeyEvent &Event)
{
    if (bBackpackOpen && PanelPage != EBBBItemPanelPage::Character)
    {
        const FKey Keys[] = {EKeys::One, EKeys::Two, EKeys::Three};
        for (int32 SlotIndex = 0; SlotIndex < UE_ARRAY_COUNT(Keys); ++SlotIndex)
        {
            if (Event.GetKey() == Keys[SlotIndex])
            {
                if (!Event.IsRepeat())
                {
                    int32 Start = 0;
                    int32 Count = 0;
                    if (StoragePage == EBBBItemType::Consumable)
                    {
                        GetItemController()->GetConsumableQuickRange(Start, Count);
                    }
                    SelectSlot(Start + SlotIndex);
                }
                return FReply::Handled();
            }
        }
    }
    if (bBackpackOpen && (Event.GetKey() == EKeys::B || Event.GetKey() == EKeys::Escape))
    {
        if (!Event.IsRepeat())
        {
            GetItemController()->ToggleBackpack();
        }
        return FReply::Handled();
    }
    return Super::NativeOnPreviewKeyDown(Geometry, Event);
}
