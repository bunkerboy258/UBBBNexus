#if WITH_DEV_AUTOMATION_TESTS && WITH_EDITOR

#include "Misc/AutomationTest.h"
#include "Misc/ScopeExit.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "MassEntitySubsystem.h"
#include "MassExecutor.h"
#include "MassProcessingContext.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Config/BBBMonsterDefinition.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Health/BBBMonsterHealthFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Health/BBBMonsterDamageFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Movement/BBBMonsterMobilityFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Network/BBBMonsterNetworkFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Behavior/BBBMonsterBehaviorFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Input/LocalControl/Health/FBBBMonsterDamageLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Processors/Health/BBBMonsterDamageProcessor.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Processors/Movement/BBBMonsterLocomotionProcessor.h"

/** 验证六部位当前结果的合并 致死与失能 不启动多客户端 */
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBBBMonsterPartDamageTest, "UBBB.Mass.ZombiePartDamage",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FBBBMonsterPartDamageTest::RunTest(const FString&)
{
    FBBBMonsterDamageLocalControlPacket Snapshot;
    FBBBMonsterDamageContribution First;
    First.PlayerId = 1;
    First.Parts.Head = 10.0;
    First.Parts.LeftArm = 25.0;
    First.LastHitTime = 2.0;
    Snapshot.Include(First);
    auto OutOfOrder = First;
    OutOfOrder.Parts.Head = 20.0;
    OutOfOrder.Parts.LeftArm = 5.0;
    OutOfOrder.LastHitTime = 1.0;
    Snapshot.Include(OutOfOrder);
    Snapshot.Include(First);
    TestEqual(TEXT("逐分量最大值保留头伤"), Snapshot.Contributions[0].Parts.Head, 20.0);
    TestEqual(TEXT("逐分量最大值不丢掉臂伤"), Snapshot.Contributions[0].Parts.LeftArm, 25.0);
    TestEqual(TEXT("旧部位快照不重演旧元数据"), Snapshot.Contributions[0].LastHitTime, 2.0);

    const auto Initialization = UWorld::InitializationValues().AllowAudioPlayback(false)
        .CreatePhysicsScene(false).CreateNavigation(false).CreateAISystem(false);
    UWorld* World = UWorld::CreateWorld(EWorldType::Game, false, NAME_None, nullptr, true, ERHIFeatureLevel::Num, &Initialization);
    if (!TestNotNull(TEXT("部位伤害隔离世界"), World))
    {
        return false;
    }
    GEngine->CreateNewWorldContext(EWorldType::Game).SetCurrentWorld(World);
    ON_SCOPE_EXIT
    {
        GEngine->DestroyWorldContext(World);
        World->DestroyWorld(false);
    };
    auto& Manager = World->GetSubsystem<UMassEntitySubsystem>()->GetMutableEntityManager();
    const auto Type = Manager.CreateArchetype({FBBBMonsterHealthFragment::StaticStruct(), FBBBMonsterDamageFragment::StaticStruct(),
        FBBBMonsterMobilityFragment::StaticStruct(), FBBBMonsterNetworkFragment::StaticStruct(), FBBBMonsterBehaviorFragment::StaticStruct()});
    auto* Settings = NewObject<UBBBMonsterDefinition>(World);
    auto* Processor = NewObject<UBBBMonsterDamageProcessor>(World);
    Processor->CallInitialize(World, Manager.AsShared());
    const auto Create = [&]()
    {
        const auto Entity = Manager.CreateEntity(Type);
        Manager.GetFragmentDataChecked<FBBBMonsterNetworkFragment>(Entity).Definition = Settings;
        return Entity;
    };
    const auto Resolve = [&]()
    {
        UE::Mass::FProcessingContext Context(Manager, 1.0f / 60.0f);
        UE::Mass::Executor::Run(*Processor, Context);
    };
    const auto Apply = [&](const FMassEntityHandle Entity, const int32 PlayerId, const EBBBMonsterHitRegion Part, const double Amount)
    {
        auto& Contributions = Manager.GetFragmentDataChecked<FBBBMonsterDamageFragment>(Entity).Contributions;
        auto& Value = Contributions.FindOrAdd(PlayerId);
        Value.PlayerId = PlayerId;
        Value.Parts.Add(Part, Amount);
        Value.LastHitRegion = Part;
        Value.LastHitTime = World->GetTimeSeconds();
    };
    const auto Head = Create();
    Apply(Head, 1, EBBBMonsterHitRegion::Head, 35.0);
    Resolve();
    const auto& HeadHealth = Manager.GetFragmentDataChecked<FBBBMonsterHealthFragment>(Head);
    TestEqual(TEXT("头部致死不要求整体生命归零"), HeadHealth.CurrentHealth, 0.0f);
    TestEqual(TEXT("头部致死时整体生命仍有六十五"), HeadHealth.MainHealth, 65.0f);
    const auto Limbs = Create();
    Apply(Limbs, 1, EBBBMonsterHitRegion::LeftArm, 40.0);
    Apply(Limbs, 1, EBBBMonsterHitRegion::RightLeg, 35.0);
    Resolve();
    auto& Mobility = Manager.GetFragmentDataChecked<FBBBMonsterMobilityFragment>(Limbs);
    TestTrue(TEXT("单腿损毁立即持续爬行"), Mobility.bCrawling);
    TestEqual(TEXT("单臂损毁保留配置伤害比例"), Mobility.AttackRatio, Settings->OneArmAttackRatio);
    TestTrue(TEXT("非致死部位损毁不会隐式死亡"), Manager.GetFragmentDataChecked<FBBBMonsterHealthFragment>(Limbs).CurrentHealth > 0.0f);
    Apply(Limbs, 2, EBBBMonsterHitRegion::RightArm, 40.0);
    Resolve();
    TestEqual(TEXT("双臂损毁不再有攻击能力"), Mobility.AttackRatio, 0.0f);
    const auto Before = Manager.GetFragmentDataChecked<FBBBMonsterHealthFragment>(Limbs).MainHealth;
    Apply(Limbs, 1, EBBBMonsterHitRegion::LeftArm, 10.0);
    Resolve();
    TestTrue(TEXT("已损毁部位仍按同一规则传导后续伤害"), Manager.GetFragmentDataChecked<FBBBMonsterHealthFragment>(Limbs).MainHealth < Before);
    const auto Crowd = Create();
    for (int32 Player = 0; Player < 16; ++Player)
    {
        Apply(Crowd, Player, EBBBMonsterHitRegion::Torso, 2.0);
    }
    Resolve();
    TestEqual(TEXT("十六个玩家身份保持独立贡献"), Manager.GetFragmentDataChecked<FBBBMonsterDamageFragment>(Crowd).Contributions.Num(), 16);
    TestEqual(TEXT("十六份当前结果收敛为三十二伤害"), Manager.GetFragmentDataChecked<FBBBMonsterHealthFragment>(Crowd).MainHealth, 68.0f);
    const auto Light = Create();
    Apply(Light, 1, EBBBMonsterHitRegion::Torso, 5.0);
    Resolve();
    TestFalse(TEXT("普通命中只减速 不强制每次踉跄"), Manager.GetFragmentDataChecked<FBBBMonsterMobilityFragment>(Light).IsStaggering(World->GetTimeSeconds()));
    const auto Turn = UBBBMonsterLocomotionProcessor::TurnTowards(FQuat::Identity, -FVector::ForwardVector, 0.1f, 90.0f);
    TestTrue(TEXT("爬行九十度每秒 一帧不会瞬间转身"), FMath::IsNearlyEqual(FMath::Abs(Turn.Rotator().Yaw), 9.0f));
    return true;
}

#endif
