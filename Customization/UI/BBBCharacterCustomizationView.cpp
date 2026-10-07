#include "BBBWork/UBBBNexus/Customization/UI/BBBCharacterCustomizationView.h"
#include "BBBWork/UBBBNexus/Customization/BBBCharacterCustomizationSession.h"
#include "BBBWork/UBBBNexus/Client/UI/BBBPlayerMenuStyle.h"
#include "BBBWork/UBBBNexus/Player/BBBPlayerController.h"
#include "Engine/Texture2D.h"
#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"
#include "Engine/GameViewportClient.h"
#include "Engine/LocalPlayer.h"
#include "GameFramework/PlayerController.h"
#include "Engine/TextureRenderTarget2D.h"
#include "InputCoreTypes.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Input/SSlider.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Layout/SConstraintCanvas.h"
#include "Widgets/Layout/SScrollBox.h"
#include "Widgets/Layout/SUniformGridPanel.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/SOverlay.h"
#include "Widgets/SWindow.h"
#include "Styling/CoreStyle.h"
#include "Widgets/Text/STextBlock.h"
#include "UObject/ConstructorHelpers.h"

DEFINE_LOG_CATEGORY_STATIC(LogBBBCustomizationView, Log, All);

#include "BBBCharacterCustomizationStyle.h"
#include "Brushes/SlateColorBrush.h"
#include "Brushes/SlateNoResource.h"
#include "Widgets/Layout/SScaleBox.h"

using namespace BBBCustomizationStyle;

UBBBCharacterCustomizationView::UBBBCharacterCustomizationView(const FObjectInitializer &ObjectInitializer)
    : Super(ObjectInitializer)
{
    SetIsFocusable(true);
    static ConstructorHelpers::FObjectFinder<UFontFace> FontAsset(
        TEXT("/Game/_Project/Customization/UI/Fonts/NotoSansCJKsc_Regular.NotoSansCJKsc_Regular"));
    InterfaceFont = FontAsset.Object;
}

void UBBBCharacterCustomizationView::SetSession(UBBBCharacterCustomizationSession *InSession)
{
    Session = InSession;
}

void UBBBCharacterCustomizationView::SetPreviewTexture(UTextureRenderTarget2D *InTexture)
{
    PreviewTexture = InTexture;
    PreviewBrush.SetResourceObject(InTexture);
    if (InTexture)
    {
        PreviewBrush.ImageSize = FVector2D(InTexture->SizeX, InTexture->SizeY);
    }
}

void UBBBCharacterCustomizationView::RefreshDraft(const bool bSuccess)
{
    RefreshSlotCardThumbnails();
    UpdatePatchPreview(TEXT("Body"));
    UpdatePatchPreview(TEXT("Vest"));
    StatusMessage = FText::FromString(bSuccess ? TEXT("预览中  应用后保存") : TEXT("预览失败  已恢复原选择"));
    InvalidateLayoutAndVolatility();
}

FName UBBBCharacterCustomizationView::GetSelectedItemId(const FName PartSlot) const
{
    if (!Session)
    {
        return NAME_None;
    }

    const FBBBAppearanceSelection Selection = Session->GetDraft();
    if (PartSlot == TEXT("Attachments"))
    {
        return Selection.Attachments;
    }

    for (const FBBBAppearancePart &Part : Selection.Parts)
    {
        if (Part.Slot == PartSlot)
        {
            return Part.Item;
        }
    }

    return NAME_None;
}

FText UBBBCharacterCustomizationView::GetSelectedItem(const FName PartSlot) const
{
    return GetItemDisplayName(GetSelectedItemId(PartSlot));
}

FText UBBBCharacterCustomizationView::GetItemDisplayName(const FName ItemId) const
{
    if (ItemId.IsNone())
    {
        return FText::FromString(TEXT("未选择"));
    }

    FBBBAppearanceItem Item;
    if (!Session || !Session->GetItem(ItemId, Item))
    {
        return FText::FromString(TEXT("配置不可用"));
    }
    if (Item.DisplayName.IsEmpty())
    {
        UE_LOG(LogBBBCustomizationView, Error, TEXT("外观款式缺少中文显示名 Item=%s"), *ItemId.ToString());
        return FText::FromString(TEXT("未命名款式"));
    }

    return Item.DisplayName;
}

int32 UBBBCharacterCustomizationView::GetSelectedPatchIndex(const FName PartSlot) const
{
    if (!Session)
    {
        return INDEX_NONE;
    }

    for (const FBBBAppearancePart &Part : Session->GetDraft().Parts)
    {
        if (Part.Slot == PartSlot)
        {
            const int32 Column = FMath::Clamp(FMath::RoundToInt(Part.Patch.X * 8.0f), 0, 7);
            const int32 Row = FMath::Clamp(FMath::RoundToInt(Part.Patch.Y * 8.0f), 0, 7);
            return Row * 8 + Column;
        }
    }

    return INDEX_NONE;
}

const FSlateBrush *UBBBCharacterCustomizationView::GetItemBrush(const FName ItemId)
{
    if (ItemId.IsNone())
    {
        return nullptr;
    }

    TSharedPtr<FSlateBrush> &Brush = ItemThumbnailBrushes.FindOrAdd(ItemId);
    if (Brush && Brush->GetResourceObject())
    {
        return Brush.Get();
    }

    FBBBAppearanceItem Item;
    if (!Session || !Session->GetItem(ItemId, Item))
    {
        return nullptr;
    }
    if (Item.Thumbnail.IsNull())
    {
        if (Item.Mesh.IsNull() && Item.Attachments.IsEmpty())
        {
            return nullptr;
        }

        UE_LOG(LogBBBCustomizationView, Error, TEXT("外观款式缺少缩略图配置 Item=%s"), *ItemId.ToString());
        return nullptr;
    }

    if (!Brush)
    {
        Brush = MakeShared<FSlateBrush>();
        Brush->ImageSize = FVector2D(256.0f);
    }
    UTexture2D *Texture = Item.Thumbnail.Get();
    if (!Texture)
    {
        return Brush.Get();
    }

    Brush->SetResourceObject(Texture);
    Brush->ImageSize = FVector2D(Texture->GetSizeX(), Texture->GetSizeY());
    if (Item.Slot == TEXT("Arms"))
    {
        Brush->SetUVRegion(FBox2d(FVector2d(0.02, 0.29), FVector2d(0.49, 0.72)));
        Brush->ImageSize = FVector2D(240.0f, 220.0f);
    }
    if (Item.Slot == TEXT("Boots"))
    {
        Brush->SetUVRegion(FBox2d(FVector2d(0.01, 0.16), FVector2d(0.49, 0.86)));
        Brush->ImageSize = FVector2D(246.0f, 358.0f);
    }
    ItemThumbnails.Add(ItemId, Texture);
    return Brush.Get();
}

void UBBBCharacterCustomizationView::LoadThumbnails()
{
    TArray<FSoftObjectPath> Resources;
    const TArray<FName> Slots = {TEXT("Head"), TEXT("Helmet"), TEXT("Body"), TEXT("Arms"), TEXT("Vest"),
        TEXT("Backpack"), TEXT("Belt"), TEXT("Legs"), TEXT("Boots"), TEXT("Attachments")};
    for (const FName PartSlot : Slots)
    {
        for (const FName ItemId : Session->GetItems(PartSlot))
        {
            FBBBAppearanceItem Item;
            if (Session->GetItem(ItemId, Item) && !Item.Thumbnail.IsNull())
            {
                Resources.AddUnique(Item.Thumbnail.ToSoftObjectPath());
            }
        }
    }
    if (!Resources.IsEmpty())
    {
        ThumbnailLoad = UAssetManager::GetStreamableManager().RequestAsyncLoad(Resources,
            FStreamableDelegate::CreateUObject(this, &ThisClass::RefreshLoadedThumbnails));
    }
}

void UBBBCharacterCustomizationView::RefreshLoadedThumbnails()
{
    TArray<FName> Items;
    ItemThumbnailBrushes.GetKeys(Items);
    for (const FName Item : Items)
    {
        GetItemBrush(Item);
    }
    RefreshSlotCardThumbnails();
    InvalidateLayoutAndVolatility();
}

void UBBBCharacterCustomizationView::RefreshSlotCardThumbnails()
{
    if (!Session)
    {
        return;
    }
    for (TPair<FName, TSharedPtr<SBox>>& Entry : SlotThumbnailBoxes)
    {
        if (!Entry.Value.IsValid())
        {
            continue;
        }
        const FName ItemId = GetSelectedItemId(Entry.Key);
        const FSlateBrush* Thumbnail = GetItemBrush(ItemId);
        if (Thumbnail)
        {
            Entry.Value->SetContent(SNew(SScaleBox).Stretch(EStretch::ScaleToFit)
                [SNew(SImage).Image(Thumbnail)]);
            continue;
        }
        FBBBAppearanceItem Item;
        const bool bHasItem = Session->GetItem(ItemId, Item);
        const bool bEmpty = ItemId.IsNone() || (bHasItem && Item.Mesh.IsNull() && Item.Attachments.IsEmpty());
        const bool bInvalidAttachment = Entry.Key == TEXT("Attachments") && bEmpty
            && ItemId != TEXT("Attachments_None");
        Entry.Value->SetContent(SNew(SBox).HAlign(HAlign_Center).VAlign(VAlign_Center)
            [MakeIcon(bEmpty && !bInvalidAttachment ? Entry.Key : FName(TEXT("Warning")), 60.0f, MutedInk)]);
    }
}

bool UBBBCharacterCustomizationView::IsLeftSideSlot(const FName PartSlot) const
{
    return PartSlot == TEXT("Head") || PartSlot == TEXT("Helmet")
        || PartSlot == TEXT("Body") || PartSlot == TEXT("Arms") || PartSlot == TEXT("Vest");
}

TSharedRef<SWidget> UBBBCharacterCustomizationView::MakeSidePanel(const bool bLeft)
{
    TSharedRef<SBox> SlotPanel = SNew(SBox)
        .Visibility_Lambda([this, bLeft]()
        {
            return ExpandedSlot.IsNone() || IsLeftSideSlot(ExpandedSlot) != bLeft
                ? EVisibility::Visible : EVisibility::Collapsed;
        })
        [
            SNew(SScrollBox)
            .Style(&ItemScrollBoxStyle)
            .ScrollBarStyle(&ItemScrollBarStyle)
            .ScrollWhenFocusChanges(EScrollWhenFocusChanges::AnimatedScroll)
            + SScrollBox::Slot()
            .Padding(0.0f, 0.0f, 4.0f, 0.0f)
            [
                MakeSlotGrid(bLeft)
            ]
        ];

    TSharedRef<SBox> ItemPanel = SNew(SBox)
        .Visibility_Lambda([this, bLeft]()
        {
            return !ExpandedSlot.IsNone() && IsLeftSideSlot(ExpandedSlot) == bLeft && !bShowingPatches
                ? EVisibility::Visible : EVisibility::Collapsed;
        });

    TSharedRef<SBox> PatchPanel = SNew(SBox)
        .Visibility_Lambda([this, bLeft]()
        {
            return !ExpandedSlot.IsNone() && IsLeftSideSlot(ExpandedSlot) == bLeft && bShowingPatches
                ? EVisibility::Visible : EVisibility::Collapsed;
        });

    if (bLeft)
    {
        LeftItemGrid = ItemPanel;
        LeftPatchGrid = PatchPanel;
    }
    if (!bLeft)
    {
        RightItemGrid = ItemPanel;
        RightPatchGrid = PatchPanel;
    }

    TSharedRef<SVerticalBox> Contents = SNew(SVerticalBox);
    Contents->AddSlot().AutoHeight().Padding(5.0f, 0.0f, 5.0f, 18.0f)
    [
        SNew(SHorizontalBox)
        + SHorizontalBox::Slot().AutoWidth().VAlign(VAlign_Center)
        [
            MakeIcon(bLeft ? TEXT("Vest") : TEXT("Backpack"), 21.0f)
        ]
        + SHorizontalBox::Slot().AutoWidth().VAlign(VAlign_Center).Padding(12.0f, 0.0f)
        [
            SNew(STextBlock)
            .Text(FText::FromString(bLeft ? TEXT("穿戴装备") : TEXT("携行装备")))
            .Font(GetCustomizationFont(15))
            .ColorAndOpacity(Ink)
        ]
        + SHorizontalBox::Slot().FillWidth(1.0f).VAlign(VAlign_Center)
        [
            SNew(SBox).HeightOverride(1.0f)
            [
                SNew(SImage).Image(FCoreStyle::Get().GetBrush("WhiteBrush"))
                .ColorAndOpacity(FLinearColor(0.36f, 0.34f, 0.28f, 0.55f))
            ]
        ]
    ];

    Contents->AddSlot()
    .FillHeight(1.0f)
    [
        SNew(SOverlay)
        + SOverlay::Slot()
        [
            SlotPanel
        ]
        + SOverlay::Slot()
        [
            ItemPanel
        ]
        + SOverlay::Slot()
        [
            PatchPanel
        ]
    ];

    if (bLeft)
    {
        Contents->AddSlot()
        .AutoHeight()
        .Padding(0.0f, 6.0f, 0.0f, 0.0f)
        [
            SNew(SVerticalBox)
            .Visibility_Lambda([this]() { return ExpandedSlot.IsNone() ? EVisibility::Visible : EVisibility::Collapsed; })
            + SVerticalBox::Slot()
            .AutoHeight()
            [
                SNew(SButton)
                .ButtonStyle(&ActionButtonStyle)
                .ButtonColorAndOpacity(FLinearColor(0.09f, 0.10f, 0.12f, 0.9f))
                .ContentPadding(FMargin(8.0f, 5.0f))
                .ToolTip(MakeTooltip(FText::FromString(TEXT("表面状态  污渍与磨损"))))
                .OnClicked_Lambda([this]()
                {
                    bSurfaceExpanded = !bSurfaceExpanded;
                    InvalidateLayoutAndVolatility();
                    return FReply::Handled();
                })
                [
                    SNew(SHorizontalBox)
                    + SHorizontalBox::Slot().AutoWidth()
                    [
                        MakeIcon(TEXT("Surface"), 24.0f)
                    ]
                    + SHorizontalBox::Slot().AutoWidth().VAlign(VAlign_Center).Padding(10.0f, 0.0f)
                    [
                        SNew(STextBlock)
                        .Text(FText::FromString(TEXT("表面处理")))
                        .Font(GetCustomizationFont(12))
                        .ColorAndOpacity(Ink)
                    ]
                    + SHorizontalBox::Slot().FillWidth(1.0f).HAlign(HAlign_Right).VAlign(VAlign_Center)
                    [
                        MakeIcon(TEXT("Surface"), 14.0f, MutedInk)
                    ]
                ]
            ]
            + SVerticalBox::Slot()
            .AutoHeight()
            .Padding(0.0f, 5.0f, 0.0f, 0.0f)
            [
                SNew(SBox)
                .Visibility_Lambda([this]()
                {
                    return bSurfaceExpanded && ExpandedSlot.IsNone()
                        ? EVisibility::Visible : EVisibility::Collapsed;
                })
                [
                    MakeSurfaceControls()
                ]
            ]
        ];
    }

    return SNew(SBorder)
        .BorderImage(FCoreStyle::Get().GetBrush("WhiteBrush"))
        .BorderBackgroundColor(FLinearColor::Transparent)
        .Padding(FMargin(8.0f, 10.0f))
        .RenderTransformPivot(FVector2D(bLeft ? 1.0f : 0.0f, 0.5f))
        .RenderTransform(FSlateRenderTransform(FShear2D(0.0f, bLeft ? 0.04f : -0.04f)))
        [
            Contents
        ];
}

void UBBBCharacterCustomizationView::UpdatePatchPreview(const FName PartSlot)
{
    UMaterialInstanceDynamic *Material = PartSlot == TEXT("Body") ? BodyPatchMaterial.Get() : VestPatchMaterial.Get();
    if (!Session || !Material)
    {
        return;
    }

    for (const FBBBAppearancePart &Part : Session->GetDraft().Parts)
    {
        if (Part.Slot == PartSlot)
        {
            Material->SetVectorParameterValue(TEXT("Coord"), FLinearColor(Part.Patch.X, Part.Patch.Y, 0.0f, 0.0f));
            return;
        }
    }
}

TSharedRef<SWidget> UBBBCharacterCustomizationView::MakePatchRow(const FName PartSlot)
{
    TObjectPtr<UMaterialInstanceDynamic> &Material = PartSlot == TEXT("Body") ? BodyPatchMaterial : VestPatchMaterial;
    FSlateBrush &Brush = PartSlot == TEXT("Body") ? BodyPatchBrush : VestPatchBrush;
    UMaterialInterface *BaseMaterial = PatchPreviewMaterial.LoadSynchronous();
    if (BaseMaterial)
    {
        Material = UMaterialInstanceDynamic::Create(BaseMaterial, this);
    }
    if (!Material)
    {
        UE_LOG(LogBBBCustomizationView, Error, TEXT("徽章预览材质不可用 Path=%s"), *PatchPreviewMaterial.ToString());
    }

    Brush.SetResourceObject(Material);
    Brush.ImageSize = FVector2D(48.0f, 48.0f);
    UpdatePatchPreview(PartSlot);

    if (!PatchAtlas)
    {
        PatchAtlas = LoadObject<UTexture2D>(nullptr,
            TEXT("/Game/_ThirdParty/Characters/UkraineSoldier/Textures/Flags/T_Flags_BC.T_Flags_BC"));
    }
    if (!PatchAtlas)
    {
        UE_LOG(LogBBBCustomizationView, Error, TEXT("徽章图案图集不可用"));
    }

    return SNew(SVerticalBox)
        + SVerticalBox::Slot()
        .AutoHeight()
        .Padding(0.0f, 0.0f, 0.0f, 8.0f)
        [
            SNew(SHorizontalBox)
            + SHorizontalBox::Slot()
            .AutoWidth()
            .Padding(0.0f, 0.0f, 10.0f, 0.0f)
            [
                SNew(SBorder)
                .BorderImage(FCoreStyle::Get().GetBrush("WhiteBrush"))
                .BorderBackgroundColor(FLinearColor(0.08f, 0.09f, 0.11f, 0.9f))
                .Padding(FMargin(3.0f))
                [
                    SNew(SImage).Image(&Brush)
                ]
            ]
            + SHorizontalBox::Slot()
            .VAlign(VAlign_Center)
            [
                SNew(STextBlock)
                .Text(FText::FromString(PartSlot == TEXT("Body") ? TEXT("身体徽章") : TEXT("背心徽章")))
                .Font(GetCustomizationFont(15))
                .ColorAndOpacity(Ink)
            ]
        ]
        + SVerticalBox::Slot()
        .FillHeight(1.0f)
        [
            MakePatchGrid(PartSlot)
        ];
}

TSharedRef<SWidget> UBBBCharacterCustomizationView::MakePatchGrid(const FName PartSlot)
{
    PatchBrushes.SetNum(64);
    if (PatchAtlas)
    {
        for (int32 Index = 0; Index < PatchBrushes.Num(); ++Index)
        {
            const int32 Column = Index % 8;
            const int32 Row = Index / 8;
            FSlateBrush &Brush = PatchBrushes[Index];
            Brush.SetResourceObject(PatchAtlas);
            Brush.ImageSize = FVector2D(48.0f, 48.0f);
            Brush.SetUVRegion(FBox2d(
                FVector2d(static_cast<double>(Column) / 8.0, static_cast<double>(Row) / 8.0),
                FVector2d(static_cast<double>(Column + 1) / 8.0, static_cast<double>(Row + 1) / 8.0)));
        }
    }

    TSharedRef<SUniformGridPanel> Grid = SNew(SUniformGridPanel).SlotPadding(FMargin(3.0f));
    for (int32 Index = 0; Index < 64; ++Index)
    {
        Grid->AddSlot(Index % 8, Index / 8)
        [
            SNew(SButton)
            .ButtonStyle(&ActionButtonStyle)
            .ButtonColorAndOpacity(FLinearColor::White)
            .ContentPadding(FMargin(0.0f))
            .OnClicked_Lambda([this, PartSlot, Index]()
            {
                if (Session && Session->SelectPatch(PartSlot, Index))
                {
                    UpdatePatchPreview(PartSlot);
                    StatusMessage = FText::FromString(TEXT("徽章图案已预览  应用后保存"));
                }
                return FReply::Handled();
            })
            [
                SNew(SBorder)
                .BorderImage_Lambda([this, PartSlot, Index]()
                {
                    return GetCardBrush(GetSelectedPatchIndex(PartSlot) == Index);
                })
                .Padding(FMargin(3.0f))
                [
                    SNew(SBox)
                    .WidthOverride(46.0f)
                    .HeightOverride(46.0f)
                    [
                        PatchAtlas
                            ? StaticCastSharedRef<SWidget>(SNew(SImage).Image(&PatchBrushes[Index]))
                            : MakeIcon(TEXT("Warning"), 22.0f, Accent)
                    ]
                ]
            ]
        ];
    }

    return SNew(SScrollBox)
        .Style(&ItemScrollBoxStyle)
        .ScrollBarStyle(&ItemScrollBarStyle)
        .ScrollWhenFocusChanges(EScrollWhenFocusChanges::AnimatedScroll)
        + SScrollBox::Slot()
        .Padding(0.0f, 0.0f, 4.0f, 0.0f)
        [
            Grid
        ];
}

TSharedRef<SWidget> UBBBCharacterCustomizationView::MakeSurfaceControls()
{
    return SNew(SBorder)
        .BorderImage(FCoreStyle::Get().GetBrush("NoBrush"))
        .Padding(FMargin(9.0f, 6.0f))
        [
            SNew(SVerticalBox)
            + SVerticalBox::Slot()
            .AutoHeight()
            [
                SNew(SHorizontalBox)
                + SHorizontalBox::Slot()
                .FillWidth(1.0f)
                [
                    SNew(STextBlock)
                    .Text(FText::FromString(TEXT("污渍")))
                    .Font(GetCustomizationFont(12))
                    .ColorAndOpacity(FLinearColor(0.89f, 0.90f, 0.91f, 1.0f))
                ]
                + SHorizontalBox::Slot()
                .AutoWidth()
                [
                    SNew(STextBlock)
                    .Text_Lambda([this]()
                    {
                        return FText::AsPercent(Session ? Session->GetDraft().Dirt : 0.0f);
                    })
                    .Font(GetCustomizationFont(12))
                    .ColorAndOpacity(FLinearColor(0.67f, 0.69f, 0.71f, 1.0f))
                ]
            ]
            + SVerticalBox::Slot()
            .AutoHeight()
            .Padding(0.0f, 0.0f, 0.0f, 5.0f)
            [
                SNew(SSlider)
                .Style(&GetSurfaceSliderStyle())
                .SliderBarColor(MutedInk)
                .SliderHandleColor(Accent)
                .Value_Lambda([this]()
                {
                    return Session ? Session->GetDraft().Dirt : 0.0f;
                })
                .OnValueChanged_Lambda([this](const float Value)
                {
                    if (Session)
                    {
                        Session->SetSurface(Value, Session->GetDraft().Weathering);
                    }
                })
            ]
            + SVerticalBox::Slot()
            .AutoHeight()
            [
                SNew(SHorizontalBox)
                + SHorizontalBox::Slot()
                .FillWidth(1.0f)
                [
                    SNew(STextBlock)
                    .Text(FText::FromString(TEXT("磨损")))
                    .Font(GetCustomizationFont(12))
                    .ColorAndOpacity(FLinearColor(0.89f, 0.90f, 0.91f, 1.0f))
                ]
                + SHorizontalBox::Slot()
                .AutoWidth()
                [
                    SNew(STextBlock)
                    .Text_Lambda([this]()
                    {
                        return FText::AsPercent(Session ? Session->GetDraft().Weathering : 0.0f);
                    })
                    .Font(GetCustomizationFont(12))
                    .ColorAndOpacity(FLinearColor(0.67f, 0.69f, 0.71f, 1.0f))
                ]
            ]
            + SVerticalBox::Slot()
            .AutoHeight()
            [
                SNew(SSlider)
                .Style(&GetSurfaceSliderStyle())
                .SliderBarColor(MutedInk)
                .SliderHandleColor(Accent)
                .Value_Lambda([this]()
                {
                    return Session ? Session->GetDraft().Weathering : 0.0f;
                })
                .OnValueChanged_Lambda([this](const float Value)
                {
                    if (Session)
                    {
                        Session->SetSurface(Session->GetDraft().Dirt, Value);
                    }
                })
            ]
        ];
}

void UBBBCharacterCustomizationView::OpenSlot(const FName PartSlot)
{
    const double Start = FPlatformTime::Seconds();
    if (!Session || Session->GetItems(PartSlot).IsEmpty())
    {
        UE_LOG(LogBBBCustomizationView, Warning, TEXT("无法打开没有候选款式的部位 Slot=%s"), *PartSlot.ToString());
        return;
    }

    ExpandedSlot = PartSlot;
    Session->FocusSlot(PartSlot);
    CurrentView = NAME_None;
    bShowingPatches = false;
    if (LeftPatchGrid)
    {
        LeftPatchGrid->SetContent(SNullWidget::NullWidget);
    }
    if (RightPatchGrid)
    {
        RightPatchGrid->SetContent(SNullWidget::NullWidget);
    }
    if (IsLeftSideSlot(PartSlot) && LeftItemGrid)
    {
        LeftItemGrid->SetContent(MakeItemGrid(PartSlot));
    }
    if (!IsLeftSideSlot(PartSlot) && RightItemGrid)
    {
        RightItemGrid->SetContent(MakeItemGrid(PartSlot));
    }
    InvalidateLayoutAndVolatility();
    UE_LOG(LogBBBCustomizationView, Verbose, TEXT("款式列表构建 Slot=%s 耗时 %.2f ms"),
        *PartSlot.ToString(), (FPlatformTime::Seconds() - Start) * 1000.0);
}

void UBBBCharacterCustomizationView::ShowPatchGrid(const FName PartSlot)
{
    if (PartSlot != TEXT("Body") && PartSlot != TEXT("Vest"))
    {
        UE_LOG(LogBBBCustomizationView, Warning, TEXT("徽章入口不支持该部位 Slot=%s"), *PartSlot.ToString());
        return;
    }

    ExpandedSlot = PartSlot;
    bShowingPatches = true;
    TSharedRef<SVerticalBox> Panel = SNew(SVerticalBox);
    Panel->AddSlot()
    .AutoHeight()
    .Padding(0.0f, 0.0f, 0.0f, 7.0f)
    [
        MakeSlotStrip(IsLeftSideSlot(PartSlot))
    ];
    Panel->AddSlot()
    .AutoHeight()
    .Padding(0.0f, 0.0f, 0.0f, 7.0f)
    [
        SNew(SButton)
        .ButtonStyle(&ActionButtonStyle)
        .ButtonColorAndOpacity(FLinearColor::White)
        .HAlign(HAlign_Left)
        .ContentPadding(FMargin(7.0f, 4.0f))
        .ToolTip(MakeTooltip(FText::FromString(TEXT("返回款式"))))
        .OnClicked_Lambda([this, PartSlot]()
        {
            OpenSlot(PartSlot);
            return FReply::Handled();
        })
        [
            MakeIcon(TEXT("Back"), 22.0f)
        ]
    ];
    Panel->AddSlot()
    .FillHeight(1.0f)
    [
        MakePatchRow(PartSlot)
    ];

    if (IsLeftSideSlot(PartSlot) && LeftPatchGrid)
    {
        LeftPatchGrid->SetContent(Panel);
    }
    if (!IsLeftSideSlot(PartSlot) && RightPatchGrid)
    {
        RightPatchGrid->SetContent(Panel);
    }
    InvalidateLayoutAndVolatility();
}

void UBBBCharacterCustomizationView::CloseSlot()
{
    if (Session)
    {
        Session->SelectView(TEXT("Full"));
        CurrentView = TEXT("Full");
    }
    ExpandedSlot = NAME_None;
    bShowingPatches = false;
    if (LeftItemGrid)
    {
        LeftItemGrid->SetContent(SNullWidget::NullWidget);
    }
    if (RightItemGrid)
    {
        RightItemGrid->SetContent(SNullWidget::NullWidget);
    }
    if (LeftPatchGrid)
    {
        LeftPatchGrid->SetContent(SNullWidget::NullWidget);
    }
    if (RightPatchGrid)
    {
        RightPatchGrid->SetContent(SNullWidget::NullWidget);
    }
    InvalidateLayoutAndVolatility();
}

TSharedRef<SWidget> UBBBCharacterCustomizationView::RebuildWidget()
{
    if (!Session)
    {
        return SNew(STextBlock).Text(FText::FromString(TEXT("换装会话不可用")));
    }

    LoadInterfaceArt();
    LoadThumbnails();
    SlotThumbnailBoxes.Reset();
    PatchAtlas = LoadObject<UTexture2D>(nullptr,
        TEXT("/Game/_ThirdParty/Characters/UkraineSoldier/Textures/Flags/T_Flags_BC.T_Flags_BC"));
    if (!PatchAtlas)
    {
        UE_LOG(LogBBBCustomizationView, Error, TEXT("身体与背心徽章图集加载失败"));
    }

    TSharedRef<SConstraintCanvas> Layout = SNew(SConstraintCanvas);
    Layout->AddSlot().Anchors(FAnchors(0.065f, 0.055f, 0.935f, 0.16f)).Offset(FMargin(0.0f))
    [
        BBBPlayerMenuStyle::MakeNavigation(false, FOnClicked(), FOnClicked::CreateWeakLambda(this, [this]()
        {
            if (ABBBPlayerController *Player = Cast<ABBBPlayerController>(GetOwningPlayer()))
            {
                Player->ToggleBackpack();
            }
            return FReply::Handled();
        }))
    ];
    Layout->AddSlot()
    .Anchors(FAnchors(0.065f, 0.22f, 0.32f, 0.81f))
    .Offset(FMargin(0.0f))
    [
        MakeSidePanel(true)
    ];

    Layout->AddSlot()
    .Anchors(FAnchors(0.68f, 0.22f, 0.935f, 0.81f))
    .Offset(FMargin(0.0f))
    [
        MakeSidePanel(false)
    ];

    Layout->AddSlot()
    .Anchors(FAnchors(0.065f, 0.92f, 0.935f, 0.975f))
    .Offset(FMargin(0.0f))
    [
        MakeActionBar()
    ];

    RefreshSlotCardThumbnails();
    return SNew(SOverlay)
        + SOverlay::Slot()
        [
            SNew(SImage).Image(&PreviewBrush).DesiredSizeOverride(FVector2D::ZeroVector)
        ]
        + SOverlay::Slot()
        [
            SNew(SBorder)
            .BorderImage(FCoreStyle::Get().GetBrush("WhiteBrush"))
            .BorderBackgroundColor(FLinearColor(0.005f, 0.008f, 0.014f, 0.13f))
            .Visibility(EVisibility::HitTestInvisible)
        ]
        + SOverlay::Slot()
        [
            SNew(SScaleBox).Stretch(EStretch::ScaleToFit)
            [
                SNew(SBox).WidthOverride(1920.0f).HeightOverride(1080.0f)
                [
                    Layout
                ]
            ]
        ];
}

FReply UBBBCharacterCustomizationView::NativeOnPreviewKeyDown(
    const FGeometry &Geometry,
    const FKeyEvent &KeyEvent)
{
    if (KeyEvent.GetKey() == EKeys::Tab)
    {
        if (!KeyEvent.IsRepeat())
        {
            if (ABBBPlayerController *Player = Cast<ABBBPlayerController>(GetOwningPlayer()))
            {
                Player->ToggleBackpack();
            }
        }
        return FReply::Handled();
    }
    const bool bFullscreenKey = KeyEvent.GetKey() == EKeys::F11
        || (KeyEvent.GetKey() == EKeys::Enter && KeyEvent.IsAltDown());
    APlayerController *Player = GetOwningPlayer();
    if (bFullscreenKey && Player && Player->GetLocalPlayer()
        && Player->GetLocalPlayer()->ViewportClient)
    {
        UGameViewportClient *Viewport = Player->GetLocalPlayer()->ViewportClient;
        TSharedPtr<SWindow> Window = Viewport->GetWindow();
        UE_LOG(LogBBBCustomizationView, Verbose, TEXT("全屏快捷键 Window=%s Mode=%d"),
            Window ? *Window->GetTag().ToString() : TEXT("None"), Window ? static_cast<int32>(Window->GetWindowMode()) : -1);
        if (Window && Window->GetTag() == TEXT("PIEWindow"))
        {
            const EWindowMode::Type Mode = Window->GetWindowMode() == EWindowMode::Windowed
                ? EWindowMode::WindowedFullscreen : EWindowMode::Windowed;
            Window->SetWindowMode(Mode);
            return FReply::Handled();
        }
        Viewport->HandleToggleFullscreenCommand();
        return FReply::Handled();
    }
    return Super::NativeOnPreviewKeyDown(Geometry, KeyEvent);
}

FReply UBBBCharacterCustomizationView::NativeOnKeyDown(
    const FGeometry &Geometry,
    const FKeyEvent &KeyEvent)
{
    if (!Session)
    {
        return Super::NativeOnKeyDown(Geometry, KeyEvent);
    }

    if (KeyEvent.GetKey() == EKeys::F6)
    {
        Session->Close();
        return FReply::Handled();
    }

    if (KeyEvent.GetKey() == EKeys::Escape)
    {
        return GoBack();
    }

    return Super::NativeOnKeyDown(Geometry, KeyEvent);
}

void UBBBCharacterCustomizationView::LoadInterfaceArt()
{
    // 纹理与画刷跟随界面持有 保证 Slate 渲染期间资源和地址都有效
    CardSurfaceTexture = LoadObject<UTexture2D>(nullptr,
        TEXT("/Game/_Project/Customization/UI/Art/T_BBBUI_CardSurface.T_BBBUI_CardSurface"));
    CardFrameTexture = LoadObject<UTexture2D>(nullptr,
        TEXT("/Game/_Project/Customization/UI/Art/T_BBBUI_CardFrame.T_BBBUI_CardFrame"));
    if (!CardSurfaceTexture || !CardFrameTexture)
    {
        UE_LOG(LogBBBCustomizationView, Error, TEXT("换装界面美术资源缺失 请检查 UI Art 目录"));
    }
    CardSurfaceBrush.SetResourceObject(CardSurfaceTexture);
    CardSurfaceBrush.ImageSize = FVector2D(256.0f);
    CardFrameBrush.SetResourceObject(CardFrameTexture);
    CardFrameBrush.ImageSize = FVector2D(256.0f);

    ActionButtonStyle = FButtonStyle()
        .SetNormal(FSlateNoResource())
        .SetHovered(FSlateColorBrush(FLinearColor(0.45f, 0.39f, 0.26f, 0.07f)))
        .SetPressed(FSlateColorBrush(FLinearColor(0.60f, 0.36f, 0.12f, 0.14f)))
        .SetNormalPadding(FMargin(0.0f))
        .SetPressedPadding(FMargin(0.0f));

    ItemScrollBarStyle = FScrollBarStyle()
        .SetNormalThumbImage(FSlateColorBrush(FLinearColor(0.26f, 0.24f, 0.20f, 0.7f)))
        .SetHoveredThumbImage(FSlateColorBrush(FLinearColor(0.52f, 0.40f, 0.25f)))
        .SetDraggedThumbImage(FSlateColorBrush(FLinearColor(0.70f, 0.52f, 0.30f)));
    ItemScrollBoxStyle = FScrollBoxStyle()
        .SetTopShadowBrush(FSlateNoResource())
        .SetBottomShadowBrush(FSlateNoResource());
}

TSharedRef<SWidget> UBBBCharacterCustomizationView::MakeViewButton(const FName ViewName)
{
    return SNew(SButton)
        .ButtonStyle(&ActionButtonStyle)
        .ToolTip(MakeTooltip(GetViewTitle(ViewName)))
        .ContentPadding(0.0f)
        .OnClicked_Lambda([this, ViewName]()
        {
            if (Session)
            {
                CurrentView = ViewName;
                Session->SelectView(ViewName);
            }
            return FReply::Handled();
        })
        [
            SNew(SBorder)
            .BorderImage(FCoreStyle::Get().GetBrush("NoBrush"))
            .Padding(13.0f, 9.0f)
            [
                SNew(SBBBCharacterCustomizationIcon)
                .Symbol(ViewName)
                .Size(24.0f)
                .Color_Lambda([this, ViewName]() { return CurrentView == ViewName ? Accent : Ink; })
            ]
        ];
}

TSharedRef<SWidget> UBBBCharacterCustomizationView::MakeActionBar()
{
    TSharedRef<SHorizontalBox> Bar = SNew(SHorizontalBox);
    Bar->AddSlot().AutoWidth().VAlign(VAlign_Center)
    [
        SNew(SButton)
        .ButtonStyle(&ActionButtonStyle)
        .ToolTip(MakeTooltip(FText::FromString(TEXT("Esc  返回  总览状态取消未应用修改"))))
        .ContentPadding(FMargin(14.0f, 9.0f))
        .OnClicked_Lambda([this]() { return GoBack(); })
        [
            SNew(SHorizontalBox)
            + SHorizontalBox::Slot().AutoWidth()[MakeIcon(TEXT("Back"), 24.0f)]
            + SHorizontalBox::Slot().AutoWidth().VAlign(VAlign_Center).Padding(10.0f, 0.0f)
            [
                SNew(STextBlock).Text(FText::FromString(TEXT("ESC")))
                .Font(FCoreStyle::GetDefaultFontStyle("Bold", 10))
                .ColorAndOpacity(Ink)
            ]
        ]
    ];
    Bar->AddSlot().FillWidth(1.0f).VAlign(VAlign_Center).Padding(20.0f, 0.0f)
    [
        SNew(STextBlock)
        .Text_Lambda([this]() { return StatusMessage; })
        .Font(GetCustomizationFont(12))
        .ColorAndOpacity(Ink)
    ];
    for (const FName ViewName : {FName(TEXT("Full")), FName(TEXT("Head")), FName(TEXT("Legs"))})
    {
        Bar->AddSlot().AutoWidth().VAlign(VAlign_Center).Padding(3.0f, 0.0f)
        [
            MakeViewButton(ViewName)
        ];
    }
    for (const float Degrees : {-15.0f, 15.0f})
    {
        Bar->AddSlot().AutoWidth().VAlign(VAlign_Center).Padding(3.0f, 0.0f)
        [
            SNew(SButton)
            .ButtonStyle(&ActionButtonStyle)
            .ToolTip(MakeTooltip(FText::FromString(Degrees < 0.0f ? TEXT("向左旋转") : TEXT("向右旋转"))))
            .ContentPadding(FMargin(13.0f, 9.0f))
            .OnClicked_Lambda([this, Degrees]()
            {
                if (Session)
                {
                    Session->RotatePreview(Degrees);
                }
                return FReply::Handled();
            })
            [
                MakeIcon(Degrees < 0.0f ? TEXT("RotateLeft") : TEXT("RotateRight"), 24.0f)
            ]
        ];
    }
    Bar->AddSlot().AutoWidth().VAlign(VAlign_Center).Padding(24.0f, 0.0f, 0.0f, 0.0f)
    [
        SNew(SButton)
        .ButtonStyle(&ActionButtonStyle)
        .ToolTip(MakeTooltip(FText::FromString(TEXT("应用当前预览并保存"))))
        .IsEnabled_Lambda([this]() { return Session && !Session->IsPreparing(); })
        .ContentPadding(0.0f)
        .OnClicked_Lambda([this]()
        {
            if (Session)
            {
                StatusMessage = FText::FromString(Session->Apply()
                    ? TEXT("外观已应用并保存") : TEXT("保存失败  请查看日志"));
            }
            return FReply::Handled();
        })
        [
            SNew(SBorder).BorderImage(FCoreStyle::Get().GetBrush("NoBrush")).Padding(20.0f, 9.0f)
            [
                SNew(SHorizontalBox)
                + SHorizontalBox::Slot().AutoWidth()[MakeIcon(TEXT("Apply"), 24.0f, Accent)]
                + SHorizontalBox::Slot().AutoWidth().VAlign(VAlign_Center).Padding(12.0f, 0.0f)
                [
                    SNew(STextBlock).Text(FText::FromString(TEXT("应用外观")))
                    .Font(GetCustomizationFont(13)).ColorAndOpacity(Accent)
                ]
            ]
        ]
    ];
    return Bar;
}

FReply UBBBCharacterCustomizationView::GoBack()
{
    if (bShowingPatches)
    {
        OpenSlot(ExpandedSlot);
        return FReply::Handled();
    }
    if (!ExpandedSlot.IsNone())
    {
        CloseSlot();
        return FReply::Handled();
    }
    if (Session)
    {
        Session->Close();
    }
    return FReply::Handled();
}
