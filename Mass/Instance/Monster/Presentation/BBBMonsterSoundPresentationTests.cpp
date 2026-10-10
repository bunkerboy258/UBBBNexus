#if WITH_DEV_AUTOMATION_TESTS && WITH_EDITOR

#include "Misc/AutomationTest.h"
#include "Misc/ScopeExit.h"
#include "Misc/PackageName.h"
#include "BBBMonsterSoundPresentationComponent.h"
#include "BBBMonsterPresentationActor.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Config/BBBMonsterSoundPresentationDefinition.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Config/BBBMonsterDefinition.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/HitReaction/BBBMonsterHitReactionFragment.h"
#include "Sound/SoundWave.h"
#include "Sound/SoundAttenuation.h"
#include "Sound/SoundConcurrency.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "UObject/Package.h"
#include "UObject/UnrealType.h"

/** 声音只消费当前事实 验证去重 清理和配置防呆 */
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBBBMonsterSoundFactsTest, "UBBB.Mass.ZombieAudio.Facts",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FBBBMonsterSoundFactsTest::RunTest(const FString& Parameters)
{
    const auto Initialization = UWorld::InitializationValues()
        .AllowAudioPlayback(false)
        .CreatePhysicsScene(false)
        .CreateNavigation(false)
        .CreateAISystem(false)
        .ShouldSimulatePhysics(false);
    UWorld* const World = UWorld::CreateWorld(EWorldType::Game, false, NAME_None, nullptr, true, ERHIFeatureLevel::Num, &Initialization);
    if (!TestNotNull(TEXT("声音测试世界"), World))
    {
        return false;
    }

    GEngine->CreateNewWorldContext(EWorldType::Game).SetCurrentWorld(World);
    ON_SCOPE_EXIT
    {
        GEngine->DestroyWorldContext(World);
        World->DestroyWorld(false);
        World->GetOutermost()->SetDirtyFlag(false);
    };

    UBBBMonsterSoundPresentationDefinition* Settings = NewObject<UBBBMonsterSoundPresentationDefinition>();
    Settings->Attenuation = NewObject<USoundAttenuation>(Settings);
    Settings->AmbientConcurrency = NewObject<USoundConcurrency>(Settings);
    Settings->ActionConcurrency = NewObject<USoundConcurrency>(Settings);
    Settings->ContactConcurrency = NewObject<USoundConcurrency>(Settings);
    Settings->AmbientConcurrency->Concurrency.SetEnableMaxCountPlatformScaling(false);
    Settings->ActionConcurrency->Concurrency.SetEnableMaxCountPlatformScaling(false);
    USoundWave* Wave = NewObject<USoundWave>(Settings);
    Wave->Duration = 1.0f;
    Settings->Footsteps.Add(Wave);
    Settings->CrawlFriction.Add(Wave);
    Settings->Landings.Add(Wave);
    Settings->Severings.Add(Wave);
    FBBBMonsterSoundVoice Voice;
    Voice.Name = TEXT("TestVoice");
    for (auto* Pool : { &Voice.Idle, &Voice.Alert, &Voice.Chase, &Voice.Attack, &Voice.Hit, &Voice.Death, &Voice.Crawl })
    {
        Pool->Add(Wave);
    }

    Settings->Voices.Add(Voice);
    Settings->Voices.Add(Voice);
    Settings->Voices[1].Name = TEXT("OtherVoice");
    TestTrue(TEXT("完整声音配置有效"), Settings->IsValid());
    Settings->Voices[1].Name = Voice.Name;
    TestFalse(TEXT("重复声线名拒绝"), Settings->IsValid());
    Settings->Voices[1].Name = TEXT("OtherVoice");
    Wave->bLooping = true;
    TestFalse(TEXT("循环素材拒绝"), Settings->IsValid());
    Wave->bLooping = false;
    Settings->HitInterval = -1.0f;
    TestFalse(TEXT("负数受击间隔拒绝"), Settings->IsValid());
    Settings->HitInterval = 0.22f;

    ABBBMonsterPresentationActor* Actor = World->SpawnActor<ABBBMonsterPresentationActor>();
    UBBBMonsterSoundPresentationComponent* Sound = Actor->GetMonsterSoundPresentation();
    TestNotNull(TEXT("声音组件由原生基类持有"), Sound);
    TestFalse(TEXT("声音没有独立组件更新"), Sound->PrimaryComponentTick.bCanEverTick);
    const FGuid Instance(1, 2, 3, 4);
    FBBBMonsterHitReactionFragment Hit;
    Hit.Serial = 7;
    Hit.Age = 0.0f;
    Sound->ApplyFacts(Instance, Settings, EBBBMonsterBehavior::Attack, 11, 0.8f, Hit, false, 0.0f, 0.0f, true);
    const int32 VoiceIndex = Sound->GetVoiceIndex();
    TestEqual(TEXT("初次交接不重播已有攻击或命中"), Sound->GetRequestedVoiceCount(), 0u);
    TestEqual(TEXT("初始命中直接作为基线"), Sound->GetObservedHitSerial(), 7u);

    Sound->ApplyFacts(Instance, Settings, EBBBMonsterBehavior::Attack, 12, 0.0f, Hit, false, 0.0f, 0.1f, false);
    TestEqual(TEXT("新攻击发声一次"), Sound->GetRequestedVoiceCount(), 1u);
    Sound->ApplyFacts(Instance, Settings, EBBBMonsterBehavior::Attack, 12, 0.2f, Hit, false, 0.0f, 0.2f, false);
    TestEqual(TEXT("同编号攻击不重复发声"), Sound->GetRequestedVoiceCount(), 1u);
    Hit.Serial = 8;
    Sound->ApplyFacts(Instance, Settings, EBBBMonsterBehavior::Attack, 12, 0.3f, Hit, false, 0.0f, 0.3f, false);
    TestEqual(TEXT("成立的新命中发声"), Sound->GetRequestedVoiceCount(), 2u);
    Hit.Serial = 9;
    Sound->ApplyFacts(Instance, Settings, EBBBMonsterBehavior::Attack, 12, 0.4f, Hit, false, 0.0f, 0.31f, false);
    TestEqual(TEXT("连续命中限流且不追加队列"), Sound->GetRequestedVoiceCount(), 2u);
    TestEqual(TEXT("被限流的当前编号仍被消费"), Sound->GetObservedHitSerial(), 9u);
    Sound->ApplyFacts(Instance, Settings, EBBBMonsterBehavior::Attack, 12, 0.7f, Hit, false, 0.0f, 0.7f, false);
    TestEqual(TEXT("限流结束不补播旧命中"), Sound->GetRequestedVoiceCount(), 2u);
    Hit.Serial = 10;
    Hit.Age = 1.0f;
    Sound->ApplyFacts(Instance, Settings, EBBBMonsterBehavior::Attack, 12, 0.8f, Hit, false, 0.0f, 0.8f, false);
    TestEqual(TEXT("过期命中不发声"), Sound->GetRequestedVoiceCount(), 2u);
    Sound->ApplyFacts(Instance, Settings, EBBBMonsterBehavior::Attack, 13, 0.9f, Hit, false, 0.0f, 0.9f, false);
    TestEqual(TEXT("晚到的攻击不补播起手音"), Sound->GetRequestedVoiceCount(), 2u);

    Sound->ApplyFacts(Instance, Settings, EBBBMonsterBehavior::Patrol, 14, 0.0f, Hit, true, 75.0f, 1.0f, false);
    TestEqual(TEXT("转入持续爬行只发一次"), Sound->GetRequestedVoiceCount(), 3u);
    Sound->ApplyFacts(Instance, Settings, EBBBMonsterBehavior::Patrol, 14, 0.0f, Hit, true, 75.0f, 1.1f, false);
    TestEqual(TEXT("爬行不逐帧发声"), Sound->GetRequestedVoiceCount(), 3u);
    Sound->ApplyFacts(Instance, Settings, EBBBMonsterBehavior::Chase, 15, 0.0f, Hit, true, 75.0f, 10.0f, false);
    TestEqual(TEXT("持续爬行到期再发声"), Sound->GetRequestedVoiceCount(), 4u);
    TestEqual(TEXT("改变行为仍使用固定声线"), Sound->GetVoiceIndex(), VoiceIndex);
    Hit.Serial = 11;
    Hit.Age = 0.0f;
    Sound->ApplyFacts(Instance, Settings, EBBBMonsterBehavior::Dead, 16, 0.0f, Hit, true, 0.0f, 11.0f, false);
    TestEqual(TEXT("致死命中只保留死亡声"), Sound->GetRequestedVoiceCount(), 5u);
    Sound->ApplyFacts(Instance, Settings, EBBBMonsterBehavior::Dead, 16, 1.0f, Hit, true, 0.0f, 12.0f, false);
    TestEqual(TEXT("死亡声不重复"), Sound->GetRequestedVoiceCount(), 5u);
    Sound->ApplyFacts(Instance, Settings, EBBBMonsterBehavior::Chase, 17, 0.0f, Hit, false, 500.0f, 20.0f, false);
    TestEqual(TEXT("迟到活动结果不恢复死亡发声"), Sound->GetRequestedVoiceCount(), 5u);

    Actor->SetActorHiddenInGame(true);
    TestEqual(TEXT("隐藏演员停止声音并清空声线"), Sound->GetVoiceIndex(), INDEX_NONE);
    TestEqual(TEXT("隐藏清理全部诊断计数"), Sound->GetRequestedVoiceCount(), 0u);
    Actor->SetActorHiddenInGame(false);
    Sound->ApplyFacts(Instance, Settings, EBBBMonsterBehavior::Dead, 16, 1.0f, Hit, true, 0.0f, 21.0f, true);
    TestEqual(TEXT("恢复死亡演员不补播死亡声"), Sound->GetRequestedVoiceCount(), 0u);
    TestEqual(TEXT("同代实体复用后声线不变"), Sound->GetVoiceIndex(), VoiceIndex);
    Sound->ApplyFacts(FGuid(8, 7, 6, 5), Settings, EBBBMonsterBehavior::Idle, 0, 0.0f, Hit, false, 0.0f, 22.0f, true);
    TestEqual(TEXT("新代实体交接不读取旧命中历史"), Sound->GetRequestedVoiceCount(), 0u);
    Sound->ApplyFacts(FGuid(8, 7, 6, 5), Settings, EBBBMonsterBehavior::Alert, 1, 0.0f, Hit, false, 0.0f, 22.1f, false);
    TestEqual(TEXT("进入警觉只发一次"), Sound->GetRequestedVoiceCount(), 1u);
    Sound->ApplyFacts(FGuid(8, 7, 6, 5), Settings, EBBBMonsterBehavior::Alert, 1, 0.0f, Hit, false, 0.0f, 22.2f, false);
    TestEqual(TEXT("警觉不逐帧发声"), Sound->GetRequestedVoiceCount(), 1u);
    Sound->ApplyFacts(FGuid(8, 7, 6, 5), Settings, EBBBMonsterBehavior::Chase, 2, 0.0f, Hit, false, 500.0f, 22.3f, false);
    Sound->ApplyFacts(FGuid(8, 7, 6, 5), Settings, EBBBMonsterBehavior::Chase, 2, 0.0f, Hit, false, 500.0f, 23.2f, false);
    TestEqual(TEXT("进入追击不继续等待原待机长间隔"), Sound->GetRequestedVoiceCount(), 2u);
    Sound->ApplyFacts(FGuid(8, 7, 6, 5), Settings, EBBBMonsterBehavior::Chase, 2, 0.0f, Hit, false, 500.0f, 23.21f, false);
    TestEqual(TEXT("追击不逐帧发声"), Sound->GetRequestedVoiceCount(), 2u);
    TestEqual(TEXT("无音频设备时未冒充实际播放"), Sound->GetStartedVoiceCount(), 0u);
    return true;
}

/** 配置资产覆盖全部现存僵尸定义 并验证三种声线和空间预算 */
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBBBMonsterSoundAssetsTest, "UBBB.Mass.ZombieAudio.Assets",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FBBBMonsterSoundAssetsTest::RunTest(const FString& Parameters)
{
    UBBBMonsterSoundPresentationDefinition* Settings = LoadObject<UBBBMonsterSoundPresentationDefinition>(nullptr,
        TEXT("/Game/_Project/System/Mass/Monster/Zombie/Shared/Audio/DA_BBBZombieSoundPresentation.DA_BBBZombieSoundPresentation"));
    if (!TestNotNull(TEXT("持久声音配置"), Settings))
    {
        return false;
    }

    TestTrue(TEXT("持久声音配置有效"), Settings->IsValid());
    TestEqual(TEXT("三种声音风格"), Settings->Voices.Num(), 3);
    TestEqual(TEXT("环境发声全局上限"), Settings->AmbientConcurrency->Concurrency.MaxCount, 8);
    TestEqual(TEXT("动作发声全局上限"), Settings->ActionConcurrency->Concurrency.MaxCount, 12);
    TestFalse(TEXT("环境预算按全局而非各演员计算"), Settings->AmbientConcurrency->Concurrency.bLimitToOwner);
    TestFalse(TEXT("动作预算按全局而非各演员计算"), Settings->ActionConcurrency->Concurrency.bLimitToOwner);
    TestFalse(TEXT("环境上限不被平台默认十六覆盖"), Settings->AmbientConcurrency->Concurrency.IsMaxCountPlatformScalingEnabled());
    TestFalse(TEXT("动作上限不被平台默认十六覆盖"), Settings->ActionConcurrency->Concurrency.IsMaxCountPlatformScalingEnabled());

    const TArray<FString> Definitions =
    {
        TEXT("Male/DA_BBBZombieMaleDefinition"),
        TEXT("Female/DA_BBBZombieFemaleDefinition"),
        TEXT("Male/Variants/Connor/DA_BBBZombieConnorDefinition"),
        TEXT("Male/Variants/Jiho/DA_BBBZombieJihoDefinition"),
        TEXT("Male/Variants/Michael/DA_BBBZombieMichaelDefinition"),
        TEXT("Male/Variants/Morita/DA_BBBZombieMoritaDefinition"),
        TEXT("Male/Variants/Renzo/DA_BBBZombieRenzoDefinition"),
        TEXT("Male/Variants/Sakurada/DA_BBBZombieSakuradaDefinition"),
        TEXT("Female/Variants/Alice/DA_BBBZombieAliceDefinition"),
        TEXT("Female/Variants/Kiyo/DA_BBBZombieKiyoDefinition"),
        TEXT("Female/Variants/Setsuko/DA_BBBZombieSetsukoDefinition"),
        TEXT("Female/Variants/Yuina/DA_BBBZombieYuinaDefinition")
    };
    for (const FString& Relative : Definitions)
    {
        const FString Path = TEXT("/Game/_Project/System/Mass/Monster/Zombie/") + Relative;
        UBBBMonsterDefinition* Definition = LoadObject<UBBBMonsterDefinition>(nullptr, *Path);
        if (TestNotNull(Relative, Definition))
        {
            TestEqual(Relative + TEXT(" 绑定同一声音配置"), Definition->SoundPresentation.Get(), Settings);
            TestTrue(Relative + TEXT(" 玩法配置仍有效"), Definition->IsValid());
        }

        FString ActorPath = Path.Replace(TEXT("DA_BBBZombie"), TEXT("BP_BBBZombie"));
        ActorPath = ActorPath.Replace(TEXT("Definition"), TEXT("Presentation"));
        const FString ClassPath = ActorPath + TEXT(".") + FPackageName::GetShortName(ActorPath) + TEXT("_C");
        UClass* ActorClass = LoadClass<ABBBMonsterPresentationActor>(nullptr, *ClassPath);
        if (TestNotNull(Relative + TEXT(" 表现蓝图类"), ActorClass))
        {
            const auto* Default = ActorClass->GetDefaultObject<ABBBMonsterPresentationActor>();
            TestNotNull(Relative + TEXT(" 继承原生声音组件"), Default->GetMonsterSoundPresentation());
        }
    }

    return true;
}

#endif
