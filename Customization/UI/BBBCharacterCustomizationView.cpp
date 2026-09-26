#include "BBBWork/UBBBNexus/Customization/UI/BBBCharacterCustomizationView.h"
#include "BBBWork/UBBBNexus/Customization/BBBCharacterCustomizationSession.h"
#include "Engine/TextureRenderTarget2D.h"
#include "InputCoreTypes.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Input/SSlider.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Layout/SScrollBox.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Text/STextBlock.h"

DEFINE_LOG_CATEGORY_STATIC(LogBBBCustomizationView, Log, All);

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
    PreviewBrush.ImageSize = FVector2D(1024.0f, 1024.0f);
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
    return SNew(SHorizontalBox)
        + SHorizontalBox::Slot()
        .FillWidth(0.8f)
        [
            SNew(STextBlock)
            .Text(FText::FromName(PartSlot))
        ]
        + SHorizontalBox::Slot()
        .AutoWidth()
        [
            SNew(SButton)
            .Text(FText::FromString(TEXT("◀")))
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
        .FillWidth(1.4f)
        [
            SNew(STextBlock)
            .Text_Lambda([this, PartSlot]()
            {
                return GetSelectedItem(PartSlot);
            })
        ]
        + SHorizontalBox::Slot()
        .AutoWidth()
        [
            SNew(SButton)
            .Text(FText::FromString(TEXT("▶")))
            .OnClicked_Lambda([this, PartSlot]()
            {
                if (Session)
                {
                    Session->CycleItem(PartSlot, 1);
                }
                return FReply::Handled();
            })
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

    return SNew(SHorizontalBox)
        + SHorizontalBox::Slot()
        .FillWidth(1.0f)
        .VAlign(VAlign_Center)
        [
            SNew(STextBlock)
            .Text(FText::FromString(PartSlot == TEXT("Body") ? TEXT("身体徽章") : TEXT("背心徽章")))
        ]
        + SHorizontalBox::Slot()
        .AutoWidth()
        [
            SNew(SButton)
            .Text(FText::FromString(TEXT("◀")))
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
        [
            SNew(SBox)
            .WidthOverride(96.0f)
            .HeightOverride(64.0f)
            [
                SNew(SImage).Image(&Brush)
            ]
        ]
        + SHorizontalBox::Slot()
        .AutoWidth()
        [
            SNew(SButton)
            .Text(FText::FromString(TEXT("▶")))
            .OnClicked_Lambda([this, PartSlot]()
            {
                if (Session && Session->CyclePatch(PartSlot, 1))
                {
                    UpdatePatchPreview(PartSlot);
                }
                return FReply::Handled();
            })
        ];
}

TSharedRef<SWidget> UBBBCharacterCustomizationView::RebuildWidget()
{
    if (!Session)
    {
        return SNew(STextBlock).Text(FText::FromString(TEXT("换装会话不可用")));
    }

    TSharedRef<SVerticalBox> Items = SNew(SVerticalBox);
    for (const FBBBAppearancePart &Part : Session->GetDraft().Parts)
    {
        Items->AddSlot()
        .AutoHeight()
        .Padding(4.0f)
        [
            MakeSelectionRow(Part.Slot)
        ];
    }
    Items->AddSlot()
    .AutoHeight()
    .Padding(4.0f)
    [
        MakeSelectionRow(TEXT("Attachments"))
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
            .Padding(4.0f)
            [
                MakePatchRow(PartSlot)
            ];
        }
    }

    TSharedRef<SVerticalBox> Controls = SNew(SVerticalBox);
    Controls->AddSlot()
    .AutoHeight()
    .Padding(8.0f)
    [
        SNew(STextBlock).Text(FText::FromString(TEXT("人物换装")))
    ];
    Controls->AddSlot()
    .FillHeight(1.0f)
    [
        SNew(SScrollBox)
        + SScrollBox::Slot()
        [
            Items
        ]
    ];
    Controls->AddSlot()
    .AutoHeight()
    .Padding(8.0f)
    [
        SNew(STextBlock).Text(FText::FromString(TEXT("污渍")))
    ];
    Controls->AddSlot()
    .AutoHeight()
    .Padding(8.0f)
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
    ];
    Controls->AddSlot()
    .AutoHeight()
    .Padding(8.0f)
    [
        SNew(STextBlock).Text(FText::FromString(TEXT("磨损")))
    ];
    Controls->AddSlot()
    .AutoHeight()
    .Padding(8.0f)
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
    ];

    TSharedRef<SHorizontalBox> Views = SNew(SHorizontalBox);
    for (const FName Name : {FName(TEXT("Full")), FName(TEXT("Head")), FName(TEXT("Legs"))})
    {
        Views->AddSlot()
        .AutoWidth()
        .Padding(3.0f)
        [
            SNew(SButton)
            .Text(FText::FromName(Name))
            .OnClicked_Lambda([this, Name]()
            {
                if (Session)
                {
                    Session->SelectView(Name);
                }
                return FReply::Handled();
            })
        ];
    }
    Controls->AddSlot()
    .AutoHeight()
    .Padding(8.0f)
    [
        Views
    ];
    Controls->AddSlot()
    .AutoHeight()
    .Padding(8.0f)
    [
        SNew(SHorizontalBox)
        + SHorizontalBox::Slot()
        .AutoWidth()
        [
            SNew(SButton)
            .Text(FText::FromString(TEXT("左转")))
            .OnClicked_Lambda([this]()
            {
                if (Session)
                {
                    Session->RotatePreview(-15.0f);
                }
                return FReply::Handled();
            })
        ]
        + SHorizontalBox::Slot()
        .AutoWidth()
        [
            SNew(SButton)
            .Text(FText::FromString(TEXT("右转")))
            .OnClicked_Lambda([this]()
            {
                if (Session)
                {
                    Session->RotatePreview(15.0f);
                }
                return FReply::Handled();
            })
        ]
    ];
    Controls->AddSlot()
    .AutoHeight()
    .Padding(8.0f)
    [
        SNew(SHorizontalBox)
        + SHorizontalBox::Slot()
        .AutoWidth()
        [
            SNew(SButton)
            .Text(FText::FromString(TEXT("应用")))
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
        .AutoWidth()
        [
            SNew(SButton)
            .Text(FText::FromString(TEXT("取消")))
            .OnClicked_Lambda([this]()
            {
                if (Session)
                {
                    Session->Close();
                }
                return FReply::Handled();
            })
        ]
    ];

    return SNew(SBorder)
        .Padding(12.0f)
        [
            SNew(SHorizontalBox)
            + SHorizontalBox::Slot()
            .FillWidth(0.7f)
            [
                SNew(SImage).Image(&PreviewBrush)
            ]
            + SHorizontalBox::Slot()
            .FillWidth(0.3f)
            .Padding(12.0f)
            [
                Controls
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
