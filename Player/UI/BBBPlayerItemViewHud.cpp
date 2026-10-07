#include "BBBWork/UBBBNexus/Player/UI/BBBPlayerItemView.h"
#include "BBBWork/UBBBNexus/Player/BBBPlayerController.h"
#include "BBBWork/UBBBNexus/Customization/UI/BBBCharacterCustomizationStyle.h"
#include "BBBWork/UBBBNexus/Client/UI/BBBPlayerMenuStyle.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/SBoxPanel.h"

TSharedRef<SWidget> UBBBPlayerItemView::MakeGameplayHud()
{
    using namespace BBBCustomizationStyle;
    const FLinearColor Signal(1.0f, 0.27f, 0.025f);
    // 装备区独立成块 后续状态有真实数据时再沿外层纵向布局追加
    return SNew(SBox).WidthOverride(380.0f)
        [
            SNew(SVerticalBox)
            + SVerticalBox::Slot().AutoHeight()
            [
                SNew(SHorizontalBox)
                + SHorizontalBox::Slot().AutoWidth().Padding(0.0f, 0.0f, 14.0f, 0.0f)
                [
                    SNew(SBox).WidthOverride(3.0f)
                    [SNew(SImage).Image(BBBPlayerMenuStyle::GetSolidBrush()).ColorAndOpacity(Signal)]
                ]
                + SHorizontalBox::Slot().FillWidth(1.0f)
                [
                    SNew(SVerticalBox)
                    + SVerticalBox::Slot().AutoHeight()
                    [
                        SNew(STextBlock).Text(FText::FromString(TEXT("当前装备")))
                        .Font(GetCustomizationFont(10)).ColorAndOpacity(Signal)
                        .ShadowOffset(FVector2D(1.0f)).ShadowColorAndOpacity(FLinearColor::Black)
                    ]
                    + SVerticalBox::Slot().AutoHeight().Padding(0.0f, 4.0f, 0.0f, 12.0f)
                    [
                        SNew(STextBlock).Text_Lambda([this]()
                        {
                            const ABBBPlayerController *Controller = GetItemController();
                            const FText Name = Controller ? Controller->GetActiveItemName() : FText::GetEmpty();
                            return Name.IsEmpty() ? FText::FromString(TEXT("空手")) : Name;
                        })
                        .Font(GetCustomizationFont(18)).ColorAndOpacity(FLinearColor(0.92f, 0.92f, 0.88f))
                        .OverflowPolicy(ETextOverflowPolicy::Ellipsis)
                        .ShadowOffset(FVector2D(1.0f)).ShadowColorAndOpacity(FLinearColor::Black)
                    ]
                    + SVerticalBox::Slot().AutoHeight()[SAssignNew(QuickBar, SHorizontalBox)]
                ]
            ]
            + SVerticalBox::Slot().AutoHeight().Padding(17.0f, 10.0f, 0.0f, 0.0f)
            [
                SNew(STextBlock).Text(FText::FromString(TEXT("TAB  背包     /     F6  换装")))
                .Font(GetCustomizationFont(10)).ColorAndOpacity(FLinearColor(0.65f, 0.66f, 0.64f))
                .ShadowOffset(FVector2D(1.0f)).ShadowColorAndOpacity(FLinearColor::Black)
            ]
        ];
}
