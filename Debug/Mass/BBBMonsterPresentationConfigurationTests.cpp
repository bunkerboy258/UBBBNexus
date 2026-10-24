#if WITH_DEV_AUTOMATION_TESTS && WITH_EDITOR

#include "Misc/AutomationTest.h"
#include "UObject/UnrealType.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Config/BBBMonsterDefinition.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Config/BBBMonsterPresentationDefinition.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Config/BBBMonsterGoreDefinition.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Config/BBBMonsterBloodPresentationDefinition.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Config/BBBMonsterVariationDefinition.h"
#include "HitReactProfile.h"

/** 验证两份主配置的真实所有权 数值边界与测试隔离 不评价画面质量 */
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBBBMonsterPresentationConfigurationTest, "UBBB.Mass.MonsterConfiguration",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FBBBMonsterPresentationConfigurationTest::RunTest(const FString&)
{
    for (const FName Field : {FName(TEXT("Gore")), FName(TEXT("BloodPresentation")), FName(TEXT("SoundPresentation")),
        FName(TEXT("AnimationStrideScale")), FName(TEXT("DeathAnimationDuration")), FName(TEXT("AnimationHitFraction"))})
    {
        TestNull(TEXT("逻辑配置无旧表现入口"), FindFProperty<FProperty>(UBBBMonsterDefinition::StaticClass(), Field));
    }
    const TCHAR* Names[] = {TEXT("Connor"), TEXT("Jiho"), TEXT("Michael"), TEXT("Morita"), TEXT("Renzo"),
        TEXT("Sakurada"), TEXT("Alice"), TEXT("Kiyo"), TEXT("Setsuko"), TEXT("Yuina")};
    for (int32 Index = 0; Index < UE_ARRAY_COUNT(Names); ++Index)
    {
        const FString Path = FString::Printf(TEXT("/Game/_Project/Mass/Monster/Zombie/%s/Variants/%s/DA_BBBZombie%sDefinition"),
            Index < 6 ? TEXT("Male") : TEXT("Female"), Names[Index], Names[Index]);
        auto* Logic = LoadObject<UBBBMonsterDefinition>(nullptr, *Path);
        if (!TestNotNull(TEXT("正式逻辑配置"), Logic) || !TestNotNull(TEXT("正式表现配置"), Logic->Presentation.Get()))
        {
            continue;
        }
        auto* Presentation = Logic->Presentation.Get();
        TestTrue(Path + TEXT(" 逻辑完整"), Logic->IsValid());
        TestTrue(Path + TEXT(" 表现完整"), Presentation->IsValid());
        TestTrue(TEXT("出生规则内联于逻辑"), Logic->Variation->GetOuter() == Logic);
        TestTrue(TEXT("伤口内联于表现"), Presentation->Gore->GetOuter() == Presentation);
        TestTrue(TEXT("血液内联于表现"), Presentation->Blood->GetOuter() == Presentation);
        for (const auto* Profile : {Presentation->HitReaction.TorsoProfile.Get(), Presentation->HitReaction.HeadProfile.Get(),
            Presentation->HitReaction.ArmsProfile.Get(), Presentation->HitReaction.LegsProfile.Get()})
        {
            TestTrue(TEXT("受击混合参数内联于表现"), Profile && Profile->GetOuter() == Presentation);
        }
    }
    auto* Test = LoadObject<UBBBMonsterPresentationDefinition>(nullptr,
        TEXT("/Game/_Project/Debug/ZombieHitReaction/DA_TargetPresentation"));
    auto* Formal = LoadObject<UBBBMonsterPresentationDefinition>(nullptr,
        TEXT("/Game/_Project/Mass/Monster/Zombie/Male/Variants/Connor/DA_BBBZombieConnorPresentation"));
    if (!TestNotNull(TEXT("独立调参配置"), Test) || !TestNotNull(TEXT("正式样本"), Formal))
    {
        return false;
    }
    TestTrue(TEXT("测试配置整体有效"), Test->IsValid());
    TestTrue(TEXT("测试受力参数不污染正式配置"), Test->HitReaction.TorsoProfile != Formal->HitReaction.TorsoProfile);
    TestTrue(TEXT("测试物理约束不污染正式身体"), Test->PhysicsAsset != Formal->PhysicsAsset);
    TestTrue(TEXT("测试血液分区独立"), Test->Blood != Formal->Blood);
    auto* Candidate = DuplicateObject<UBBBMonsterPresentationDefinition>(Formal, GetTransientPackage());
    Candidate->Blood->HitSpray.Lifetime = 3;
    TestFalse(TEXT("拒绝超预算血粒子寿命"), Candidate->IsValid());
    Candidate->Blood->HitSpray.Lifetime = 1;
    Candidate->HitReaction.ImpulseScale = 5;
    TestFalse(TEXT("拒绝超预算受击倍率"), Candidate->IsValid());
    Candidate->HitReaction.ImpulseScale = 1;
    Candidate->Gibs.Lifetime = 121;
    TestFalse(TEXT("拒绝超预算残肢寿命"), Candidate->IsValid());
    return true;
}

#endif
