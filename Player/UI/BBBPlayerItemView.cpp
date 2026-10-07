#include "BBBWork/UBBBNexus/Player/UI/BBBPlayerItemView.h"
#include "BBBWork/UBBBNexus/Player/UI/SBBBPlayerItemSlot.h"
#include "BBBWork/UBBBNexus/Player/BBBPlayerController.h"
#include "BBBWork/UBBBNexus/Player/BBBPlayerItemDisplayData.h"
#include "Engine/FontFace.h"
#include "Engine/Texture2D.h"
#include "BBBWork/UBBBNexus/Customization/UI/BBBCharacterCustomizationStyle.h"
#include "UObject/ConstructorHelpers.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/Layout/SScaleBox.h"
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
    static ConstructorHelpers::FObjectFinder<UTexture2D> Surface(
        TEXT("/Game/_Project/Customization/UI/Art/T_BBBUI_CardSurface.T_BBBUI_CardSurface"));
    static ConstructorHelpers::FObjectFinder<UTexture2D> Frame(
        TEXT("/Game/_Project/Customization/UI/Art/T_BBBUI_CardFrame.T_BBBUI_CardFrame"));
    CardSurfaceTexture = Surface.Object;
    CardFrameTexture = Frame.Object;
    CardSurfaceBrush.SetResourceObject(CardSurfaceTexture);
    CardFrameBrush.SetResourceObject(CardFrameTexture);
    CardSurfaceBrush.ImageSize = FVector2D(256.0f);
    CardFrameBrush.ImageSize = FVector2D(256.0f);
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

void UBBBPlayerItemView::RefreshItems()
{
    ABBBPlayerController *Controller = GetItemController();
    if (!Controller || !QuickBar.IsValid() || !BackpackSlots.IsValid())
    {
        return;
    }
    const bool bPawnChanged = ObservedPawn.Get() != Controller->GetPawn();
    if (bPawnChanged)
    {
        ObservedPawn = Controller->GetPawn();
        InspectedSlot = INDEX_NONE;
    }
    const TArray<AActor *> Items = Controller->GetBackpackItems();
    const int32 QuickCount = Controller->GetQuickAccessSlotCount();
    QuickBar->ClearChildren();
    BackpackSlots->ClearChildren();
    TSharedRef<SHorizontalBox> QuickRow = SNew(SHorizontalBox);
    for (int32 SlotIndex = 0; SlotIndex < QuickCount; ++SlotIndex)
    {
        QuickBar->AddSlot().AutoWidth().Padding(0.0f, 0.0f, 6.0f, 0.0f)
        [
            SNew(SBBBPlayerItemSlot).View(this).Slot(SlotIndex).Compact(true)
        ];
        QuickRow->AddSlot().FillWidth(1.0f).Padding(5.0f)
        [
            SNew(SBBBPlayerItemSlot).View(this).Slot(SlotIndex)
        ];
    }
    const auto AddSection = [this](const FText &Title)
    {
        BackpackSlots->AddSlot().AutoHeight().Padding(5.0f, 0.0f, 5.0f, 12.0f)
        [
            SNew(STextBlock).Text(Title)
            .Font(BBBCustomizationStyle::GetCustomizationFont(14)).ColorAndOpacity(BBBCustomizationStyle::Ink)
        ];
    };
    AddSection(FText::FromString(TEXT("快捷装备")));
    BackpackSlots->AddSlot().AutoHeight().Padding(0.0f, 0.0f, 0.0f, 25.0f)[QuickRow];
    AddSection(FText::FromString(TEXT("携带物品")));
    TSharedRef<SUniformGridPanel> Grid = SNew(SUniformGridPanel).SlotPadding(FMargin(5.0f));
    for (int32 SlotIndex = QuickCount; SlotIndex < Items.Num(); ++SlotIndex)
    {
        const int32 Position = SlotIndex - QuickCount;
        Grid->AddSlot(Position % 5, Position / 5)
        [
            SNew(SBBBPlayerItemSlot).View(this).Slot(SlotIndex)
        ];
    }
    BackpackSlots->AddSlot().AutoHeight()[Grid];

    int32 Revision = 0;
    int32 Succeeded = 0;
    int32 Rejected = 0;
    Controller->GetItemOperationResult(Revision, Succeeded, Rejected);
    if (StatusText.IsValid() && !bPawnChanged && Revision != ObservedOperationRevision)
    {
        StatusText->SetText(Rejected > 0
            ? FText::FromString(TEXT("操作未完成，请检查槽位与背包容量"))
            : FText::FromString(TEXT("物品已更新")));
    }
    if (StatusText.IsValid() && bPawnChanged)
    {
        StatusText->SetText(FText::FromString(TEXT("拖动物品可移动或交换，点击快捷槽位可装备")));
    }
    ObservedOperationRevision = Revision;
    InspectSlot(InspectedSlot);
}

void UBBBPlayerItemView::InspectSlot(const int32 SlotIndex)
{
    InspectedSlot = SlotIndex;
    const ABBBPlayerController *Controller = GetItemController();
    const FBBBPlayerItemDisplayData Data = Controller ? Controller->GetItemDisplayData(SlotIndex) : FBBBPlayerItemDisplayData();
    DetailTexture = Data.Icon;
    if (DetailArtwork.IsValid())
    {
        if (DetailTexture)
        {
            DetailBrush.SetResourceObject(DetailTexture);
            DetailBrush.ImageSize = FVector2D(DetailTexture->GetSizeX(), DetailTexture->GetSizeY());
            DetailArtwork->SetContent(SNew(SScaleBox).Stretch(EStretch::ScaleToFit)
                [SNew(SImage).Image(&DetailBrush)]);
        }
        else
        {
            DetailArtwork->SetContent(BBBCustomizationStyle::MakeIcon(
                Data.bOccupied ? TEXT("Item") : TEXT("Backpack"), 76.0f, BBBCustomizationStyle::MutedInk));
        }
    }
    if (DetailName.IsValid())
    {
        DetailName->SetText(!Data.Name.IsEmpty() ? Data.Name : FText::FromString(TEXT("物品详情")));
    }
    if (DetailDescription.IsValid())
    {
        DetailDescription->SetText(Data.bOccupied ? Data.Description
            : FText::FromString(TEXT("将鼠标移到物品上查看说明。\n\n拖动可整理背包；放入前方快捷槽位后，即可使用数字键装备。")));
    }
}

bool UBBBPlayerItemView::SelectSlot(const int32 SlotIndex)
{
    ABBBPlayerController *Controller = GetItemController();
    if (!Controller || SlotIndex < INDEX_NONE || SlotIndex >= Controller->GetQuickAccessSlotCount())
    {
        return false;
    }
    return Controller->SubmitItemSelect(SlotIndex);
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
        UE_LOG(LogBBBPlayerItems, Warning, TEXT("拖动物品状态已变化，拒绝移动 Source=%d Target=%d"), Source, Target);
        if (StatusText.IsValid())
        {
            StatusText->SetText(FText::FromString(TEXT("物品位置已变化，请重新拖动")));
        }
        return false;
    }
    const bool bAccepted = Controller->SubmitItemMove(Source, Target);
    if (StatusText.IsValid())
    {
        StatusText->SetText(FText::FromString(bAccepted ? TEXT("正在整理物品…") : TEXT("当前无法移动物品")));
    }
    return bAccepted;
}

FText UBBBPlayerItemView::GetActiveItemText() const
{
    const ABBBPlayerController *Controller = GetItemController();
    const FText Name = Controller ? Controller->GetActiveItemName() : FText::GetEmpty();
    return !Name.IsEmpty() ? FText::Format(FText::FromString(TEXT("手持 · {0}")), Name)
        : FText::FromString(TEXT("空手"));
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
    return FText::Format(FText::FromString(TEXT("携带 {0} / {1}")), Occupied, Items.Num());
}

FReply UBBBPlayerItemView::NativeOnPreviewKeyDown(const FGeometry &Geometry, const FKeyEvent &KeyEvent)
{
    if (bBackpackOpen)
    {
        if (KeyEvent.GetKey() == EKeys::F6)
        {
            if (!KeyEvent.IsRepeat())
            {
                GetItemController()->ToggleCustomization();
            }
            return FReply::Handled();
        }
        const FKey Keys[] = {EKeys::One, EKeys::Two, EKeys::Three, EKeys::Four, EKeys::Five};
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
    if (bBackpackOpen && (KeyEvent.GetKey() == EKeys::Tab || KeyEvent.GetKey() == EKeys::Escape))
    {
        if (!KeyEvent.IsRepeat())
        {
            GetItemController()->ToggleBackpack();
        }
        return FReply::Handled();
    }
    return Super::NativeOnPreviewKeyDown(Geometry, KeyEvent);
}
