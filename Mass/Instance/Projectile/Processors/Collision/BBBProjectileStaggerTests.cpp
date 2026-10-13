#include "CoreMinimal.h"

#if WITH_DEV_AUTOMATION_TESTS && WITH_EDITOR

#include "Misc/AutomationTest.h"
#include "Misc/ScopeExit.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "UObject/Package.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/PlayerState.h"
#include "MassEntitySubsystem.h"
#include "MassExecutor.h"
#include "MassProcessingContext.h"
#include "BBBWork/UBBBNexus/Mass/Core/BBBMassSubsystem.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Projectile/Fragments/Spawn/BBBProjectileSpawnInputFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Projectile/Processors/Spawn/BBBProjectileParseProcessor.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Projectile/Processors/Movement/BBBProjectileMovementProcessor.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Projectile/Processors/Collision/BBBProjectileCollisionProcessor.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Config/BBBMonsterDefinition.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Health/BBBMonsterHealthFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Health/BBBMonsterHealthInputFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Movement/BBBMonsterMobilityFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/HitReaction/BBBMonsterHitReactionInputFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Network/BBBMonsterNetworkFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Processors/Health/BBBMonsterDamageProcessor.h"

/** 用真实出生 移动 碰撞与累计输入验证踉跄锁定 不依赖远程探针往返时间 */
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBBBProjectileStaggerTest, "UBBB.Mass.ProjectileStagger",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FBBBProjectileStaggerTest::RunTest(const FString&)
{
    const auto Initialization = UWorld::InitializationValues().AllowAudioPlayback(false).CreatePhysicsScene(true)
        .CreateNavigation(false).CreateAISystem(false).ShouldSimulatePhysics(false);
    UWorld* World = UWorld::CreateWorld(EWorldType::Game, false, NAME_None, nullptr, true, ERHIFeatureLevel::Num, &Initialization);
    GEngine->CreateNewWorldContext(EWorldType::Game).SetCurrentWorld(World);
    ON_SCOPE_EXIT
    {
        GEngine->DestroyWorldContext(World);
        World->DestroyWorld(false);
        World->GetOutermost()->SetDirtyFlag(false);
    };

    auto* Mass = World->GetSubsystem<UBBBMassSubsystem>();
    auto& Manager = World->GetSubsystem<UMassEntitySubsystem>()->GetMutableEntityManager();
    const auto ProjectileType = Manager.CreateArchetype({FTransformFragment::StaticStruct(), FMassVelocityFragment::StaticStruct(),
        FBBBProjectileMotionFragment::StaticStruct(), FBBBProjectileCollisionFragment::StaticStruct(),
        FBBBProjectileLifetimeFragment::StaticStruct(), FBBBProjectilePresentationFragment::StaticStruct(), FBBBProjectileSpawnInputFragment::StaticStruct()});
    const auto MonsterType = Manager.CreateArchetype({FBBBMonsterDamageFragment::StaticStruct(), FBBBMonsterHealthInputFragment::StaticStruct(),
        FBBBMonsterNetworkFragment::StaticStruct(), FBBBMonsterHealthFragment::StaticStruct(),
        FBBBMonsterMobilityFragment::StaticStruct(), FBBBMonsterHitReactionInputFragment::StaticStruct()});
    const auto Target = Manager.CreateEntity(MonsterType);
    auto& Network = Manager.GetFragmentDataChecked<FBBBMonsterNetworkFragment>(Target);
    Network.InstanceId = FGuid::NewGuid();
    Network.Definition = NewObject<UBBBMonsterDefinition>(World);
    auto* Controller = World->SpawnActor<APlayerController>();
    auto* Player = World->SpawnActor<APlayerState>();
    Player->SetPlayerId(7);
    Controller->SetPlayerState(Player);

    const auto Run = [&Manager, World](UClass* Type)
    {
        auto* Processor = NewObject<UMassProcessor>(World, Type);
        Processor->CallInitialize(World, Manager.AsShared());
        UE::Mass::FProcessingContext Context(Manager, 0.1f);
        UE::Mass::Executor::Run(*Processor, Context);
    };
    const auto Fire = [&](const int32 Count, const bool bSameAttack, const float Value)
    {
        FBBBProjectileSpawnLocalControlPacket Packet;
        Packet.LaunchTransform = FTransform(FVector(0, 0, 90));
        Packet.Mesh = NewObject<UStaticMesh>(World);
        Packet.Speed = 1000.0f;
        Packet.Radius = 0.0f;
        Packet.Damage = 1.0f;
        Packet.DurableDamage = 1.0f;
        Packet.StaggerValue = Value;
        Packet.Controller = Controller;
        Packet.bCanCauseDamage = true;
        for (int32 Index = 0; Index < Count; ++Index)
        {
            if (!bSameAttack)
            {
                Packet.AttackId = FGuid::NewGuid();
                Packet.AttackTime = FPlatformTime::Seconds();
            }
            const auto Bullet = Manager.CreateEntity(ProjectileType);
            TestTrue(TEXT("实际子弹出生输入有效"), Mass->SubmitInput(Bullet, Packet));
        }
        Mass->BeginCollisionFrame();
        Mass->AddCollisionBody({Target, FVector(50, 0, 90), 10.0f});
        Run(UBBBProjectileParseProcessor::StaticClass());
        Run(UBBBProjectileMovementProcessor::StaticClass());
        Run(UBBBProjectileCollisionProcessor::StaticClass());
        auto& Input = Manager.GetFragmentDataChecked<FBBBMonsterHealthInputFragment>(Target).Damage;
        TestTrue(TEXT("真实碰撞生成累计伤害输入"), Input.bActive && Input.Packet.IsValid());
        Input.Packet.Apply(Manager.GetFragmentDataChecked<FBBBMonsterDamageFragment>(Target));
        Input.bActive = false;
        Run(UBBBMonsterDamageProcessor::StaticClass());
    };

    Fire(1, false, 1.0f);
    auto& Mobility = Manager.GetFragmentDataChecked<FBBBMonsterMobilityFragment>(Target);
    TestTrue(TEXT("一生命值的实际子弹独立触发完整踉跄"), Mobility.IsStaggering(World->GetTimeSeconds()));
    const FGuid Identity = Mobility.StaggerAttackId;
    const float Start = Mobility.StaggerStartedAt;
    const float End = Mobility.StaggerEndsAt;
    Fire(3, false, 1.0f);
    TestEqual(TEXT("动作中三次独立开火不替换已成立动作"), Mobility.StaggerAttackId, Identity);
    TestEqual(TEXT("动作中不重启时间"), Mobility.StaggerStartedAt, Start);
    TestEqual(TEXT("动作中不延长时间"), Mobility.StaggerEndsAt, End);
    TestEqual(TEXT("全部实际子弹仍然计伤"), Manager.GetFragmentDataChecked<FBBBMonsterHealthFragment>(Target).CurrentHealth, 96.0f);
    TestEqual(TEXT("完整原片最后阶段仍停止主动水平运动"), Mobility.GetSpeedRatio(End - 0.001f), 0.0f);

    Manager.GetFragmentDataChecked<FBBBMonsterDamageFragment>(Target).Contributions.Reset();
    Mobility = FBBBMonsterMobilityFragment();
    Manager.GetFragmentDataChecked<FBBBMonsterHealthFragment>(Target) = FBBBMonsterHealthFragment();
    Fire(8, true, 1.0f);
    const auto& Contribution = Manager.GetFragmentDataChecked<FBBBMonsterDamageFragment>(Target).Contributions.FindChecked(7);
    TestEqual(TEXT("同发八弹丸累计八点部位伤害"), Contribution.Parts.Torso, 8.0);
    TestEqual(TEXT("同发多弹丸只成立一个反应身份"), Contribution.Stagger.ActiveAttackId, Contribution.Stagger.AttackId);
    TestTrue(TEXT("同发八弹丸已经消费唯一候选"), Contribution.Stagger.bEvaluated);

    Manager.GetFragmentDataChecked<FBBBMonsterDamageFragment>(Target).Contributions.Reset();
    Mobility = FBBBMonsterMobilityFragment();
    Manager.GetFragmentDataChecked<FBBBMonsterHealthFragment>(Target) = FBBBMonsterHealthFragment();
    Fire(1, false, 0.0f);
    TestFalse(TEXT("零踉跄值的实际子弹不触发旧重击途径"), Mobility.IsStaggering(World->GetTimeSeconds()));
    TestTrue(TEXT("未触发的真实命中仍有轻减速"), Mobility.GetSpeedRatio(World->GetTimeSeconds()) < 1.0f);
    return true;
}

#endif
