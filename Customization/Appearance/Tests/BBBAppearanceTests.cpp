#include "BBBWork/UBBBNexus/Customization/Appearance/BBBAppearanceComponent.h"
#include "Misc/AutomationTest.h"

#if WITH_DEV_AUTOMATION_TESTS

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBBBAppearanceStructureTest, "BBB.Appearance.SelectionStructure",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FBBBAppearanceStructureTest::RunTest(const FString &Parameters)
{
    FBBBAppearanceSelection Selection;
    TestFalse(TEXT("空组合不能进入网络"), Selection.IsValid());
    FBBBAppearancePart Body;
    Body.Slot = TEXT("Body");
    Body.Item = TEXT("Body_Shirt");
    Selection.Parts.Add(Body);
    TestTrue(TEXT("完整记录满足结构要求"), Selection.IsValid());
    Selection.Parts.Add(Body);
    TestFalse(TEXT("拒绝重复部位"), Selection.IsValid());
    Selection.Parts.Pop();
    Selection.Dirt = 2.0f;
    TestFalse(TEXT("拒绝越界污渍参数"), Selection.IsValid());
    Selection.Dirt = 0.5f;
    Selection.Parts[0].Colors.SetNum(9);
    TestFalse(TEXT("拒绝过多颜色参数"), Selection.IsValid());
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBBBAppearanceMatchingTest, "BBB.Appearance.ExplicitLegMatching",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FBBBAppearanceMatchingTest::RunTest(const FString &Parameters)
{
    UBBBAppearanceComponent *Component = NewObject<UBBBAppearanceComponent>();
    UDataTable *Table = NewObject<UDataTable>(Component);
    Table->RowStruct = FBBBAppearanceItem::StaticStruct();
    Component->Catalog = Table;

    FBBBAppearanceItem Tucked;
    Tucked.Slot = TEXT("Legs");
    Tucked.LegStyle = TEXT("Tucked");
    Tucked.AlternateLegItem = TEXT("Trousers_Outside");
    Table->AddRow(TEXT("Trousers_Inside"), Tucked);
    FBBBAppearanceItem Outside = Tucked;
    Outside.LegStyle = TEXT("Outside");
    Outside.AlternateLegItem = TEXT("Trousers_Inside");
    Table->AddRow(TEXT("Trousers_Outside"), Outside);
    FBBBAppearanceItem Boots;
    Boots.Slot = TEXT("Boots");
    Boots.RequiredLegStyle = TEXT("Outside");
    Table->AddRow(TEXT("Boots_Low"), Boots);

    FBBBAppearancePart LegsPart;
    LegsPart.Slot = TEXT("Legs");
    LegsPart.Item = TEXT("Trousers_Inside");
    FBBBAppearancePart BootsPart;
    BootsPart.Slot = TEXT("Boots");
    BootsPart.Item = TEXT("Boots_Low");
    Component->DefaultSelection.Parts = {LegsPart, BootsPart};
    FBBBAppearanceSelection Selection = Component->DefaultSelection;
    TestTrue(TEXT("由配置完成裤靴匹配"), Component->PrepareSelection(Selection));
    TestEqual(TEXT("选择显式关联的裤腿 不依赖行顺序"), Selection.Parts[0].Item, FName(TEXT("Trousers_Outside")));
    TestEqual(TEXT("默认配置不被草稿修改"), Component->DefaultSelection.Parts[0].Item, FName(TEXT("Trousers_Inside")));
    return true;
}

#endif
