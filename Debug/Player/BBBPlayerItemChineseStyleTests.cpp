#if WITH_DEV_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"
#include "BBBWork/UBBBNexus/Player/UI/BBBPlayerItemView.h"
#include "BBBWork/UBBBNexus/Player/UI/BBBPlayerItemStyle.h"
#include "Engine/UserInterfaceSettings.h"
#include "Materials/Material.h"
#include "Widgets/Text/STextBlock.h"

/** 检查真实背包控件的主界面文案与字体材质 保留物品详情字体 */
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBBBPlayerItemChineseStyleTest, "BBB.Player.ItemView.ChineseStyle",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FBBBPlayerItemChineseStyleTest::RunTest(const FString &Parameters)
{
    UBBBPlayerItemView *View = NewObject<UBBBPlayerItemView>();
    TestTrue(TEXT("背包界面初始化"), View->Initialize());
    const TSharedRef<SWidget> Root = View->TakeWidget();
    const TSet<FString> Required = {
        TEXT("背包"), TEXT("装备"), TEXT("穿戴物"), TEXT("消耗品"),
        TEXT("杂物"), TEXT("快捷栏"), TEXT("穿戴"), TEXT("返回")};
    const TSet<FString> Removed = {
        TEXT("Bag"), TEXT("QUICKBAR"), TEXT("GEAR"), TEXT("EXIT"),
        TEXT("EQUIPMENT"), TEXT("WEARABLES"), TEXT("CONSUMABLES"), TEXT("MISC")};
    TSet<FString> Found;
    const FSlateFontInfo Art = BBBItemUI::ChineseArtFont(22);
    TFunction<void(const TSharedRef<SWidget> &)> Visit;
    Visit = [&](const TSharedRef<SWidget> &Widget)
    {
        if (Widget->GetType() == TEXT("STextBlock"))
        {
            const TSharedRef<STextBlock> Text = StaticCastSharedRef<STextBlock>(Widget);
            const FString Label = Text->GetText().ToString();
            TestFalse(TEXT("主界面不残留旧英文导航"), Removed.Contains(Label));
            if (Required.Contains(Label))
            {
                Found.Add(Label);
                TestTrue(FString::Printf(TEXT("中文导航使用艺术字材质 %s"), *Label),
                    Text->GetFont().FontMaterial == Art.FontMaterial);
            }
        }
        FChildren *Children = Widget->GetChildren();
        for (int32 Index = 0; Index < Children->Num(); ++Index)
        {
            Visit(Children->GetChildAt(Index));
        }
    };
    Visit(Root);
    TestEqual(TEXT("真实控件覆盖全部中文主导航"), Found.Num(), Required.Num());
    const UFontFace *Face = Cast<UFontFace>(Art.GetCompositeFont()->DefaultTypeface.Fonts[0].Font.GetFontFaceAsset());
    TestTrue(TEXT("主字体启用距离场"), Face && Face->bEnableDistanceFieldRendering);
    TestTrue(TEXT("项目启用距离场字形光栅化"), GetDefault<UUserInterfaceSettings>()->bEnableDistanceFieldFontRasterization);
    const UMaterialInterface *Material = Cast<UMaterialInterface>(Art.FontMaterial);
    TestTrue(TEXT("字体材质属于界面域"), Material && Material->GetMaterial()->MaterialDomain == MD_UI);
    TestTrue(TEXT("字体材质透明混合"), Material && Material->GetBlendMode() == BLEND_Translucent);
    TestNull(TEXT("物品说明保留原字体"), BBBItemUI::Font(14).FontMaterial.Get());
    TestNull(TEXT("武器名称保留原艺术字"), BBBItemUI::ArtFont(52).FontMaterial.Get());
    return true;
}

#endif
