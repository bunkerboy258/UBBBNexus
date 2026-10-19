#if WITH_DEV_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Config/BBBMonsterNavigationProfile.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Config/BBBMonsterNavigationSettings.h"

/** 验证个体路线保持稳定并遵守配置的分布范围 */
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBBBMonsterNavigationProfileTest, "UBBB.Mass.ZombieNavigationProfile",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::CommandletContext | EAutomationTestFlags::EngineFilter)

bool FBBBMonsterNavigationProfileTest::RunTest(const FString& Parameters)
{
    FBBBMonsterNavigationProfile Profile;
    TestTrue(TEXT("默认路线配置有效"), Profile.IsValid());
    TestTrue(TEXT("默认世界预算有效"), GetDefault<UBBBMonsterNavigationSettings>()->IsValid());
    int32 Direct = 0;
    int32 Positive = 0;
    int32 Negative = 0;
    int32 LeftLane = 0;
    int32 RightLane = 0;
    bool bStable = true;
    bool bBounded = true;
    for (uint32 Seed = 1; Seed <= 4096; ++Seed)
    {
        float Arc = 0.0f;
        float Lane = 0.0f;
        Profile.Sample(Seed, Arc, Lane);
        float RepeatedArc = 0.0f;
        float RepeatedLane = 0.0f;
        Profile.Sample(Seed, RepeatedArc, RepeatedLane);
        bStable &= Arc == RepeatedArc && Lane == RepeatedLane;
        bBounded &= FMath::IsFinite(Arc) && FMath::IsFinite(Lane) &&
            (Arc == 0.0f || (FMath::Abs(Arc) >= Profile.ArcStrengthRange.X && FMath::Abs(Arc) <= Profile.ArcStrengthRange.Y)) &&
            Lane >= Profile.LaneOffsetRange.X && Lane <= Profile.LaneOffsetRange.Y;
        Direct += Arc == 0.0f && Lane == 0.0f;
        Positive += Arc > 0.0f;
        Negative += Arc < 0.0f;
        LeftLane += Lane < 0.0f;
        RightLane += Lane > 0.0f;
    }
    TestTrue(TEXT("相同出生种子重复规划得到相同形状"), bStable);
    TestTrue(TEXT("所有抽样都在有效配置范围内"), bBounded);
    TestTrue(TEXT("直接路线比例接近配置并同时存在两种绕行方向"), Direct > 650 && Direct < 1000 && Positive > 1300 && Negative > 1300);
    TestTrue(TEXT("默认横向偏移覆盖左右方向"), LeftLane > 1300 && RightLane > 1300);
    Profile.DirectPathChance = 1.0f;
    float Arc = 1.0f;
    float Lane = 1.0f;
    Profile.Sample(42, Arc, Lane);
    TestTrue(TEXT("直接接近比例为一时关闭全部塑形"), Arc == 0.0f && Lane == 0.0f);
    Profile.ArcStrengthRange = FVector2D(1.0, 0.5);
    TestFalse(TEXT("拒绝反向范围"), Profile.IsValid());
    Profile.ArcStrengthRange = FVector2D(0.2, 0.7);
    Profile.NumMidPoints = 7;
    TestFalse(TEXT("拒绝超出插件范围的形状参数"), Profile.IsValid());
    return true;
}

#endif
