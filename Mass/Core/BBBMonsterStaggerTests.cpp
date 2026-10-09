#if WITH_DEV_AUTOMATION_TESTS && WITH_EDITOR

#include "Misc/AutomationTest.h"
#include "Misc/ScopeExit.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "MassEntitySubsystem.h"
#include "MassExecutor.h"
#include "MassProcessingContext.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Config/BBBMonsterDefinition.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Behavior/BBBMonsterBehaviorFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Health/BBBMonsterHealthFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Health/BBBMonsterDamageFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Movement/BBBMonsterMobilityFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Network/BBBMonsterNetworkFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Processors/Health/BBBMonsterDamageProcessor.h"

/** 验证六部位停顿 连射不重启 攻击时序与爬行死亡抢占 */
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBBBMonsterStaggerTest, "UBBB.Mass.ZombieStagger",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FBBBMonsterStaggerTest::RunTest(const FString& Parameters)
{
    const auto Initialization = UWorld::InitializationValues().AllowAudioPlayback(false).CreatePhysicsScene(false).CreateNavigation(false).CreateAISystem(false);
    UWorld* World = UWorld::CreateWorld(EWorldType::Game, false, NAME_None, nullptr, true, ERHIFeatureLevel::Num, &Initialization);
    if (!TestNotNull(TEXT("踉跄隔离世界"), World))
    {
        return false;
    }
    GEngine->CreateNewWorldContext(EWorldType::Game).SetCurrentWorld(World);
    ON_SCOPE_EXIT
    {
        GEngine->DestroyWorldContext(World);
        World->DestroyWorld(false);
    };
    FMassEntityManager& Manager = World->GetSubsystem<UMassEntitySubsystem>()->GetMutableEntityManager();
    const auto Archetype = Manager.CreateArchetype({FBBBMonsterHealthFragment::StaticStruct(), FBBBMonsterDamageFragment::StaticStruct(),
        FBBBMonsterMobilityFragment::StaticStruct(), FBBBMonsterNetworkFragment::StaticStruct(), FBBBMonsterBehaviorFragment::StaticStruct()});
    auto* Settings = NewObject<UBBBMonsterDefinition>(World);
    auto* Processor = NewObject<UBBBMonsterDamageProcessor>(World);
    Processor->CallInitialize(World, Manager.AsShared());
    const auto Run = [&Manager, Processor]()
    {
        UE::Mass::FProcessingContext Context(Manager, 0.016f);
        UE::Mass::Executor::Run(*Processor, Context);
    };
    const auto Create = [&Manager, Archetype, Settings]()
    {
        const auto Entity = Manager.CreateEntity(Archetype);
        Manager.GetFragmentDataChecked<FBBBMonsterNetworkFragment>(Entity).Definition = Settings;
        Manager.GetFragmentDataChecked<FBBBMonsterBehaviorFragment>(Entity).State = EBBBMonsterBehavior::Chase;
        return Entity;
    };
    const auto Hit = [&Manager, World](const FMassEntityHandle Entity, const EBBBMonsterHitRegion Region, const double Damage, const double Age)
    {
        FBBBMonsterDamageContribution Contribution;
        Contribution.PlayerId = 1;
        Contribution.Parts.Add(Region, Damage);
        Contribution.LastHitRegion = Region;
        Contribution.LastHitTime = World->GetTimeSeconds() - Age;
        Manager.GetFragmentDataChecked<FBBBMonsterDamageFragment>(Entity).Contributions.Add(1, Contribution);
    };
    const float Now = World->GetTimeSeconds();
    for (int32 Region = 0; Region < 6; ++Region)
    {
        const auto Entity = Create();
        Hit(Entity, static_cast<EBBBMonsterHitRegion>(Region), 25.0, 0.0);
        Run();
        auto& Mobility = Manager.GetFragmentDataChecked<FBBBMonsterMobilityFragment>(Entity);
        TestTrue(TEXT("六部位有效非致命命中都可踉跄"), Mobility.IsStaggering(Now));
        TestEqual(TEXT("主段水平速度为零"), Mobility.GetSpeedRatio(Now + 0.1f), 0.0f);
        TestTrue(TEXT("停顿和踉跄有明确截止时间"), Mobility.HitStopEndsAt < Mobility.StaggerEndsAt);
        const float Start = Mobility.StaggerStartedAt;
        const float End = Mobility.StaggerEndsAt;
        const float StopEnd = Mobility.HitStopEndsAt;
        const auto FirstRegion = Mobility.StaggerRegion;
        Hit(Entity, static_cast<EBBBMonsterHitRegion>(Region), 26.0, 0.0);
        Run();
        TestEqual(TEXT("连射不重启动画"), Mobility.StaggerStartedAt, Start);
        TestEqual(TEXT("连射不无限延长踉跄"), Mobility.StaggerEndsAt, End);
        TestEqual(TEXT("连射不延长零速度阶段"), Mobility.HitStopEndsAt, StopEnd);
        TestEqual(TEXT("连射不切换正在播放的方向"), Mobility.StaggerRegion, FirstRegion);
        TestTrue(TEXT("停顿结束保留可见减速"), Mobility.GetSpeedRatio(StopEnd) > 0.0f && Mobility.GetSpeedRatio(StopEnd) <= 0.5f);
        TestEqual(TEXT("恢复最终回到正常速度"), Mobility.GetSpeedRatio(Mobility.SlowEndsAt), 1.0f);
        const float SlowEnd = Mobility.SlowEndsAt;
        Run();
        TestEqual(TEXT("重复累计字典不刷新减速"), Mobility.SlowEndsAt, SlowEnd);
    }
    const auto Attacking = Create();
    auto& AttackState = Manager.GetFragmentDataChecked<FBBBMonsterBehaviorFragment>(Attacking);
    AttackState.State = EBBBMonsterBehavior::Attack;
    AttackState.ActionId = 7;
    Hit(Attacking, EBBBMonsterHitRegion::Torso, 5.0, 0.0);
    Run();
    const auto& AttackMobility = Manager.GetFragmentDataChecked<FBBBMonsterMobilityFragment>(Attacking);
    TestFalse(TEXT("普通命中不抢占攻击"), AttackMobility.IsStaggering(Now));
    TestEqual(TEXT("普通命中不重启攻击编号"), AttackState.ActionId, 7u);
    TestEqual(TEXT("攻击中不新增水平硬停顿"), AttackMobility.HitStopEndsAt, 0.0f);

    Hit(Attacking, EBBBMonsterHitRegion::Torso, 35.0, 0.0);
    Run();
    TestTrue(TEXT("攻击时强命中提供踉跄事实供行为取消未命中攻击"), AttackMobility.IsStaggering(Now));

    const auto Suppressed = Create();
    for (int32 Shot = 1; Shot <= 4; ++Shot)
    {
        Hit(Suppressed, EBBBMonsterHitRegion::Torso, Shot * 10.0, 0.0);
        Run();
    }
    TestTrue(TEXT("小伤害连射累计压力可以触发一次踉跄"), Manager.GetFragmentDataChecked<FBBBMonsterMobilityFragment>(Suppressed).IsStaggering(Now));

    const auto Crawling = Create();
    Hit(Crawling, EBBBMonsterHitRegion::LeftLeg, 35.0, 0.0);
    Run();
    const auto& CrawlMobility = Manager.GetFragmentDataChecked<FBBBMonsterMobilityFragment>(Crawling);
    TestTrue(TEXT("有效腿伤仍按原阈值转入持续爬行"), CrawlMobility.bCrawling);
    TestFalse(TEXT("爬行不会播放站立踉跄"), CrawlMobility.IsStaggering(Now));

    const auto Dead = Create();
    auto& DeadMobility = Manager.GetFragmentDataChecked<FBBBMonsterMobilityFragment>(Dead);
    DeadMobility.StaggerStartedAt = Now;
    DeadMobility.StaggerEndsAt = Now + 1.0f;
    DeadMobility.HitStopEndsAt = Now + 0.5f;
    Hit(Dead, EBBBMonsterHitRegion::Torso, 200.0, 0.0);
    Run();
    TestFalse(TEXT("死亡立即清除踉跄"), DeadMobility.IsStaggering(Now));
    TestEqual(TEXT("死亡清除停顿"), DeadMobility.GetSpeedRatio(Now), 1.0f);

    const auto Expired = Create();
    Hit(Expired, EBBBMonsterHitRegion::Torso, 5.0, 10.0);
    Run();
    TestFalse(TEXT("迟到快照不重演踉跄"), Manager.GetFragmentDataChecked<FBBBMonsterMobilityFragment>(Expired).IsStaggering(Now));
    const auto Mirror = Create();
    Run();
    TestFalse(TEXT("没有有效伤害的镜像输入不生成踉跄"), Manager.GetFragmentDataChecked<FBBBMonsterMobilityFragment>(Mirror).IsStaggering(Now));
    return true;
}

#endif
