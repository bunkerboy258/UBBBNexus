#include "BBBWork/UBBBNexus/Player/UI/BBBPlayerItemView.h"
#include "BBBWork/UBBBNexus/Player/UI/SBBBPlayerItemSlot.h"
#include "BBBWork/UBBBNexus/Player/BBBPlayerController.h"
#include "BBBWork/UBBBNexus/Player/BBBPlayerItemDisplayData.h"
#include "BBBWork/UBBBNexus/Customization/UI/BBBCharacterCustomizationStyle.h"
#include "Engine/FontFace.h"
#include "UObject/ConstructorHelpers.h"
#include "Widgets/Layout/SUniformGridPanel.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Text/STextBlock.h"

DEFINE_LOG_CATEGORY_STATIC(LogBBBPlayerItems, Log, All);

UBBBPlayerItemView::UBBBPlayerItemView(const FObjectInitializer &ObjectInitializer)
    : Super(ObjectInitializer)
{
    SetIsFocusable(true);
    static ConstructorHelpers::FObjectFinder<UFontFace> Font(
        TEXT("/Game/_Project/Customization/UI/Fonts/NotoSansCJKsc_Regular.NotoSansCJKsc_Regular"));
    InterfaceFont = Font.Object;
}

ABBBPlayerController *UBBBPlayerItemView::GetItemController() const
{
    return Cast<ABBBPlayerController>(GetOwningPlayer());
}

bool UBBBPlayerItemView::IsBackpackOpen() const
{
    return bBackpackOpen;
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
    Super::NativeDestruct();
}

void UBBBPlayerItemView::SetBackpackOpen(const bool bOpen)
{
    bBackpackOpen = bOpen;
    if (bOpen)
    {
        RefreshItems();
        SetKeyboardFocus();
    }
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
    const double Age = FPlatformTime::Seconds() - QuickSelectionTime;
    return FMath::Clamp(static_cast<float>((1.8 - Age) / 0.35), 0.0f, 1.0f);
}

void UBBBPlayerItemView::RefreshItems()
{
    ABBBPlayerController *Controller = GetItemController();
    if (!Controller || !QuickBar.IsValid() || !BackpackSlots.IsValid())
    {
        return;
    }
    const bool bPawnChanged = ObservedPawn.Get() != Controller->GetPawn();
    const int32 Selected = Controller->GetSelectedItemSlot();
    if (!bPawnChanged && Selected != ObservedSelectedSlot)
    {
        NotifyQuickSelection();
    }
    ObservedPawn = Controller->GetPawn();
    ObservedSelectedSlot = Selected;
    const TArray<AActor *> Items = Controller->GetBackpackItems();
    const int32 QuickCount = Controller->GetQuickAccessSlotCount();
    QuickBar->ClearChildren();
    BackpackSlots->ClearChildren();
    TSharedRef<SHorizontalBox> QuickRow = SNew(SHorizontalBox);
    for (int32 SlotIndex = 0; SlotIndex < QuickCount; ++SlotIndex)
    {
        QuickBar->AddSlot().AutoWidth().Padding(4.0f)
        [SNew(SBBBPlayerItemSlot).View(this).Slot(SlotIndex).Compact(true)];
        QuickRow->AddSlot().FillWidth(1.0f).Padding(3.0f)
        [SNew(SBBBPlayerItemSlot).View(this).Slot(SlotIndex)];
    }
    const auto AddSection = [this](const TCHAR *Title)
    {
        BackpackSlots->AddSlot().AutoHeight().Padding(3.0f, 0.0f, 3.0f, 10.0f)
        [SNew(STextBlock).Text(FText::FromString(Title))
            .Font(BBBCustomizationStyle::GetCustomizationFont(11))
            .ColorAndOpacity(FLinearColor(0.7f, 0.7f, 0.7f))];
    };
    AddSection(TEXT("快捷栏"));
    BackpackSlots->AddSlot().AutoHeight().Padding(0.0f, 0.0f, 0.0f, 24.0f)[QuickRow];
    AddSection(TEXT("随身物品"));
    TSharedRef<SUniformGridPanel> Grid = SNew(SUniformGridPanel).SlotPadding(FMargin(3.0f));
    for (int32 SlotIndex = QuickCount; SlotIndex < Items.Num(); ++SlotIndex)
    {
        const int32 Position = SlotIndex - QuickCount;
        Grid->AddSlot(Position % 4, Position / 4)
        [SNew(SBBBPlayerItemSlot).View(this).Slot(SlotIndex)];
    }
    BackpackSlots->AddSlot().AutoHeight()[Grid];
    int32 Revision = 0;
    int32 Succeeded = 0;
    int32 Rejected = 0;
    Controller->GetItemOperationResult(Revision, Succeeded, Rejected);
    if (StatusText.IsValid() && Revision != ObservedOperationRevision)
    {
        StatusText->SetText(Rejected > 0
            ? FText::FromString(TEXT("操作未完成 请检查物品位置")) : FText::GetEmpty());
    }
    ObservedOperationRevision = Revision;
}

bool UBBBPlayerItemView::SelectSlot(const int32 SlotIndex)
{
    ABBBPlayerController *Controller = GetItemController();
    return Controller && SlotIndex >= INDEX_NONE && SlotIndex < Controller->GetQuickAccessSlotCount()
        && Controller->SubmitItemSelect(SlotIndex);
}

bool UBBBPlayerItemView::MoveItem(const int32 Source, const int32 Target,
    const APawn *SourcePawn, const AActor *SourceItem)
{
    ABBBPlayerController *Controller = GetItemController();
    const TArray<AActor *> Items = Controller ? Controller->GetBackpackItems() : TArray<AActor *>();
    if (!bBackpackOpen || !Controller || !IsValid(SourcePawn) || Controller->GetPawn() != SourcePawn
        || !IsValid(SourceItem) || !Items.IsValidIndex(Source) || !Items.IsValidIndex(Target)
        || Items[Source] != SourceItem)
    {
        UE_LOG(LogBBBPlayerItems, Warning, TEXT("拖动物品状态已变化 拒绝移动 Source=%d Target=%d"), Source, Target);
        return false;
    }
    return Controller->SubmitItemMove(Source, Target);
}

FText UBBBPlayerItemView::GetCapacityText() const
{
    const ABBBPlayerController *Controller = GetItemController();
    const TArray<AActor *> Items = Controller ? Controller->GetBackpackItems() : TArray<AActor *>();
    int32 Occupied = 0;
    for (const AActor *Item : Items)
    {
        if (IsValid(Item))
        {
            ++Occupied;
        }
    }
    return FText::Format(FText::FromString(TEXT("{0} / {1}")), Occupied, Items.Num());
}

FReply UBBBPlayerItemView::NativeOnPreviewKeyDown(const FGeometry &Geometry, const FKeyEvent &KeyEvent)
{
    if (bBackpackOpen && (KeyEvent.GetKey() == EKeys::Tab || KeyEvent.GetKey() == EKeys::Escape))
    {
        if (!KeyEvent.IsRepeat())
        {
            GetItemController()->ToggleBackpack();
        }
        return FReply::Handled();
    }
    if (bBackpackOpen)
    {
        const FKey Keys[] = {EKeys::One, EKeys::Two, EKeys::Three};
        for (int32 SlotIndex = 0; SlotIndex < UE_ARRAY_COUNT(Keys); ++SlotIndex)
        {
            if (KeyEvent.GetKey() == Keys[SlotIndex])
            {
                if (!KeyEvent.IsRepeat())
                {
                    SelectSlot(SlotIndex);
                }
                return FReply::Handled();
            }
        }
    }
    return Super::NativeOnPreviewKeyDown(Geometry, KeyEvent);
}
