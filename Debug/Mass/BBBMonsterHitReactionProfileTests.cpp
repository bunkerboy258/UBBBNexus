#if WITH_DEV_AUTOMATION_TESTS && WITH_EDITOR

#include "Misc/AutomationTest.h"
#include "Misc/ConfigCacheIni.h"
#include "HitReactProfile.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Presentation/BBBMonsterPresentationActor.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Presentation/BBBMonsterHitReactionComponent.h"

/** 静态验证正式受击配置可加载 不替换配置或伪造初始化完成状态 */
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBBBMonsterHitReactionProfileTest, "UBBB.Mass.ZombieHitReactionProfiles",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FBBBMonsterHitReactionProfileTest::RunTest(const FString&)
{
    TArray<FString> CookDirectories;
    GConfig->GetArray(TEXT("/Script/UnrealEd.ProjectPackagingSettings"), TEXT("DirectoriesToAlwaysCook"), CookDirectories, GGameIni);
    TestTrue(TEXT("四份软引用配置必须随项目烘焙"), CookDirectories.ContainsByPredicate([](const FString& Directory)
    {
        return Directory.Contains(TEXT("/Game/_Project/Mass/Monster/Zombie/Shared/HitReaction"));
    }));

    const auto* Defaults = GetDefault<UBBBMonsterHitReactionComponent>();
    const TCHAR* Names[] = {TEXT("HRP_BBBZombieTorso"), TEXT("HRP_BBBZombieHead"),
        TEXT("HRP_BBBZombieArms"), TEXT("HRP_BBBZombieLegs")};
    if (!TestEqual(TEXT("正式受击固定使用四份部位配置"), Defaults->AvailableProfiles.Num(), static_cast<int32>(UE_ARRAY_COUNT(Names))))
    {
        return false;
    }

    for (int32 Index = 0; Index < UE_ARRAY_COUNT(Names); ++Index)
    {
        const FString ExpectedPath = FString::Printf(
            TEXT("/Game/_Project/Mass/Monster/Zombie/Shared/HitReaction/%s.%s"), Names[Index], Names[Index]);
        auto ProfileReference = Defaults->AvailableProfiles[Index];
        TestEqual(TEXT("原生默认配置直接引用当前正式路径"), ProfileReference.ToSoftObjectPath().ToString(), ExpectedPath);
        const UHitReactProfile* Profile = ProfileReference.LoadSynchronous();
        if (!TestNotNull(*FString::Printf(TEXT("正式受击配置必须可加载 %s"), Names[Index]), Profile))
        {
            continue;
        }

        TestEqual(TEXT("加载结果不得依赖历史路径跳转"), Profile->GetPathName(), ExpectedPath);
        TestTrue(TEXT("正式物理混合权重必须有效"), FMath::IsFinite(Profile->MaxBlendWeight)
            && Profile->MaxBlendWeight > 0.0f && Profile->MaxBlendWeight <= 1.0f);
        TestTrue(TEXT("正式骨骼混合速度必须有效"), FMath::IsFinite(Profile->BoneBlendRate) && Profile->BoneBlendRate > 0.0f);
    }

    const TCHAR* PresentationPaths[] = {
        TEXT("/Game/_Project/Mass/Monster/Zombie/Male/BP_BBBZombieMalePresentation.BP_BBBZombieMalePresentation_C"),
        TEXT("/Game/_Project/Mass/Monster/Zombie/Female/BP_BBBZombieFemalePresentation.BP_BBBZombieFemalePresentation_C"),
        TEXT("/Game/_Project/Mass/Monster/Zombie/Male/Variants/Connor/BP_BBBZombieConnorPresentation.BP_BBBZombieConnorPresentation_C"),
        TEXT("/Game/_Project/Mass/Monster/Zombie/Male/Variants/Jiho/BP_BBBZombieJihoPresentation.BP_BBBZombieJihoPresentation_C"),
        TEXT("/Game/_Project/Mass/Monster/Zombie/Male/Variants/Michael/BP_BBBZombieMichaelPresentation.BP_BBBZombieMichaelPresentation_C"),
        TEXT("/Game/_Project/Mass/Monster/Zombie/Male/Variants/Morita/BP_BBBZombieMoritaPresentation.BP_BBBZombieMoritaPresentation_C"),
        TEXT("/Game/_Project/Mass/Monster/Zombie/Male/Variants/Renzo/BP_BBBZombieRenzoPresentation.BP_BBBZombieRenzoPresentation_C"),
        TEXT("/Game/_Project/Mass/Monster/Zombie/Male/Variants/Sakurada/BP_BBBZombieSakuradaPresentation.BP_BBBZombieSakuradaPresentation_C"),
        TEXT("/Game/_Project/Mass/Monster/Zombie/Female/Variants/Alice/BP_BBBZombieAlicePresentation.BP_BBBZombieAlicePresentation_C"),
        TEXT("/Game/_Project/Mass/Monster/Zombie/Female/Variants/Kiyo/BP_BBBZombieKiyoPresentation.BP_BBBZombieKiyoPresentation_C"),
        TEXT("/Game/_Project/Mass/Monster/Zombie/Female/Variants/Setsuko/BP_BBBZombieSetsukoPresentation.BP_BBBZombieSetsukoPresentation_C"),
        TEXT("/Game/_Project/Mass/Monster/Zombie/Female/Variants/Yuina/BP_BBBZombieYuinaPresentation.BP_BBBZombieYuinaPresentation_C")};
    for (const TCHAR* Path : PresentationPaths)
    {
        const UClass* PresentationClass = LoadClass<ABBBMonsterPresentationActor>(nullptr, Path);
        if (!TestNotNull(*FString::Printf(TEXT("正式表现类必须可加载 %s"), Path), PresentationClass))
        {
            continue;
        }

        const auto* ActorDefaults = PresentationClass->GetDefaultObject<ABBBMonsterPresentationActor>();
        const auto* Reaction = ActorDefaults->FindComponentByClass<UBBBMonsterHitReactionComponent>();
        if (!TestNotNull(TEXT("正式表现必须具备程序化受击组件"), Reaction))
        {
            continue;
        }

        TestTrue(*FString::Printf(TEXT("正式表现继承当前四份受击配置 %s"), Path),
            Reaction->AvailableProfiles == Defaults->AvailableProfiles);
    }

    return true;
}

#endif
