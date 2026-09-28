#include "BBBWork/UBBBNexus/Customization/UI/BBBCharacterCustomizationView.h"
#include "BBBWork/UBBBNexus/Customization/BBBCharacterCustomizationSession.h"
#include "Engine/Texture2D.h"
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
#include "Styling/CoreStyle.h"
#include "Widgets/Text/STextBlock.h"

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

    if (const TSharedPtr<FSlateBrush> *ExistingBrush = ItemThumbnailBrushes.Find(ItemId))
    {
        return ExistingBrush->Get();
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

    UTexture2D *Texture = Item.Thumbnail.LoadSynchronous();
    if (!Texture)
    {
        UE_LOG(LogBBBCustomizationView, Error, TEXT("外观缩略图加载失败 Item=%s Path=%s"),
            *ItemId.ToString(), *Item.Thumbnail.ToString());
        return nullptr;
    }

    TSharedPtr<FSlateBrush> Brush = MakeShared<FSlateBrush>();
    Brush->SetResourceObject(Texture);
    Brush->ImageSize = FVector2D(Texture->GetSizeX(), Texture->GetSizeY());
    ItemThumbnails.Add(ItemId, Texture);
    ItemThumbnailBrushes.Add(ItemId, Brush);
    return Brush.Get();
}

void UBBBCharacterCustomizationView::RefreshSlotCardThumbnails()
{
    if (!Session)
    {
        return;
    }

    for (TPair<FName, TSharedPtr<SBox>> &Entry : SlotThumbnailBoxes)
    {
        if (!Entry.Value.IsValid())
        {
            continue;
        }

        const FName ItemId = GetSelectedItemId(Entry.Key);
        const FSlateBrush *Thumbnail = GetItemBrush(ItemId);
        if (Thumbnail)
        {
            Entry.Value->SetContent(SNew(SScaleBox).Stretch(EStretch::ScaleToFit)[SNew(SImage).Image(Thumbnail)]);
            continue;
        }

        FBBBAppearanceItem Item;
        const bool bHasItem = !ItemId.IsNone() && Session->GetItem(ItemId, Item);
        const bool bEmptyItem = bHasItem && Item.Mesh.IsNull() && Item.Attachments.IsEmpty();
        const bool bUnconfiguredAttachment = Entry.Key == TEXT("Attachments") && bEmptyItem
            && ItemId != TEXT("Attachments_None");
        FText Placeholder = FText::FromString(TEXT("配置不可用"));
        if (bHasItem)
        {
            Placeholder = FText::FromString(TEXT("缩略图缺失"));
        }
        if (ItemId.IsNone() || bEmptyItem)
        {
            Placeholder = FText::FromString(TEXT("无部件"));
        }
        if (bUnconfiguredAttachment)
        {
            Placeholder = FText::FromString(TEXT("配置未完成"));
        }
        Entry.Value->SetContent(
            SNew(STextBlock)
            .Text(Placeholder)
            .Font(GetCustomizationFont(18))
            .Justification(ETextJustify::Center)
            .ColorAndOpacity(FLinearColor(0.84f, 0.68f, 0.52f, 1.0f)));
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
    Contents->AddSlot()
    .AutoHeight()
    .Padding(0.0f, 0.0f, 0.0f, 9.0f)
    [
        SNew(SVerticalBox)
        + SVerticalBox::Slot()
        .AutoHeight()
        [
            SNew(STextBlock)
            .Text(FText::FromString(bLeft ? TEXT("上身与护具") : TEXT("下身与携行")))
            .Font(GetCustomizationFont(22))
            .ColorAndOpacity(FLinearColor(0.92f, 0.83f, 0.70f, 1.0f))
        ]
        + SVerticalBox::Slot()
        .AutoHeight()
        .Padding(0.0f, 2.0f, 0.0f, 0.0f)
        [
            SNew(STextBlock)
            .Text(FText::FromString(TEXT("当前穿戴部位")))
            .Font(GetCustomizationFont(18))
            .ColorAndOpacity(FLinearColor(0.68f, 0.70f, 0.72f, 1.0f))
        ]
    ];

    Contents->AddSlot()
    .AutoHeight()
    .Padding(0.0f, 0.0f, 0.0f, 8.0f)
    [
        SNew(SBox)
        .HeightOverride(1.0f)
        [
            SNew(SBorder)
            .BorderImage(FCoreStyle::Get().GetBrush("WhiteBrush"))
            .BorderBackgroundColor(FLinearColor(0.35f, 0.39f, 0.46f, 0.72f))
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
            + SVerticalBox::Slot()
            .AutoHeight()
            [
                SNew(SButton)
                .ButtonStyle(&ActionButtonStyle)
                .ButtonColorAndOpacity(FLinearColor(0.09f, 0.10f, 0.12f, 0.9f))
                .ContentPadding(FMargin(8.0f, 5.0f))
                .TextStyle(&GetCustomizationButtonTextStyle())
                .Text(FText::FromString(TEXT("表面状态")))
                .OnClicked_Lambda([this]()
                {
                    bSurfaceExpanded = !bSurfaceExpanded;
                    InvalidateLayoutAndVolatility();
                    return FReply::Handled();
                })
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
        .Padding(FMargin(14.0f, 10.0f))
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
    Brush.ImageSize = FVector2D(72.0f, 54.0f);
    UpdatePatchPreview(PartSlot);

    if (!PatchAtlas)
    {
        PatchAtlas = LoadObject<UTexture2D>(nullptr,
            TEXT("/Game/UkraineSoldier/Textures/Flags/T_Flags_BC.T_Flags_BC"));
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
                .Font(GetCustomizationFont(20))
                .ColorAndOpacity(FLinearColor(0.91f, 0.85f, 0.77f, 1.0f))
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
                .BorderImage(FCoreStyle::Get().GetBrush("WhiteBrush"))
                .BorderBackgroundColor_Lambda([this, PartSlot, Index]()
                {
                    const FLinearColor Selected(0.90f, 0.49f, 0.20f, 1.0f);
                    const FLinearColor Normal(0.20f, 0.22f, 0.25f, 0.9f);
                    return GetSelectedPatchIndex(PartSlot) == Index ? Selected : Normal;
                })
                .Padding(FMargin(2.0f))
                [
                    SNew(SBox)
                    .WidthOverride(46.0f)
                    .HeightOverride(46.0f)
                    [
                        PatchAtlas
                            ? StaticCastSharedRef<SWidget>(SNew(SImage).Image(&PatchBrushes[Index]))
                            : StaticCastSharedRef<SWidget>(SNew(STextBlock)
                                .Text(FText::AsNumber(Index + 1))
                                .Font(GetCustomizationFont(16))
                                .Justification(ETextJustify::Center))
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
        .BorderImage(FCoreStyle::Get().GetBrush("WhiteBrush"))
        .BorderBackgroundColor(FLinearColor(0.035f, 0.041f, 0.050f, 0.88f))
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
                    .Font(GetCustomizationFont(18))
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
                    .Font(GetCustomizationFont(18))
                    .ColorAndOpacity(FLinearColor(0.67f, 0.69f, 0.71f, 1.0f))
                ]
            ]
            + SVerticalBox::Slot()
            .AutoHeight()
            .Padding(0.0f, 0.0f, 0.0f, 5.0f)
            [
                SNew(SSlider)
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
                    .Font(GetCustomizationFont(18))
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
                    .Font(GetCustomizationFont(18))
                    .ColorAndOpacity(FLinearColor(0.67f, 0.69f, 0.71f, 1.0f))
                ]
            ]
            + SVerticalBox::Slot()
            .AutoHeight()
            [
                SNew(SSlider)
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
    if (!Session || Session->GetItems(PartSlot).IsEmpty())
    {
        UE_LOG(LogBBBCustomizationView, Warning, TEXT("无法打开没有候选款式的部位 Slot=%s"), *PartSlot.ToString());
        return;
    }

    ExpandedSlot = PartSlot;
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
        .ButtonColorAndOpacity(FLinearColor(0.09f, 0.11f, 0.14f, 0.94f))
        .ContentPadding(FMargin(7.0f, 4.0f))
        .TextStyle(&GetCustomizationButtonTextStyle())
        .Text(FText::FromString(TEXT("返回款式")))
        .OnClicked_Lambda([this, PartSlot]()
        {
            OpenSlot(PartSlot);
            return FReply::Handled();
        })
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
    SlotThumbnailBoxes.Reset();
    PatchAtlas = LoadObject<UTexture2D>(nullptr,
        TEXT("/Game/UkraineSoldier/Textures/Flags/T_Flags_BC.T_Flags_BC"));
    if (!PatchAtlas)
    {
        UE_LOG(LogBBBCustomizationView, Error, TEXT("身体与背心徽章图集加载失败"));
    }

    TSharedRef<SConstraintCanvas> Layout = SNew(SConstraintCanvas);
    Layout->AddSlot()
    .Anchors(FAnchors(0.0f, 0.0f, 1.0f, 1.0f))
    [
        SNew(SImage).Image(&PreviewBrush)
    ];

    Layout->AddSlot()
    .Anchors(FAnchors(0.0f, 0.0f, 1.0f, 1.0f))
    [
        SNew(SBorder)
        .BorderImage(FCoreStyle::Get().GetBrush("WhiteBrush"))
        .BorderBackgroundColor(FLinearColor(0.005f, 0.008f, 0.014f, 0.13f))
    ];

    Layout->AddSlot()
    .Anchors(FAnchors(0.075f, 0.13f, 0.315f, 0.86f))
    .Offset(FMargin(0.0f))
    [
        MakeSidePanel(true)
    ];

    Layout->AddSlot()
    .Anchors(FAnchors(0.685f, 0.13f, 0.925f, 0.86f))
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

    return Layout;
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
        .SetHovered(FSlateColorBrush(FLinearColor(0.15f, 0.13f, 0.10f, 0.55f)))
        .SetPressed(FSlateColorBrush(FLinearColor(0.30f, 0.23f, 0.14f, 0.75f)))
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
        .ContentPadding(FMargin(16.0f, 6.0f))
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
            SNew(STextBlock)
            .Text(GetViewTitle(ViewName))
            .Font(GetCustomizationFont(18))
            .ColorAndOpacity_Lambda([this, ViewName]()
            {
                return CurrentView == ViewName
                    ? FSlateColor(FLinearColor(0.88f, 0.64f, 0.32f))
                    : FSlateColor(FLinearColor(0.66f, 0.65f, 0.61f));
            })
        ];
}

TSharedRef<SWidget> UBBBCharacterCustomizationView::MakeActionBar()
{
    TSharedRef<SHorizontalBox> Bar = SNew(SHorizontalBox);
    Bar->AddSlot().AutoWidth().VAlign(VAlign_Center)
    [
        SNew(SButton)
        .ButtonStyle(&ActionButtonStyle)
        .ContentPadding(FMargin(16.0f, 6.0f))
        .TextStyle(&GetCustomizationButtonTextStyle())
        .Text(FText::FromString(TEXT("Esc  返回")))
        .OnClicked_Lambda([this]()
        {
            return GoBack();
        })
    ];
    Bar->AddSlot().FillWidth(1.0f).VAlign(VAlign_Center).Padding(16.0f, 0.0f)
    [
        SNew(STextBlock)
        .Text_Lambda([this]() { return StatusMessage; })
        .Font(GetCustomizationFont(16))
        .ColorAndOpacity(FLinearColor(0.66f, 0.64f, 0.59f))
        .AutoWrapText(true)
    ];
    for (const FName ViewName : {FName(TEXT("Full")), FName(TEXT("Head")), FName(TEXT("Legs"))})
    {
        Bar->AddSlot().AutoWidth().VAlign(VAlign_Center)[MakeViewButton(ViewName)];
    }
    for (const float Degrees : {-15.0f, 15.0f})
    {
        Bar->AddSlot().AutoWidth().VAlign(VAlign_Center)
        [
            SNew(SButton)
            .ButtonStyle(&ActionButtonStyle)
            .ContentPadding(FMargin(14.0f, 6.0f))
            .TextStyle(&GetCustomizationButtonTextStyle())
            .Text(FText::FromString(Degrees < 0.0f ? TEXT("转左") : TEXT("转右")))
            .OnClicked_Lambda([this, Degrees]()
            {
                if (Session)
                {
                    Session->RotatePreview(Degrees);
                }
                return FReply::Handled();
            })
        ];
    }
    Bar->AddSlot().AutoWidth().VAlign(VAlign_Center).Padding(24.0f, 0.0f, 0.0f, 0.0f)
    [
        SNew(SButton)
        .ButtonStyle(&ActionButtonStyle)
        .ContentPadding(FMargin(20.0f, 6.0f))
        .OnClicked_Lambda([this]()
        {
            if (Session)
            {
                const bool bApplied = Session->Apply();
                StatusMessage = FText::FromString(bApplied
                    ? TEXT("外观已应用并保存") : TEXT("处理失败  请查看日志"));
            }
            return FReply::Handled();
        })
        [
            SNew(STextBlock)
            .Text(FText::FromString(TEXT("应用外观")))
            .Font(GetCustomizationFont(20))
            .ColorAndOpacity(FLinearColor(0.88f, 0.64f, 0.32f))
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
