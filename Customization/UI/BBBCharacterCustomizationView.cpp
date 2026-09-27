#include "BBBWork/UBBBNexus/Customization/UI/BBBCharacterCustomizationView.h"
#include "BBBWork/UBBBNexus/Customization/BBBCharacterCustomizationSession.h"
#include "Engine/TextureRenderTarget2D.h"
#include "InputCoreTypes.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Input/SSlider.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Layout/SScaleBox.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Layout/SScrollBox.h"
#include "Widgets/SBoxPanel.h"
#include "Styling/CoreStyle.h"
#include "Widgets/Text/STextBlock.h"

DEFINE_LOG_CATEGORY_STATIC(LogBBBCustomizationView, Log, All);

namespace
{
FText GetPartSlotTitle(const FName PartSlot)
{
    if (PartSlot == TEXT("Body"))
    {
        return FText::FromString(TEXT("身体"));
    }
    if (PartSlot == TEXT("Vest"))
    {
        return FText::FromString(TEXT("战术背心"));
    }
    if (PartSlot == TEXT("Attachments"))
    {
        return FText::FromString(TEXT("挂载组合"));
    }
    return FText::FromName(PartSlot);
}

FText GetViewTitle(const FName ViewName)
{
    if (ViewName == TEXT("Full"))
    {
        return FText::FromString(TEXT("全身"));
    }
    if (ViewName == TEXT("Head"))
    {
        return FText::FromString(TEXT("头部"));
    }
    return FText::FromString(TEXT("腿部"));
}
}

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
    PreviewBrush.ImageSize = FVector2D(768.0f, 1024.0f);
}

FText UBBBCharacterCustomizationView::GetSelectedItem(const FName PartSlot) const
{
    if (!Session)
    {
        return FText::GetEmpty();
    }

    const FBBBAppearanceSelection Selection = Session->GetDraft();
    if (PartSlot == TEXT("Attachments"))
    {
        return FText::FromName(Selection.Attachments);
    }
    for (const FBBBAppearancePart &Part : Selection.Parts)
    {
        if (Part.Slot == PartSlot)
        {
            return FText::FromName(Part.Item);
        }
    }
    return FText::GetEmpty();
}

TSharedRef<SWidget> UBBBCharacterCustomizationView::MakeSelectionRow(const FName PartSlot)
{
    const FLinearColor CardColor(0.035f, 0.043f, 0.055f, 1.0f);
    const FLinearColor ButtonColor(0.075f, 0.086f, 0.105f, 1.0f);

    return SNew(SBorder)
        .BorderImage(FCoreStyle::Get().GetBrush("WhiteBrush"))
        .BorderBackgroundColor(CardColor)
        .Padding(FMargin(14.0f, 10.0f))
        [
            SNew(SHorizontalBox)
            + SHorizontalBox::Slot()
            .FillWidth(1.0f)
            .VAlign(VAlign_Center)
            [
                SNew(SVerticalBox)
                + SVerticalBox::Slot()
                .AutoHeight()
                [
                    SNew(STextBlock)
                    .Text(GetPartSlotTitle(PartSlot))
                    .Font(FCoreStyle::Get().GetFontStyle("SmallFont"))
                    .ColorAndOpacity(FLinearColor(0.62f, 0.67f, 0.73f, 1.0f))
                ]
                + SVerticalBox::Slot()
                .AutoHeight()
                .Padding(0.0f, 5.0f, 0.0f, 0.0f)
                [
                    SNew(STextBlock)
                    .Text_Lambda([this, PartSlot]()
                    {
                        return GetSelectedItem(PartSlot);
                    })
                    .ColorAndOpacity(FLinearColor(0.91f, 0.92f, 0.94f, 1.0f))
                ]
            ]
            + SHorizontalBox::Slot()
            .AutoWidth()
            .VAlign(VAlign_Center)
            .Padding(10.0f, 0.0f, 3.0f, 0.0f)
            [
                SNew(SButton)
                .ButtonColorAndOpacity(ButtonColor)
                .ContentPadding(FMargin(13.0f, 8.0f))
                .Text(FText::FromString(TEXT("‹")))
                .OnClicked_Lambda([this, PartSlot]()
                {
                    if (Session)
                    {
                        Session->CycleItem(PartSlot, -1);
                    }
                    return FReply::Handled();
                })
            ]
            + SHorizontalBox::Slot()
            .AutoWidth()
            .VAlign(VAlign_Center)
            .Padding(3.0f, 0.0f, 0.0f, 0.0f)
            [
                SNew(SButton)
                .ButtonColorAndOpacity(ButtonColor)
                .ContentPadding(FMargin(13.0f, 8.0f))
                .Text(FText::FromString(TEXT("›")))
                .OnClicked_Lambda([this, PartSlot]()
                {
                    if (Session)
                    {
                        Session->CycleItem(PartSlot, 1);
                    }
                    return FReply::Handled();
                })
            ]
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
        UE_LOG(LogBBBCustomizationView, Error, TEXT("徽章缩略图材质不可用 Path=%s"), *PatchPreviewMaterial.ToString());
    }

    // 每个部位持有自己的动态材质 选择图案时不会修改另一行或原版材质资产
    Brush.SetResourceObject(Material);
    Brush.ImageSize = FVector2D(96.0f, 64.0f);
    UpdatePatchPreview(PartSlot);

    const FLinearColor CardColor(0.035f, 0.043f, 0.055f, 1.0f);
    const FLinearColor ButtonColor(0.075f, 0.086f, 0.105f, 1.0f);

    return SNew(SBorder)
        .BorderImage(FCoreStyle::Get().GetBrush("WhiteBrush"))
        .BorderBackgroundColor(CardColor)
        .Padding(FMargin(14.0f, 10.0f))
        [
            SNew(SHorizontalBox)
            + SHorizontalBox::Slot()
            .FillWidth(1.0f)
            .VAlign(VAlign_Center)
            [
                SNew(SVerticalBox)
                + SVerticalBox::Slot()
                .AutoHeight()
                [
                    SNew(STextBlock)
                    .Text(FText::FromString(PartSlot == TEXT("Body") ? TEXT("身体徽章") : TEXT("背心徽章")))
                    .ColorAndOpacity(FLinearColor(0.87f, 0.89f, 0.92f, 1.0f))
                ]
                + SVerticalBox::Slot()
                .AutoHeight()
                .Padding(0.0f, 5.0f, 0.0f, 0.0f)
                [
                    SNew(STextBlock)
                    .Text(FText::FromString(TEXT("独立图案选择")))
                    .Font(FCoreStyle::Get().GetFontStyle("SmallFont"))
                    .ColorAndOpacity(FLinearColor(0.52f, 0.58f, 0.65f, 1.0f))
                ]
            ]
            + SHorizontalBox::Slot()
            .AutoWidth()
            .VAlign(VAlign_Center)
            .Padding(8.0f, 0.0f, 4.0f, 0.0f)
            [
                SNew(SButton)
                .ButtonColorAndOpacity(ButtonColor)
                .ContentPadding(FMargin(11.0f, 7.0f))
                .Text(FText::FromString(TEXT("‹")))
                .OnClicked_Lambda([this, PartSlot]()
                {
                    if (Session && Session->CyclePatch(PartSlot, -1))
                    {
                        UpdatePatchPreview(PartSlot);
                    }
                    return FReply::Handled();
                })
            ]
            + SHorizontalBox::Slot()
            .AutoWidth()
            .VAlign(VAlign_Center)
            [
                SNew(SBorder)
                .BorderImage(FCoreStyle::Get().GetBrush("WhiteBrush"))
                .BorderBackgroundColor(FLinearColor(0.012f, 0.016f, 0.021f, 1.0f))
                .Padding(4.0f)
                [
                    SNew(SBox)
                    .WidthOverride(84.0f)
                    .HeightOverride(56.0f)
                    [
                        SNew(SImage).Image(&Brush)
                    ]
                ]
            ]
            + SHorizontalBox::Slot()
            .AutoWidth()
            .VAlign(VAlign_Center)
            .Padding(4.0f, 0.0f, 0.0f, 0.0f)
            [
                SNew(SButton)
                .ButtonColorAndOpacity(ButtonColor)
                .ContentPadding(FMargin(11.0f, 7.0f))
                .Text(FText::FromString(TEXT("›")))
                .OnClicked_Lambda([this, PartSlot]()
                {
                    if (Session && Session->CyclePatch(PartSlot, 1))
                    {
                        UpdatePatchPreview(PartSlot);
                    }
                    return FReply::Handled();
                })
            ]
        ];
}

TSharedRef<SWidget> UBBBCharacterCustomizationView::RebuildWidget()
{
    if (!Session)
    {
        return SNew(STextBlock).Text(FText::FromString(TEXT("换装会话不可用")));
    }

    TSharedRef<SVerticalBox> Items = SNew(SVerticalBox);
    Items->AddSlot()
    .AutoHeight()
    .Padding(0.0f, 4.0f, 0.0f, 8.0f)
    [
        SNew(STextBlock)
        .Text(FText::FromString(TEXT("装备部件")))
        .Font(FCoreStyle::Get().GetFontStyle("NormalFont"))
        .ColorAndOpacity(FLinearColor(0.92f, 0.93f, 0.95f, 1.0f))
    ];
    for (const FBBBAppearancePart &Part : Session->GetDraft().Parts)
    {
        Items->AddSlot()
        .AutoHeight()
        .Padding(0.0f, 3.0f)
        [
            MakeSelectionRow(Part.Slot)
        ];
    }
    Items->AddSlot()
    .AutoHeight()
    .Padding(0.0f, 3.0f)
    [
        MakeSelectionRow(TEXT("Attachments"))
    ];

    Items->AddSlot()
    .AutoHeight()
    .Padding(0.0f, 14.0f, 0.0f, 8.0f)
    [
        SNew(STextBlock)
        .Text(FText::FromString(TEXT("徽章图案")))
        .Font(FCoreStyle::Get().GetFontStyle("NormalFont"))
        .ColorAndOpacity(FLinearColor(0.92f, 0.93f, 0.95f, 1.0f))
    ];
    for (const FName PartSlot : {FName(TEXT("Body")), FName(TEXT("Vest"))})
    {
        const bool bHasPart = Session->GetDraft().Parts.ContainsByPredicate([PartSlot](const FBBBAppearancePart &Part)
        {
            return Part.Slot == PartSlot;
        });
        if (bHasPart)
        {
            Items->AddSlot()
            .AutoHeight()
            .Padding(0.0f, 3.0f)
            [
                MakePatchRow(PartSlot)
            ];
        }
    }

    TSharedRef<SHorizontalBox> Views = SNew(SHorizontalBox);
    for (const FName Name : {FName(TEXT("Full")), FName(TEXT("Head")), FName(TEXT("Legs"))})
    {
        Views->AddSlot()
        .FillWidth(1.0f)
        .Padding(3.0f, 0.0f)
        [
            SNew(SButton)
            .ButtonColorAndOpacity_Lambda([this, Name]()
            {
                const FLinearColor Active(0.82f, 0.31f, 0.055f, 1.0f);
                const FLinearColor Inactive(0.075f, 0.086f, 0.105f, 1.0f);
                return FSlateColor(CurrentView == Name ? Active : Inactive);
            })
            .ContentPadding(FMargin(10.0f, 8.0f))
            .Text(GetViewTitle(Name))
            .OnClicked_Lambda([this, Name]()
            {
                if (Session)
                {
                    CurrentView = Name;
                    Session->SelectView(Name);
                }
                return FReply::Handled();
            })
        ];
    }

    TSharedRef<SHorizontalBox> RotateControls = SNew(SHorizontalBox);
    RotateControls->AddSlot()
    .FillWidth(1.0f)
    .Padding(3.0f, 0.0f)
    [
        SNew(SButton)
        .ButtonColorAndOpacity(FLinearColor(0.075f, 0.086f, 0.105f, 1.0f))
        .ContentPadding(FMargin(10.0f, 8.0f))
        .Text(FText::FromString(TEXT("向左旋转")))
        .OnClicked_Lambda([this]()
        {
            if (Session)
            {
                Session->RotatePreview(-15.0f);
            }
            return FReply::Handled();
        })
    ];
    RotateControls->AddSlot()
    .FillWidth(1.0f)
    .Padding(3.0f, 0.0f)
    [
        SNew(SButton)
        .ButtonColorAndOpacity(FLinearColor(0.075f, 0.086f, 0.105f, 1.0f))
        .ContentPadding(FMargin(10.0f, 8.0f))
        .Text(FText::FromString(TEXT("向右旋转")))
        .OnClicked_Lambda([this]()
        {
            if (Session)
            {
                Session->RotatePreview(15.0f);
            }
            return FReply::Handled();
        })
    ];

    TSharedRef<SWidget> SurfaceControls = SNew(SBorder)
        .BorderImage(FCoreStyle::Get().GetBrush("WhiteBrush"))
        .BorderBackgroundColor(FLinearColor(0.035f, 0.043f, 0.055f, 1.0f))
        .Padding(FMargin(14.0f, 10.0f))
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
                    .Text(FText::FromString(TEXT("污渍强度")))
                    .ColorAndOpacity(FLinearColor(0.87f, 0.89f, 0.92f, 1.0f))
                ]
                + SHorizontalBox::Slot()
                .AutoWidth()
                [
                    SNew(STextBlock)
                    .Text_Lambda([this]()
                    {
                        return FText::AsPercent(Session ? Session->GetDraft().Dirt : 0.0f);
                    })
                    .ColorAndOpacity(FLinearColor(0.63f, 0.68f, 0.74f, 1.0f))
                ]
            ]
            + SVerticalBox::Slot()
            .AutoHeight()
            .Padding(0.0f, 3.0f, 0.0f, 8.0f)
            [
                SNew(SSlider)
                .SliderBarColor(FLinearColor(0.20f, 0.22f, 0.25f, 1.0f))
                .SliderHandleColor(FLinearColor(0.94f, 0.38f, 0.06f, 1.0f))
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
                    .Text(FText::FromString(TEXT("磨损强度")))
                    .ColorAndOpacity(FLinearColor(0.87f, 0.89f, 0.92f, 1.0f))
                ]
                + SHorizontalBox::Slot()
                .AutoWidth()
                [
                    SNew(STextBlock)
                    .Text_Lambda([this]()
                    {
                        return FText::AsPercent(Session ? Session->GetDraft().Weathering : 0.0f);
                    })
                    .ColorAndOpacity(FLinearColor(0.63f, 0.68f, 0.74f, 1.0f))
                ]
            ]
            + SVerticalBox::Slot()
            .AutoHeight()
            .Padding(0.0f, 3.0f, 0.0f, 0.0f)
            [
                SNew(SSlider)
                .SliderBarColor(FLinearColor(0.20f, 0.22f, 0.25f, 1.0f))
                .SliderHandleColor(FLinearColor(0.94f, 0.38f, 0.06f, 1.0f))
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

    TSharedRef<SWidget> ActionButtons = SNew(SHorizontalBox)
        + SHorizontalBox::Slot()
        .FillWidth(1.0f)
        .Padding(3.0f, 0.0f)
        [
            SNew(SButton)
            .ButtonColorAndOpacity(FLinearColor(0.91f, 0.34f, 0.045f, 1.0f))
            .ContentPadding(FMargin(12.0f, 10.0f))
            .Text(FText::FromString(TEXT("应用外观")))
            .OnClicked_Lambda([this]()
            {
                if (Session)
                {
                    Session->Apply();
                }
                return FReply::Handled();
            })
        ]
        + SHorizontalBox::Slot()
        .FillWidth(1.0f)
        .Padding(3.0f, 0.0f)
        [
            SNew(SButton)
            .ButtonColorAndOpacity(FLinearColor(0.075f, 0.086f, 0.105f, 1.0f))
            .ContentPadding(FMargin(12.0f, 10.0f))
            .Text(FText::FromString(TEXT("取消")))
            .OnClicked_Lambda([this]()
            {
                if (Session)
                {
                    Session->Close();
                }
                return FReply::Handled();
            })
        ];

    return SNew(SBorder)
        .BorderImage(FCoreStyle::Get().GetBrush("WhiteBrush"))
        .BorderBackgroundColor(FLinearColor(0.008f, 0.010f, 0.015f, 0.98f))
        .Padding(FMargin(30.0f, 20.0f))
        [
            SNew(SVerticalBox)
            + SVerticalBox::Slot()
            .AutoHeight()
            .Padding(0.0f, 0.0f, 0.0f, 14.0f)
            [
                SNew(SHorizontalBox)
                + SHorizontalBox::Slot()
                .FillWidth(1.0f)
                [
                    SNew(SVerticalBox)
                    + SVerticalBox::Slot()
                    .AutoHeight()
                    [
                        SNew(STextBlock)
                        .Text(FText::FromString(TEXT("外观配置")))
                        .Font(FCoreStyle::Get().GetFontStyle("LargeFont"))
                        .ColorAndOpacity(FLinearColor(0.95f, 0.95f, 0.96f, 1.0f))
                    ]
                    + SVerticalBox::Slot()
                    .AutoHeight()
                    .Padding(0.0f, 3.0f, 0.0f, 0.0f)
                    [
                        SNew(STextBlock)
                        .Text(FText::FromString(TEXT("本地预览   确认后应用并保存")))
                        .Font(FCoreStyle::Get().GetFontStyle("SmallFont"))
                        .ColorAndOpacity(FLinearColor(0.53f, 0.58f, 0.65f, 1.0f))
                    ]
                ]
                + SHorizontalBox::Slot()
                .AutoWidth()
                .VAlign(VAlign_Center)
                [
                    SNew(STextBlock)
                    .Text(FText::FromString(TEXT("FIELD KIT  /  01")))
                    .Font(FCoreStyle::Get().GetFontStyle("SmallFont"))
                    .ColorAndOpacity(FLinearColor(0.82f, 0.38f, 0.11f, 1.0f))
                ]
            ]
            + SVerticalBox::Slot()
            .AutoHeight()
            [
                SNew(SBox)
                .HeightOverride(1.0f)
                [
                    SNew(SBorder)
                    .BorderImage(FCoreStyle::Get().GetBrush("WhiteBrush"))
                    .BorderBackgroundColor(FLinearColor(0.27f, 0.29f, 0.32f, 0.72f))
                ]
            ]
            + SVerticalBox::Slot()
            .FillHeight(1.0f)
            .Padding(0.0f, 14.0f, 0.0f, 0.0f)
            [
                SNew(SHorizontalBox)
                + SHorizontalBox::Slot()
                .FillWidth(0.42f)
                [
                    SNew(SBorder)
                    .BorderImage(FCoreStyle::Get().GetBrush("WhiteBrush"))
                    .BorderBackgroundColor(FLinearColor(0.018f, 0.021f, 0.028f, 1.0f))
                    .Padding(FMargin(16.0f))
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
                                .Text(FText::FromString(TEXT("人物预览")))
                                .ColorAndOpacity(FLinearColor(0.90f, 0.91f, 0.93f, 1.0f))
                            ]
                            + SHorizontalBox::Slot()
                            .AutoWidth()
                            [
                                SNew(STextBlock)
                                .Text(FText::FromString(TEXT("独立场景")))
                                .Font(FCoreStyle::Get().GetFontStyle("SmallFont"))
                                .ColorAndOpacity(FLinearColor(0.54f, 0.60f, 0.68f, 1.0f))
                            ]
                        ]
                        + SVerticalBox::Slot()
                        .FillHeight(1.0f)
                        .Padding(0.0f, 12.0f)
                        [
                            SNew(SBorder)
                            .BorderImage(FCoreStyle::Get().GetBrush("WhiteBrush"))
                            .BorderBackgroundColor(FLinearColor(0.005f, 0.007f, 0.012f, 1.0f))
                            .Padding(FMargin(4.0f))
                            [
                                SNew(SScaleBox)
                                .Stretch(EStretch::ScaleToFit)
                                .HAlign(HAlign_Center)
                                .VAlign(VAlign_Center)
                                [
                                    SNew(SImage).Image(&PreviewBrush)
                                ]
                            ]
                        ]
                        + SVerticalBox::Slot()
                        .AutoHeight()
                        [
                            Views
                        ]
                        + SVerticalBox::Slot()
                        .AutoHeight()
                        .Padding(0.0f, 6.0f, 0.0f, 0.0f)
                        [
                            RotateControls
                        ]
                    ]
                ]
                + SHorizontalBox::Slot()
                .FillWidth(0.58f)
                .Padding(16.0f, 0.0f, 0.0f, 0.0f)
                [
                    SNew(SBorder)
                    .BorderImage(FCoreStyle::Get().GetBrush("WhiteBrush"))
                    .BorderBackgroundColor(FLinearColor(0.018f, 0.021f, 0.028f, 1.0f))
                    .Padding(FMargin(20.0f))
                    [
                        SNew(SVerticalBox)
                        + SVerticalBox::Slot()
                        .AutoHeight()
                        .Padding(0.0f, 0.0f, 0.0f, 12.0f)
                        [
                            SNew(SHorizontalBox)
                            + SHorizontalBox::Slot()
                            .FillWidth(1.0f)
                            [
                                SNew(STextBlock)
                                .Text(FText::FromString(TEXT("装备")))
                                .Font(FCoreStyle::Get().GetFontStyle("LargeFont"))
                                .ColorAndOpacity(FLinearColor(0.94f, 0.94f, 0.95f, 1.0f))
                            ]
                            + SHorizontalBox::Slot()
                            .AutoWidth()
                            .VAlign(VAlign_Center)
                            [
                                SNew(STextBlock)
                                .Text(FText::FromString(TEXT("APPEARANCE")))
                                .Font(FCoreStyle::Get().GetFontStyle("SmallFont"))
                                .ColorAndOpacity(FLinearColor(0.73f, 0.36f, 0.13f, 1.0f))
                            ]
                        ]
                        + SVerticalBox::Slot()
                        .FillHeight(1.0f)
                        [
                            SNew(SScrollBox)
                            + SScrollBox::Slot()
                            .Padding(0.0f, 0.0f, 4.0f, 0.0f)
                            [
                                Items
                            ]
                        ]
                        + SVerticalBox::Slot()
                        .AutoHeight()
                        .Padding(0.0f, 10.0f, 0.0f, 10.0f)
                        [
                            SurfaceControls
                        ]
                        + SVerticalBox::Slot()
                        .AutoHeight()
                        [
                            ActionButtons
                        ]
                    ]
                ]
            ]
        ];
}

FReply UBBBCharacterCustomizationView::NativeOnKeyDown(
    const FGeometry &Geometry,
    const FKeyEvent &KeyEvent)
{
    if (Session && (KeyEvent.GetKey() == EKeys::Escape || KeyEvent.GetKey() == EKeys::F6))
    {
        Session->Close();
        return FReply::Handled();
    }
    return Super::NativeOnKeyDown(Geometry, KeyEvent);
}
