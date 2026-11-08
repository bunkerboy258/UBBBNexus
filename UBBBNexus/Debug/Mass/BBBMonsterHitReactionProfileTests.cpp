#if WITH_DEV_AUTOMATION_TESTS && WITH_EDITOR

#include "Misc/AutomationTest.h"
#include "HitReactProfile.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Config/BBBMonsterPresentationDefinition.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Presentation/BBBMonsterHitReactionComponent.h"

/** 参数直接由表现资产持有 不再依赖构造函数中的四条软路径 */
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBBBMonsterHitReactionProfileTest, "UBBB.Mass.ZombieHitReactionProfiles",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FBBBMonsterHitReactionProfileTest::RunTest(const FString&)
{
    TestEqual(TEXT("原生组件不保留独立配置软路径"), GetDefault<UBBBMonsterHitReactionComponent>()->AvailableProfiles.Num(), 0);
    auto* Definition = LoadObject<UBBBMonsterPresentationDefinition>(nullptr,
        TEXT("/Game/_Project/Mass/Monster/Zombie/Male/Variants/Connor/DA_BBBZombieConnorPresentation"));
    if (!TestNotNull(TEXT("正式表现配置"), Definition))
    {
        return false;
    }
    for (const UHitReactProfile* Profile : {Definition->HitReaction.TorsoProfile.Get(), Definition->HitReaction.HeadProfile.Get(),
        Definition->HitReaction.ArmsProfile.Get(), Definition->HitReaction.LegsProfile.Get()})
    {
        if (!TestNotNull(TEXT("内联受击分区"), Profile))
        {
            continue;
        }
        TestTrue(TEXT("受击分区由表现配置直接持有"), Profile->GetOuter() == Definition);
        TestTrue(TEXT("受力混合权重有效"), FMath::IsFinite(Profile->MaxBlendWeight)
            && Profile->MaxBlendWeight > 0 && Profile->MaxBlendWeight <= 1);
        TestTrue(TEXT("混合上限不超过现有六部位预算"), Profile->MaxActiveBlends <= 6);
    }
    return true;
}

#endif
