#include "BBBWork/UBBBNexus/Player/UI/BBBPlayerItemView.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "BBBWork/UBBBNexus/Player/UI/SBBBPlayerItemSlot.h"
#include "BBBWork/UBBBNexus/Player/UI/BBBPlayerItemStyle.h"
#include "BBBWork/UBBBNexus/Player/UI/BBBPlayerItemPortrait.h"
#include "BBBWork/UBBBNexus/Player/BBBPlayerController.h"
#include "BBBWork/UBBBNexus/Player/BBBPlayerItemDisplayData.h"
#include "Engine/FontFace.h"
#include "Engine/Texture2D.h"
#include "Engine/TextureRenderTarget2D.h"
#include "UObject/ConstructorHelpers.h"
#include "Widgets/Layout/SUniformGridPanel.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Text/STextBlock.h"

UBBBPlayerItemView::UBBBPlayerItemView(const FObjectInitializer &ObjectInitializer) : Super(ObjectInitializer)
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
bool UBBBPlayerItemView::IsGearPage() const
{
    return bGearPage;
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
    bQuickBarHeld = false;
    if (!bOpen)
    {
        if (CharacterPreview)
        {
            CharacterPreview->Close();
            CharacterPreview = nullptr;
        }
        CharacterBrush.SetResourceObject(nullptr);
        return;
    }
    ABBBPlayerController *Controller = GetItemController();
    InspectedSlot = INDEX_NONE;
    if (Controller)
    {
        for (int32 SlotIndex = 0; SlotIndex < Controller->GetQuickAccessSlotCount(); ++SlotIndex)
        {
            if (Controller->GetItemDisplayData(SlotIndex).bActive)
            {
                InspectedSlot = SlotIndex;
                break;
            }
        }
    }
    InspectedInstance = Controller ? Controller->GetItemDisplayData(InspectedSlot).InstanceId : FGuid();
    bMiscPage = false;
    bGearPage = false;
    CharacterPreview = NewObject<UBBBPlayerItemPortrait>(this);
    CharacterPreview->Open(Controller ? Controller->GetPawn() : nullptr);
    CharacterBrush.SetResourceObject(CharacterPreview->GetMaterial());
    CharacterBrush.DrawAs = ESlateBrushDrawType::Image;
    CharacterBrush.ImageSize = FVector2D(640.0f, 1080.0f);
    RefreshItems();
    SetKeyboardFocus();
}

void UBBBPlayerItemView::NativeTick(const FGeometry &Geometry, float DeltaTime)
{
    Super::NativeTick(Geometry, DeltaTime);
    if (bBackpackOpen && CharacterPreview)
    {
        CharacterPreview->Update(DeltaTime);
    }
}

void UBBBPlayerItemView::SetQuickBarHeld(bool bHeld)
{
    bQuickBarHeld = bHeld;
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
    RefreshDetail();
}

FReply UBBBPlayerItemView::ShowStorage(bool bMisc)
{
    bMiscPage = bMisc;
    RefreshItems();
    return FReply::Handled();
}
FReply UBBBPlayerItemView::ShowGear(bool bGear)
{
    bGearPage = bGear;
    RefreshItems();
    return FReply::Handled();
}

const FSlateBrush *UBBBPlayerItemView::GetDetailBrush() const
{
    return &DetailBrush;
}
const FSlateBrush *UBBBPlayerItemView::GetCharacterBrush() const
{
    return &CharacterBrush;
}

void UBBBPlayerItemView::RefreshDetail()
{
    const auto *Controller = GetItemController();
    const auto Data = Controller ? Controller->GetItemDisplayData(InspectedSlot) : FBBBPlayerItemDisplayData();
    UTexture2D *Image = Data.DisplayImage ? Data.DisplayImage : Data.Icon;
    DetailBrush.SetResourceObject(Data.DisplayMaterial ? static_cast<UObject *>(Data.DisplayMaterial) : Image);
    DetailBrush.DrawAs = Data.DisplayMaterial || Image ? ESlateBrushDrawType::Image : ESlateBrushDrawType::NoDrawType;
    DetailBrush.ImageSize = Image ? FVector2D(Image->GetSizeX(), Image->GetSizeY()) : FVector2D(512.0f);
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
    RefreshDetail();
    QuickBar->ClearChildren();
    BackpackSlots->ClearChildren();
    EquipmentSlots->ClearChildren();
    for (int32 SlotIndex = 0; SlotIndex < Controller->GetQuickAccessSlotCount(); ++SlotIndex)
    {
        QuickBar->AddSlot().FillWidth(1.0f).Padding(
            12.0f)[SNew(SBBBPlayerItemSlot).View(this).Slot(SlotIndex).Compact(true).Width(160.0f).Height(90.0f)];
    }
    int32 Start = 0;
    int32 Count = 0;
    if (bMiscPage)
    {
        Controller->GetMiscStorageRange(Start, Count);
    }
    if (!bMiscPage)
    {
        Controller->GetEquipmentStorageRange(Start, Count);
    }
    TSharedRef<SUniformGridPanel> Grid = SNew(SUniformGridPanel).SlotPadding(FMargin(5.0f));
    const int32 Columns = bMiscPage ? 6 : 5;
    const float Width = bMiscPage ? 126.0f : 154.0f;
    for (int32 Position = 0; Position < Count; ++Position)
    {
        Grid->AddSlot(
            Position % Columns,
            Position /
                Columns)[SNew(SBBBPlayerItemSlot).View(this).Slot(Start + Position).Width(Width).Height(Width * 0.57f)];
    }
    BackpackSlots->AddSlot().AutoHeight()[Grid];
    TArray<FName> WearNames;
    Controller->GetWearSlotRange(Start, WearNames);
    TSharedRef<SUniformGridPanel> Right = SNew(SUniformGridPanel).SlotPadding(FMargin(5.0f));
    const int32 RightCount = bGearPage ? WearNames.Num() : Controller->GetQuickAccessSlotCount();
    for (int32 Position = 0; Position < RightCount; ++Position)
    {
        Right->AddSlot(
            bGearPage ? Position % 2 : 0,
            bGearPage
                ? Position / 2
                : Position)[SNew(SBBBPlayerItemSlot)
                                .View(this)
                                .Slot(bGearPage ? Start + Position : Position)
                                .PositionLabel(bGearPage ? FText::FromString(WearNames[Position].ToString().ToUpper()) : FText::GetEmpty())
                                .Width(bGearPage ? 162.0f : 340.0f)
                                .Height(bGearPage ? 150.0f : 126.0f)];
    }
    EquipmentSlots->AddSlot().AutoHeight()[Right];
    int32 Revision = 0;
    int32 Succeeded = 0;
    int32 Rejected = 0;
    Controller->GetItemOperationResult(Revision, Succeeded, Rejected);
    if (StatusText && Revision != ObservedOperationRevision)
    {
        StatusText->SetText(Rejected ? FText::FromString(TEXT("此位置不能放置该物品")) : FText::GetEmpty());
    }
    ObservedOperationRevision = Revision;
}

bool UBBBPlayerItemView::SelectSlot(int32 SlotIndex)
{
    ABBBPlayerController *Controller = GetItemController();
    return Controller && SlotIndex >= INDEX_NONE && SlotIndex < Controller->GetQuickAccessSlotCount() &&
           Controller->SubmitItemSelect(SlotIndex);
}

bool UBBBPlayerItemView::MoveItem(int32 Source, int32 Target, const APawn *SourcePawn, FGuid InstanceId)
{
    ABBBPlayerController *Controller = GetItemController();
    return bBackpackOpen && Controller && Controller->GetPawn() == SourcePawn && InstanceId.IsValid() &&
           Controller->GetItemDisplayData(Source).InstanceId == InstanceId &&
           Controller->SubmitItemMove(Source, Target, InstanceId);
}

FReply UBBBPlayerItemView::NativeOnPreviewKeyDown(const FGeometry &Geometry, const FKeyEvent &Event)
{
    if (bBackpackOpen)
    {
        const FKey Keys[] = {EKeys::One, EKeys::Two, EKeys::Three, EKeys::Four, EKeys::Five};
        for (int32 SlotIndex = 0; SlotIndex < UE_ARRAY_COUNT(Keys); ++SlotIndex)
        {
            if (Event.GetKey() == Keys[SlotIndex])
            {
                if (!Event.IsRepeat())
                {
                    SelectSlot(SlotIndex);
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
