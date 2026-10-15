#include "CoreMinimal.h"

#if WITH_DEV_AUTOMATION_TESTS && WITH_EDITOR

#include "Misc/AutomationTest.h"
#include "BBBWork/UBBBNexus/Player/UI/BBBCombatAmmoRail.h"
#include "Layout/Geometry.h"
#include "Rendering/DrawElements.h"

/** 验证真实绘制元素保持准星右下短弧 并覆盖空弹与连续填充 */
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBBBCombatAmmoRailTest, "BBB.Player.Hud.AmmoRail",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FBBBCombatAmmoRailTest::RunTest(const FString &Parameters)
{
    const FLinearColor AmmoColor(1.0f, 1.0f, 1.0f, 0.95f);
    for (const float Height : {720.0f, 1080.0f, 2160.0f})
    {
        const FVector2f Size(Height * 16.0f / 9.0f, Height);
        const FGeometry Geometry = FGeometry::MakeRoot(Size, FSlateLayoutTransform());
        const FVector2f Center = Size * 0.5f;
        const float Radius = 50.0f * FMath::Clamp(Height / 1080.0f, 0.55f, 1.6f);
        FSlateWindowElementList Elements(nullptr);
        BBBCombatAmmoRail::Paint(Elements, Geometry, 0, 3, 6, false, AmmoColor);
        const auto &Lines = Elements.GetUncachedDrawElements().Get<(uint8)EElementType::ET_Line>();
        TestEqual(TEXT("每发子弹都有可见轨道与描边"), Lines.Num(), 12);
        int32 Filled = 0;
        for (const FSlateLineElement &Line : Lines)
        {
            Filled += Line.GetTint().Equals(AmmoColor) ? 1 : 0;
            for (const FVector2f &Point : Line.GetPoints())
            {
                const FVector2f Offset = Point - Center;
                TestTrue(TEXT("细轨道保持准星右下角"), Offset.X > 0.0f && Offset.Y > 0.0f);
                TestTrue(TEXT("细轨道与准星保持约定距离"), FMath::IsNearlyEqual(Offset.Size(), Radius, 0.02f));
                const float Angle = FMath::RadiansToDegrees(FMath::Atan2(Offset.Y, Offset.X));
                TestTrue(TEXT("细轨道仅覆盖二十至八十度"), Angle >= 19.9f && Angle <= 80.1f);
            }
        }
        TestEqual(TEXT("六发容量剩余三发仅填亮三格"), Filled, 3);
    }
    const FGeometry Geometry = FGeometry::MakeRoot(FVector2f(1920.0f, 1080.0f), FSlateLayoutTransform());
    FSlateWindowElementList Empty(nullptr);
    BBBCombatAmmoRail::Paint(Empty, Geometry, 0, 0, 6, true, AmmoColor);
    TestEqual(TEXT("连续弹药耗尽仍保留整条暗轨"),
        Empty.GetUncachedDrawElements().Get<(uint8)EElementType::ET_Line>().Num(), 2);
    FSlateWindowElementList Half(nullptr);
    BBBCombatAmmoRail::Paint(Half, Geometry, 0, 3, 6, true, AmmoColor);
    const auto &HalfLines = Half.GetUncachedDrawElements().Get<(uint8)EElementType::ET_Line>();
    if (TestEqual(TEXT("连续弹药同时绘制暗轨与剩余量"), HalfLines.Num(), 4))
    {
        const FVector2f End = HalfLines[3].GetPoints().Last() - FVector2f(960.0f, 540.0f);
        TestTrue(TEXT("半仓连续填充终点为五十度"),
            FMath::IsNearlyEqual(FMath::RadiansToDegrees(FMath::Atan2(End.Y, End.X)), 50.0f, 0.1f));
    }
    FSlateWindowElementList NoAmmo(nullptr);
    BBBCombatAmmoRail::Paint(NoAmmo, Geometry, 0, 0, 0, false, AmmoColor);
    TestEqual(TEXT("无弹药容量不绘制轨道"),
        NoAmmo.GetUncachedDrawElements().Get<(uint8)EElementType::ET_Line>().Num(), 0);
    return true;
}

#endif
