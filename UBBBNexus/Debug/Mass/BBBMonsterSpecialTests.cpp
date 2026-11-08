#include "Misc/AutomationTest.h"
#include <limits>

#if WITH_DEV_AUTOMATION_TESTS && WITH_EDITOR
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Config/BBBMonsterSpecialDefinition.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Combat/BBBMonsterSpecialFragment.h"
#include "BBBWork/UBBBNexus/Combat/BBBHitReceiver.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Input/AuthorityFact/Network/FBBBMonsterStateAuthorityFactPacket.h"

/** 验证特感网络拒绝非法阶段 保留点伤害适配与普通移动交接 */
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBBBMonsterSpecialContractTest, "UBBB.Mass.SpecialInfected.Contracts",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FBBBMonsterSpecialContractTest::RunTest(const FString&)
{
    FBBBMonsterStateAuthorityFactPacket Packet;
    Packet.InstanceId = FGuid::NewGuid();
    Packet.Revision = 1;
    TestTrue(TEXT("普通怪物快照保持有效"), Packet.IsValid());
    Packet.Special.Phase = 255;
    TestFalse(TEXT("拒绝未知技能阶段"), Packet.IsValid());
    Packet.Special.Phase = 2;
    Packet.Special.Goal.X = std::numeric_limits<double>::quiet_NaN();
    TestFalse(TEXT("拒绝非法目标坐标"), Packet.IsValid());
    FBBBMonsterSpecialFact Fact;
    Fact.Phase = 2;
    TestTrue(TEXT("冲撞接管普通移动"), Fact.HoldsMovement());
    Fact.Phase = 5;
    TestFalse(TEXT("潜行继续使用导航移动"), Fact.HoldsMovement());
    Fact.Phase = 6;
    TestFalse(TEXT("狂攻继续使用正常接近与近战"), Fact.HoldsMovement());
    auto* Rule = NewObject<UBBBMonsterSpecialDefinition>();
    Rule->Kind = EBBBMonsterSpecialKind::Charger;
    TestTrue(TEXT("默认冲撞规则有效"), Rule->IsValid());
    Rule->RushSpeed = 0;
    TestFalse(TEXT("拒绝零速度冲撞"), Rule->IsValid());
    Rule->RushSpeed = 1000;
    Rule->Kind = EBBBMonsterSpecialKind::Spitter;
    TestFalse(TEXT("喷吐缺少粘液规则时拒绝"), Rule->IsValid());
    FBBBHitContribution Hit;
    Hit.AddVelocity = FVector(900, 0, 350);
    Hit.AddHoldSeconds = 2;
    TestTrue(TEXT("零扣血的击飞是有效命中"), Hit.IsValid());
    return true;
}
#endif
