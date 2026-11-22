#include "BBBWork/UBBBNexus/Player/UI/BBBPlayerItemView.h"
#include "Materials/MaterialInterface.h"
#include "BBBWork/UBBBNexus/Player/UI/SBBBPlayerItemSlot.h"
#include "BBBWork/UBBBNexus/Player/UI/BBBPlayerItemStyle.h"
#include "BBBWork/UBBBNexus/Player/UI/BBBPlayerItemText.h"
#include "BBBWork/UBBBNexus/Player/UI/BBBPlayerItemPortrait.h"
#include "Engine/TextureRenderTarget2D.h"
#include "BBBWork/UBBBNexus/Player/BBBPlayerController.h"
#include "BBBWork/UBBBNexus/Player/BBBPlayerItemDisplayData.h"
#include "BBBWork/UBBBNexus/Character/Input/LocalControl/Item/FBBBCharacterItemMoveLocalControlPacket.h"
#include "Engine/FontFace.h"
#include "Engine/Texture2D.h"
#include "GameFramework/Pawn.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundBase.h"
#include "UObject/ConstructorHelpers.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Layout/SUniformGridPanel.h"
#include "Framework/Application/SlateApplication.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Images/SImage.h"

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
    static ConstructorHelpers::FObjectFinder<UMaterialInterface> FrameMaterial(
        TEXT("/Game/_Project/UI/Bag/M_SlotFrame.M_SlotFrame"));
    SlotFrameMaterial = FrameMaterial.Object;
    static ConstructorHelpers::FObjectFinder<UTexture2D> Avatar(
        TEXT("/Game/_Project/UI/Gameplay/T_MaodieAvatar.T_MaodieAvatar"));
    static ConstructorHelpers::FObjectFinder<USoundBase> CompletionSound(
        TEXT("/Game/_Project/UI/Gameplay/Evac.Evac"));
    MaodieAvatar = Avatar.Object;
    EvacSound = CompletionSound.Object;
    const TCHAR* Names[] = {TEXT("Open"), TEXT("Close"), TEXT("Tab"), TEXT("Lift"), TEXT("Place"), TEXT("Reject"), TEXT("Equip")};
    for (const TCHAR* Name : Names)
    {
        const FString Path = FString::Printf(TEXT("/Game/_Project/UI/Bag/Audio/SW_Inventory_%s.SW_Inventory_%s"), Name, Name);
        InventorySounds.Add(LoadObject<USoundBase>(nullptr, *Path));
    }
}

void UBBBPlayerItemView::PlayInventorySound(EInventorySound Event)
{
    const int32 Index = static_cast<int32>(Event);
    if (InventorySounds.IsValidIndex(Index) && InventorySounds[Index])
    {
        UGameplayStatics::PlaySound2D(this, InventorySounds[Index], Event == EInventorySound::Equip ? 0.22f : 0.36f);
    }
}

void UBBBPlayerItemView::NotifyItemDrag(bool bStarted)
{
    if (bBackpackOpen)
    {
        PlayInventorySound(bStarted ? EInventorySound::Lift : EInventorySound::Reject);
    }
}

ABBBPlayerController *UBBBPlayerItemView::GetItemController() const
{
    return Cast<ABBBPlayerController>(GetOwningPlayer());
}

bool UBBBPlayerItemView::IsBackpackOpen() const
{
    return bBackpackOpen;
}

const ABBBPlayerItemStage* UBBBPlayerItemView::GetItemStage() const
{
    return CharacterPreview ? CharacterPreview->GetStage() : nullptr;
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
    if (bBackpackOpen != bOpen)
    {
        PlayInventorySound(bOpen ? EInventorySound::Open : EInventorySound::Close);
        bAwaitingPlacementResult = false;
    }
    bBackpackOpen = bOpen;
    if (!bOpen)
    {
        if (CharacterPreview)
        {
            CharacterPreview->SetActive(false);
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
    CharacterBrush.SetResourceObject(CharacterPreview->GetTexture());
    CharacterBrush.DrawAs = ESlateBrushDrawType::Image;
    CharacterBrush.ImageSize = FVector2D(1920.0f, 1080.0f);
    RefreshItems();
    SetKeyboardFocus();
}

void UBBBPlayerItemView::NativeTick(const FGeometry &Geometry, float DeltaTime)
{
    Super::NativeTick(Geometry, DeltaTime);
    UpdateExplorationPresentation();
    if (!bBackpackOpen)
    {
        const auto* Controller = GetItemController();
        if (CharacterPreview && !CharacterPreview->IsForPawn(Controller ? Controller->GetPawn() : nullptr))
        {
            CharacterPreview->Close();
            CharacterPreview = nullptr;
        }
        return;
    }
    if (!CharacterPreview)
    {
        CharacterPreview = NewObject<UBBBPlayerItemPortrait>(this);
    }
    auto *Controller = GetItemController();
    CharacterPreview->Prepare(Controller ? Controller->GetPawn() : nullptr);
    CharacterPreview->SetActive(bBackpackOpen);
    CharacterPreview->SetPanels(StoragePanel, LoadoutPanel, NavigationPanel);
    if (CharacterPreview->Update(DeltaTime, ReadParallaxCursor()))
    {
        PlayInventorySound(EInventorySound::Equip);
    }
    if (CharacterBrush.GetResourceObject() != CharacterPreview->GetTexture())
    {
        CharacterBrush.SetResourceObject(CharacterPreview->GetTexture());
    }
}

TOptional<FVector2D> UBBBPlayerItemView::ReadParallaxCursor() const
{
    if (!RoomImage || !FSlateApplication::IsInitialized())
    {
        return FVector2D::ZeroVector;
    }

    const auto& Slate = FSlateApplication::Get();
    if (Slate.IsDragDropping() || Slate.GetPressedMouseButtons().Contains(EKeys::LeftMouseButton))
    {
        return {};
    }

    const FGeometry& Image = RoomImage->GetTickSpaceGeometry();
    const FVector2D Size = Image.GetLocalSize();
    if (!Slate.IsActive() || Size.X <= 0 || Size.Y <= 0)
    {
        return FVector2D::ZeroVector;
    }

    const FVector2D UV = Image.AbsoluteToLocal(Slate.GetCursorPos()) / Size;
    if (UV.X < 0.0 || UV.Y < 0.0 || UV.X > 1.0 || UV.Y > 1.0)
    {
        return FVector2D::ZeroVector;
    }
    const auto ApplyDeadZone = [](double Axis)
    {
        constexpr double DeadZone = 0.04;
        const double Magnitude = FMath::Max(0.0, (FMath::Abs(Axis) - DeadZone) / (1.0 - DeadZone));
        return FMath::Sign(Axis) * Magnitude;
    };
    return FVector2D(ApplyDeadZone(2.0 * UV.X - 1.0), ApplyDeadZone(1.0 - 2.0 * UV.Y));
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
    return FMath::Clamp(static_cast<float>((1.8 - (FPlatformTime::Seconds() - QuickSelectionTime)) / 0.35), 0.0f, 1.0f);
}

void UBBBPlayerItemView::InspectSlot(int32 SlotIndex)
{
    ABBBPlayerController *Controller = GetItemController();
    if (!Controller || !Controller->GetItemDisplayData(SlotIndex).bOccupied)
    {
        return;
    }
    const int32 PreviousSlot = InspectedSlot;
    InspectedSlot = SlotIndex;
    InspectedInstance = Controller->GetItemDisplayData(SlotIndex).InstanceId;
    for (const auto* Slots : {&StorageSlotViews, &PanelSlotViews})
    {
        for (const auto& SlotView : *Slots)
        {
            const int32 Index = SlotView->GetSlotIndex();
            if (Index == PreviousSlot || Index == InspectedSlot)
            {
                SlotView->Invalidate(EInvalidateWidgetReason::Paint);
            }
        }
    }
}

FReply UBBBPlayerItemView::ShowStorage(EBBBItemType Type)
{
    if (StoragePage != Type)
    {
        PlayInventorySound(EInventorySound::Tab);
    }
    StoragePage = Type;
    RefreshItems();
    return FReply::Handled();
}
FReply UBBBPlayerItemView::ShowPanel(EBBBItemPanelPage Page)
{
    const EBBBItemType Storage = Page == EBBBItemPanelPage::Character ? EBBBItemType::Wearable : EBBBItemType::Equipment;
    if (PanelPage != Page || StoragePage != Storage)
    {
        PlayInventorySound(EInventorySound::Tab);
    }
    PanelPage = Page;
    StoragePage = Storage;
    RefreshItems();
    return FReply::Handled();
}

#if WITH_DEV_AUTOMATION_TESTS
TArray<TSharedPtr<SWidget>> UBBBPlayerItemView::GetMenuPanels() const
{
    return {BackpackChrome, StoragePanel, LoadoutPanel, NavigationPanel};
}
#endif

const FSlateBrush *UBBBPlayerItemView::GetCharacterBrush() const
{
    return CharacterBrush.GetResourceObject() ? &CharacterBrush : nullptr;
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
    if (ObservedPawn.Get() != Controller->GetPawn())
    {
        bAwaitingPlacementResult = false;
    }
    ObservedPawn = Controller->GetPawn().Get();
    ObservedSelectedSlot = Selected;
    const int32 QuickCount = Controller->GetQuickAccessSlotCount();
    if (QuickSlotViews.Num() != QuickCount)
    {
        QuickBar->ClearChildren();
        QuickSlotViews.Reset();
        for (int32 Index = 0; Index < QuickCount; ++Index)
        {
            TSharedPtr<SBBBPlayerItemSlot> SlotWidget;
            QuickBar->AddSlot().FillWidth(1.0f).Padding(12.0f)
                [SAssignNew(SlotWidget, SBBBPlayerItemSlot)
                    .View(this).Slot(Index).Compact(true)
                    .Width(160.0f).Height(90.0f)];
            QuickSlotViews.Add(SlotWidget);
        }
    }
    else
    {
        for (const auto &SlotWidget : QuickSlotViews)
        {
            SlotWidget->Refresh();
        }
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
                    .Width(145.0f).Height(BBBItemUI::StorageCellHeight)];
            StorageSlotViews.Add(SlotWidget);
        }
        BackpackSlots->AddSlot().AutoHeight()[Grid];
    }
    else
    {
        for (const auto &SlotWidget : StorageSlotViews)
        {
            SlotWidget->Refresh();
        }
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
        auto AddGroup = [this](int32 First, int32 Size, int32 Columns, float Width, float Height, bool bWear, bool bConsumable)
        {
            auto Grid = SNew(SUniformGridPanel).SlotPadding(FMargin(4.0f));
            for (int32 Index = 0; Index < Size; ++Index)
            {
                TSharedPtr<SBBBPlayerItemSlot> Cell;
                Grid->AddSlot(Index % Columns, Index / Columns)
                    [SAssignNew(Cell, SBBBPlayerItemSlot).View(this).Slot(First + Index).OuterRight(true)
                        .PositionLabel(bWear ? FText::FromName(BuiltWearNames[Index]) : FText::GetEmpty())
                        .Width(Width).Height(Height).ConsumablePreview(bConsumable)];
                PanelSlotViews.Add(Cell);
            }
            EquipmentSlots->AddSlot().AutoHeight()[Grid];
        };
        if (bCharacterPage)
        {
            constexpr int32 WearColumns = 2;
            const int32 Rows = FMath::Max(1, FMath::DivideAndRoundUp(Total, WearColumns));
            const float CellHeight = BBBItemUI::InventoryGridHeight / Rows - 8.0f;
            AddGroup(RightStart, Total, WearColumns, 233.0f, CellHeight, true, false);
        }
        else
        {
            constexpr int32 ConsumableColumns = 3;
            constexpr float ConsumableCellHeight = 145.0f;
            constexpr float HeadingHeight = 52.0f;
            constexpr float CellPadding = 8.0f;
            const int32 ConsumableRows = FMath::DivideAndRoundUp(ConsumableCount, ConsumableColumns);
            const float BottomGroupHeight = HeadingHeight + ConsumableRows * (ConsumableCellHeight + CellPadding);
            const float WeaponCellHeight = FMath::Max(1.0f,
                (BBBItemUI::InventoryGridHeight - BottomGroupHeight) / FMath::Max(1, WeaponCount) - CellPadding);
            AddGroup(0, WeaponCount, 1, 472, WeaponCellHeight, false, false);
            EquipmentSlots->AddSlot().AutoHeight().Padding(4, 0, 0, 0)
                [SNew(SBox).HeightOverride(HeadingHeight).VAlign(VAlign_Center)
                    [SNew(STextBlock).Text(INVTEXT("CONSUMABLES")).Font(BBBItemUI::ArtFont(22))
                        .ColorAndOpacity(BBBItemUI::MutedText)]];
            AddGroup(ConsumableStart, ConsumableCount, ConsumableColumns, 148, ConsumableCellHeight, false, true);
        }
    }
    else
    {
        for (const auto &SlotWidget : PanelSlotViews)
        {
            SlotWidget->Refresh();
        }
    }
    int32 Revision = 0;
    int32 Succeeded = 0;
    int32 Rejected = 0;
    Controller->GetItemOperationResult(Revision, Succeeded, Rejected);
    if (StatusText && Revision != ObservedOperationRevision)
    {
        StatusText->SetText(Rejected ? BBBItemText::PlacementRejected : FText::GetEmpty());
    }
    if (bAwaitingPlacementResult && Revision != ObservedOperationRevision && (Rejected > 0 || Succeeded > 0))
    {
        bAwaitingPlacementResult = false;
        PlayInventorySound(Rejected > 0 ? EInventorySound::Reject : EInventorySound::Place);
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
    const bool bAccepted = bBackpackOpen && Controller && Controller->GetPawn() == SourcePawn && InstanceId.IsValid() &&
           Controller->GetItemDisplayData(Source).InstanceId == InstanceId &&
           Controller->SubmitItemMove(Source, Target, InstanceId);
    bAwaitingPlacementResult |= bAccepted;
    if (bBackpackOpen && !bAccepted)
    {
        PlayInventorySound(EInventorySound::Reject);
    }
    return bAccepted;
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
    if (Data.EquipSlot == FBBBCharacterItemMoveLocalControlPacket::FirstEmptyWeaponQuickSlot)
    {
        if (Data.bQuick)
        {
            return SelectSlot(Source);
        }
        return MoveItem(Source, Data.EquipSlot, SourcePawn, InstanceId);
    }
    if (Source != Data.EquipSlot && !MoveItem(Source, Data.EquipSlot, SourcePawn, InstanceId))
    {
        return false;
    }
    int32 ConsumableStart = 0;
    int32 ConsumableCount = 0;
    Controller->GetConsumableQuickRange(ConsumableStart, ConsumableCount);
    if (Data.EquipSlot == ConsumableStart)
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
