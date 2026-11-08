#include "Misc/AutomationTest.h"

#if WITH_DEV_AUTOMATION_TESTS && WITH_EDITOR
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "MassEntitySubsystem.h"
#include "MassExecutor.h"
#include "MassProcessingContext.h"
#include "Misc/ScopeExit.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Config/BBBMonsterDefinition.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Health/BBBMonsterHealthInputFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Health/BBBMonsterHealthFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Network/BBBMonsterNetworkFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Movement/BBBMonsterMobilityFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/HitReaction/BBBMonsterHitReactionFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Processors/Health/BBBMonsterDamageProcessor.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBBBHitRulesTest, "BBB.Combat.HitRules",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FBBBHitRulesTest::RunTest(const FString&)
{
    const auto Init = UWorld::InitializationValues().AllowAudioPlayback(false)
        .CreatePhysicsScene(false).CreateNavigation(false).CreateAISystem(false);
    UWorld* World = UWorld::CreateWorld(EWorldType::Game, false, NAME_None, nullptr, true, ERHIFeatureLevel::Num, &Init);
    GEngine->CreateNewWorldContext(EWorldType::Game).SetCurrentWorld(World);
    ON_SCOPE_EXIT
    {
        GEngine->DestroyWorldContext(World);
        World->DestroyWorld(false);
        World->GetOutermost()->SetDirtyFlag(false);
    };
    auto& Manager = World->GetSubsystem<UMassEntitySubsystem>()->GetMutableEntityManager();
    const auto Type = Manager.CreateArchetype({FBBBMonsterDamageFragment::StaticStruct(),
        FBBBMonsterHealthFragment::StaticStruct(), FBBBMonsterNetworkFragment::StaticStruct(),
        FBBBMonsterMobilityFragment::StaticStruct(), FBBBMonsterHitReactionFragment::StaticStruct()});
    auto* Processor = NewObject<UBBBMonsterDamageProcessor>(World);
    Processor->CallInitialize(World, Manager.AsShared());
    auto* Definition = NewObject<UBBBMonsterDefinition>(World);
    Definition->StaggerStartValue = 10;
    Definition->StaggerFullValue = 50;
    const auto Create = [&]()
    {
        const auto Entity = Manager.CreateEntity(Type);
        auto& Net = Manager.GetFragmentDataChecked<FBBBMonsterNetworkFragment>(Entity);
        Net.Definition = Definition;
        Net.InstanceId = FGuid::NewGuid();
        return Entity;
    };
    const auto Run = [&]()
    {
        UE::Mass::FProcessingContext Context(Manager, 0.01f);
        UE::Mass::Executor::Run(*Processor, Context);
    };
    const auto Entity = Create();
    auto& Damage = Manager.GetFragmentDataChecked<FBBBMonsterDamageFragment>(Entity);
    FBBBHitContribution Hit;
    Hit.PlayerId = 7;
    Hit.Part = 0;
    Hit.AddStagger = 30;
    Hit.AttackId = FGuid::NewGuid();
    Hit.Direction = FVector::ForwardVector;
    Hit.Normal = -Hit.Direction;
    Damage.PendingHits.Add(Hit);
    Run();
    TestEqual(TEXT("零扣血命中仍由强度计算概率"), Damage.Contributions[7].Stagger.Chance, 0.5f);
    TestEqual(TEXT("纯踉跄贡献不产生生命伤害"), Manager.GetFragmentDataChecked<FBBBMonsterHealthFragment>(Entity).CurrentHealth, 100.f);
    const auto Tough = Create();
    auto* ToughDefinition = NewObject<UBBBMonsterDefinition>(World);
    ToughDefinition->StaggerStartValue = 40;
    ToughDefinition->StaggerFullValue = 100;
    Manager.GetFragmentDataChecked<FBBBMonsterNetworkFragment>(Tough).Definition = ToughDefinition;
    auto& ToughDamage = Manager.GetFragmentDataChecked<FBBBMonsterDamageFragment>(Tough);
    ToughDamage.PendingHits.Add(Hit);
    Run();
    TestEqual(TEXT("相同贡献由目标抗性决定不同概率"), ToughDamage.Contributions[7].Stagger.Chance, 0.f);
    auto Invalid = Hit;
    Invalid.Part = MAX_uint8;
    TestFalse(TEXT("无部位点命中不默认为躯干"), FBBBMonsterDamageLocalControlPacket{{Invalid}}.IsValid());
    Invalid.bRadial = true;
    TestTrue(TEXT("范围作用不要求单点部位"), FBBBMonsterDamageLocalControlPacket{{Invalid}}.IsValid());
    const auto Radial = Create();
    FBBBHitContribution Blast{10.f};
    Blast.PlayerId = 7;
    Blast.bRadial = true;
    Blast.AddDurableDamage = 10;
    auto& BlastDamage = Manager.GetFragmentDataChecked<FBBBMonsterDamageFragment>(Radial);
    BlastDamage.PendingHits.Add(Blast);
    Run();
    TestTrue(TEXT("目标范围权重分配一次贡献"), FMath::IsNearlyEqual(BlastDamage.Contributions[7].Parts.Sum(), 10.0, 0.001));
    TestTrue(TEXT("躯干权重为五成"), FMath::IsNearlyEqual(BlastDamage.Contributions[7].Parts.Torso, 5.0, 0.001));

    const auto Mirror = Create();
    auto& MirrorDamage = Manager.GetFragmentDataChecked<FBBBMonsterDamageFragment>(Mirror);
    FBBBMonsterDamageTotalsRemoteMessagePacket Snapshot;
    Snapshot.Contributions = {Damage.Contributions[7]};
    Snapshot.bInitialSnapshot = true;
    Snapshot.Apply(MirrorDamage);
    Run();
    auto& MirrorHit = Manager.GetFragmentDataChecked<FBBBMonsterHitReactionFragment>(Mirror);
    TestEqual(TEXT("首次镜像只还原状态不重演旧受击"), MirrorHit.Serial, 0u);
    Snapshot.bInitialSnapshot = false;
    auto& RemoteHit = Snapshot.Contributions[0].LastHitResult;
    ++RemoteHit.Sequence;
    RemoteHit.Velocity = FVector(100, 0, 0);
    Snapshot.Apply(MirrorDamage);
    Run();
    TestEqual(TEXT("新结果只发布一次目标反应"), MirrorHit.Serial, 1u);
    TestTrue(TEXT("镜像还原已结算物理结果"), MirrorHit.ImpactVelocity.Equals(FVector(100, 0, 0)));
    Snapshot.Apply(MirrorDamage);
    Run();
    TestEqual(TEXT("重复累计消息不重复受力"), MirrorHit.Serial, 1u);
    return true;
}
#endif
