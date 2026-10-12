#if WITH_DEV_AUTOMATION_TESTS && WITH_EDITOR

#include "Misc/AutomationTest.h"
#include "Misc/ScopeExit.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "UObject/Package.h"
#include "Serialization/MemoryWriter.h"
#include "Serialization/MemoryReader.h"
#include "Serialization/ObjectAndNameAsStringProxyArchive.h"
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
        World->GetOutermost()->SetDirtyFlag(false);
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
    const auto Hit = [&Manager, World, Settings](const FMassEntityHandle Entity, const EBBBMonsterHitRegion Region, const double Damage, const double Age, const bool bTrigger = true)
    {
        FBBBMonsterDamageContribution Contribution;
        Contribution.PlayerId = 1;
        Contribution.Parts.Add(Region, Damage);
        Contribution.LastHitRegion = Region;
        Contribution.LastHitTime = World->GetTimeSeconds() - Age;
        if (bTrigger)
        {
            Contribution.Stagger.ActiveAttackId = FGuid::NewGuid();
            Contribution.Stagger.StartedAt = Contribution.LastHitTime;
            Contribution.Stagger.EndsAt = Contribution.LastHitTime + Settings->GetBodyPart(Region).StaggerDuration;
            Contribution.Stagger.ActiveRegion = Region;
        }
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
        TestEqual(TEXT("完整动作全程停止主动水平移动"), Mobility.HitStopEndsAt, Mobility.StaggerEndsAt);
        TestTrue(TEXT("完整原片不再压到零点九秒"), Mobility.StaggerEndsAt - Mobility.StaggerStartedAt > 2.3f);
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
        TestEqual(TEXT("原片结束前仍然停步"), Mobility.GetSpeedRatio(StopEnd - 0.001f), 0.0f);
        TestEqual(TEXT("恢复最终回到正常速度"), Mobility.GetSpeedRatio(FMath::Max(Mobility.SlowEndsAt, Mobility.StaggerEndsAt)), 1.0f);
        const float SlowEnd = Mobility.SlowEndsAt;
        Run();
        TestEqual(TEXT("重复累计字典不刷新减速"), Mobility.SlowEndsAt, SlowEnd);
    }
    const auto Attacking = Create();
    auto& AttackState = Manager.GetFragmentDataChecked<FBBBMonsterBehaviorFragment>(Attacking);
    AttackState.State = EBBBMonsterBehavior::Attack;
    AttackState.ActionId = 7;
    Hit(Attacking, EBBBMonsterHitRegion::Torso, 5.0, 0.0, false);
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
        Hit(Suppressed, EBBBMonsterHitRegion::Torso, Shot * 10.0, 0.0, false);
        Run();
    }
    TestFalse(TEXT("累计伤害不能绕过子弹踉跄判定"), Manager.GetFragmentDataChecked<FBBBMonsterMobilityFragment>(Suppressed).IsStaggering(Now));
    const auto HeavyWithoutTrigger = Create();
    Hit(HeavyWithoutTrigger, EBBBMonsterHitRegion::Torso, 60.0, 0.0, false);
    Run();
    TestFalse(TEXT("大伤害没有踉跄结果时也不触发旧重击机制"), Manager.GetFragmentDataChecked<FBBBMonsterMobilityFragment>(HeavyWithoutTrigger).IsStaggering(Now));
    const auto SmallWithTrigger = Create();
    Hit(SmallWithTrigger, EBBBMonsterHitRegion::Torso, 1.0, 0.0);
    Run();
    TestTrue(TEXT("小伤害可以独立触发完整踉跄"), Manager.GetFragmentDataChecked<FBBBMonsterMobilityFragment>(SmallWithTrigger).IsStaggering(Now));
    TestTrue(TEXT("头部概率倍率大于手臂"), Settings->HeadPart.StaggerMultiplier > Settings->LeftArmPart.StaggerMultiplier);
    TestTrue(TEXT("腿部概率倍率大于躯干"), Settings->LeftLegPart.StaggerMultiplier > Settings->TorsoPart.StaggerMultiplier);
    const auto Restored = Create();
    Hit(Restored, EBBBMonsterHitRegion::RightLeg, 1.0, 0.5);
    Manager.GetFragmentDataChecked<FBBBMonsterHealthFragment>(Restored).ResolvedDamage = 1.0;
    Run();
    TestTrue(TEXT("当前结果恢复不依赖重复结算伤害"), Manager.GetFragmentDataChecked<FBBBMonsterMobilityFragment>(Restored).IsStaggering(Now));

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
    FBBBMonsterDamageContribution Current;
    Current.PlayerId = 1;
    Current.Parts.Torso = 10.0;
    Current.Stagger.AttackId = FGuid::NewGuid();
    Current.Stagger.AttackTime = 1.0;
    Current.Stagger.ActiveAttackId = Current.Stagger.AttackId;
    Current.Stagger.StartedAt = 1.0;
    Current.Stagger.EndsAt = 1.0 + Settings->TorsoPart.StaggerDuration;
    FBBBMonsterDamageContribution NewHit;
    NewHit.PlayerId = 1;
    NewHit.Parts.LeftArm = 5.0;
    NewHit.LastHitTime = 2.0;
    NewHit.Stagger.AttackId = FGuid::NewGuid();
    NewHit.Stagger.AttackTime = 2.0;
    const auto ActiveId = Current.Stagger.ActiveAttackId;
    Current.Merge(NewHit);
    TestTrue(TEXT("交叉部位伤害仍按分量合并"), Current.Parts.Torso == 10.0 && Current.Parts.LeftArm == 5.0);
    TestEqual(TEXT("后续未触发命中不覆盖当前有效踉跄"), Current.Stagger.ActiveAttackId, ActiveId);
    const auto Snapshot = Current;
    Current.Merge(Snapshot);
    TestTrue(TEXT("重复当前结果合并幂等"), Current == Snapshot);
    TestTrue(TEXT("当前踉跄贡献快照合法"), Current.IsValid());
    TArray<uint8> Bytes;
    FMemoryWriter Writer(Bytes);
    FObjectAndNameAsStringProxyArchive Saving(Writer, false);
    FBBBMonsterDamageContribution::StaticStruct()->SerializeItem(Saving, &Current, nullptr);
    FMemoryReader Reader(Bytes);
    FObjectAndNameAsStringProxyArchive Loading(Reader, false);
    FBBBMonsterDamageContribution Received;
    FBBBMonsterDamageContribution::StaticStruct()->SerializeItem(Loading, &Received, nullptr);
    TestEqual(TEXT("当前踉跄身份进入反射传输"), Received.Stagger.ActiveAttackId, ActiveId);
    TestEqual(TEXT("当前踉跄结束时间进入反射传输"), Received.Stagger.EndsAt, Current.Stagger.EndsAt);
    TestFalse(TEXT("随机候选和命中汇总不进入网络快照"), Received.Stagger.AttackId.IsValid());
    TestTrue(TEXT("远端当前快照有效且不携带随机样本"), Received.IsValid() && Received.Stagger.Impacts.Sum() == 0.0);
    const auto LocalAttackId = Current.Stagger.AttackId;
    Current.Merge(Received);
    TestEqual(TEXT("房主镜像返回不清掉本机同发去重缓存"), Current.Stagger.AttackId, LocalAttackId);
    FBBBMonsterStaggerContribution Candidate;
    Candidate.AttackId = FGuid::NewGuid();
    Candidate.HitTime = 5.0;
    Candidate.Duration = 3.0f;
    Candidate.Roll = 0.5f;
    Candidate.Chance = 0.0f;
    TestFalse(TEXT("零踉跄值绝不触发"), Candidate.Resolve(0.0));
    Candidate.AttackId = FGuid::NewGuid();
    Candidate.bEvaluated = false;
    Candidate.Chance = 1.0f;
    TestTrue(TEXT("足够踉跄值可以必定触发"), Candidate.Resolve(0.0));
    for (int32 Pellet = 0; Pellet < 64; ++Pellet)
    {
        TestFalse(TEXT("同发全部霰弹不重复判定"), Candidate.Resolve(0.0));
    }
    Candidate.bEvaluated = false;
    TestFalse(TEXT("恢复保护期间的新命中不排队"), Candidate.Resolve(5.35));
    TestFalse(TEXT("被保护窗口拒绝的攻击不能晚些时候补播"), Candidate.Resolve(0.0));
    Candidate.bEvaluated = false;
    Candidate.HitTime = 5.35;
    TestTrue(TEXT("恢复保护完成后的新攻击正常判定"), Candidate.Resolve(5.35));
    return true;
}

#endif
