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
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Input/LocalControl/Health/FBBBMonsterDamageLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Processors/Health/BBBMonsterDamageProcessor.h"

/** 验证累计腿伤 不叠乘减速 过期消息与不可逆爬行 */
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBBBMonsterMobilityTest, "UBBB.Mass.ZombieMobility",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FBBBMonsterMobilityTest::RunTest(const FString& Parameters)
{
    const auto Initialization = UWorld::InitializationValues().AllowAudioPlayback(false).CreatePhysicsScene(false).CreateNavigation(false).CreateAISystem(false);
    UWorld* World = UWorld::CreateWorld(EWorldType::Game, false, NAME_None, nullptr, true, ERHIFeatureLevel::Num, &Initialization);
    if (!TestNotNull(TEXT("隔离世界"), World))
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
        FBBBMonsterMobilityFragment::StaticStruct(), FBBBMonsterNetworkFragment::StaticStruct()});
    const auto Entity = Manager.CreateEntity(Archetype);
    auto& Network = Manager.GetFragmentDataChecked<FBBBMonsterNetworkFragment>(Entity);
    Network.Definition = NewObject<UBBBMonsterDefinition>(World);
    auto& Damage = Manager.GetFragmentDataChecked<FBBBMonsterDamageFragment>(Entity);
    auto& Mobility = Manager.GetFragmentDataChecked<FBBBMonsterMobilityFragment>(Entity);
    auto& Health = Manager.GetFragmentDataChecked<FBBBMonsterHealthFragment>(Entity);
    auto* Processor = NewObject<UBBBMonsterDamageProcessor>(World);
    Processor->CallInitialize(World, Manager.AsShared());
    const auto Run = [&Manager, Processor]()
    {
        UE::Mass::FProcessingContext Context(Manager, 0.016f);
        UE::Mass::Executor::Run(*Processor, Context);
    };
    FBBBMonsterDamageContribution First;
    First.PlayerId = 1;
    First.Damage = 20.0;
    First.LegDamage = 20.0;
    First.LastHitTime = World->GetTimeSeconds();
    First.LastHitRegion = EBBBMonsterHitRegion::LeftLeg;
    Damage.Contributions.Add(1, First);
    Run();
    TestEqual(TEXT("腿伤首次减速"), Mobility.SlowMinimumRatio, 0.5f);
    TestFalse(TEXT("未达到腿伤阈值"), Mobility.bCrawling);
    const float End = Mobility.SlowEndsAt;
    Run();
    TestEqual(TEXT("重复快照不刷新减速"), Mobility.SlowEndsAt, End);

    FBBBMonsterDamageContribution Second = First;
    Second.PlayerId = 2;
    Second.Damage = 10.0;
    Second.LegDamage = 10.0;
    Damage.Contributions.Add(2, Second);
    Run();
    TestTrue(TEXT("合并不同玩家腿伤达到阈值"), Mobility.bCrawling);
    TestEqual(TEXT("第二次命中不叠乘"), Mobility.SlowMinimumRatio, 0.5f);
    TestEqual(TEXT("生命按全部累计贡献计算"), Health.CurrentHealth, 70.0f);
    FBBBMonsterDamageLocalControlPacket Packet;
    Packet.Include(First);
    auto Older = First;
    Older.Damage = 5.0;
    Older.LegDamage = 5.0;
    Older.LastHitTime = -5.0;
    Packet.Include(Older);
    TestEqual(TEXT("迟到小快照不覆盖腿伤"), Packet.Contributions[0].LegDamage, 20.0);
    TestEqual(TEXT("迟到小快照不覆盖最新命中"), Packet.Contributions[0].LastHitTime, First.LastHitTime);
    First.Damage = 40.0;
    First.LastHitTime = -10.0;
    Damage.Contributions.Add(1, First);
    Run();
    TestEqual(TEXT("过期命中不刷新短时减速"), Mobility.SlowEndsAt, End);
    TestTrue(TEXT("后续躯干伤害不能恢复站立"), Mobility.bCrawling);
    First.Damage = 100.0;
    Damage.Contributions.Add(1, First);
    Run();
    TestEqual(TEXT("死亡清除减速"), Mobility.SlowMinimumRatio, 1.0f);
    TestTrue(TEXT("死亡保持伏地姿势"), Mobility.bCrawling);
    return true;
}

#endif
