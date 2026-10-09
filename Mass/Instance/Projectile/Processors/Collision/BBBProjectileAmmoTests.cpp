#include "CoreMinimal.h"

#if WITH_DEV_AUTOMATION_TESTS && WITH_EDITOR

#include "Misc/AutomationTest.h"
#include "Misc/ScopeExit.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "Components/BoxComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/PlayerState.h"
#include "MassEntitySubsystem.h"
#include "MassExecutor.h"
#include "MassProcessingContext.h"
#include "BBBWork/UBBBNexus/Mass/Core/BBBMassSubsystem.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Projectile/Fragments/Spawn/BBBProjectileSpawnInputFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Projectile/Processors/Spawn/BBBProjectileParseProcessor.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Projectile/Processors/Movement/BBBProjectileMovementProcessor.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Projectile/Processors/Lifetime/BBBProjectileLifetimeProcessor.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Projectile/Processors/Collision/BBBProjectileCollisionProcessor.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Health/BBBMonsterHealthInputFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Network/BBBMonsterNetworkFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/HitReaction/BBBMonsterHitReactionInputFragment.h"

/** 验证散射弹丸 网格爆炸弹 重力 引信和本机伤害权限 */
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBBBProjectileAmmoTest, "UBBB.Mass.ProjectileAmmo",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FBBBProjectileAmmoTest::RunTest(const FString&)
{
    const auto Initialization = UWorld::InitializationValues().AllowAudioPlayback(false).CreatePhysicsScene(true)
        .CreateNavigation(false).CreateAISystem(false).ShouldSimulatePhysics(false);
    UWorld* World = UWorld::CreateWorld(EWorldType::Game, false, NAME_None, nullptr, true, ERHIFeatureLevel::Num, &Initialization);
    GEngine->CreateNewWorldContext(EWorldType::Game).SetCurrentWorld(World);
    ON_SCOPE_EXIT
    {
        GEngine->DestroyWorldContext(World);
        World->DestroyWorld(false);
    };

    auto* Mass = World->GetSubsystem<UBBBMassSubsystem>();
    auto& Manager = World->GetSubsystem<UMassEntitySubsystem>()->GetMutableEntityManager();
    const auto ProjectileType = Manager.CreateArchetype({FTransformFragment::StaticStruct(), FMassVelocityFragment::StaticStruct(),
        FBBBProjectileMotionFragment::StaticStruct(), FBBBProjectileCollisionFragment::StaticStruct(),
        FBBBProjectileLifetimeFragment::StaticStruct(), FBBBProjectilePresentationFragment::StaticStruct(),
        FBBBProjectileSpawnInputFragment::StaticStruct()});
    const auto MonsterType = Manager.CreateArchetype({FBBBMonsterDamageFragment::StaticStruct(),
        FBBBMonsterHealthInputFragment::StaticStruct(), FBBBMonsterNetworkFragment::StaticStruct(),
        FBBBMonsterHitReactionInputFragment::StaticStruct()});
    const auto A = Manager.CreateEntity(MonsterType);
    const auto B = Manager.CreateEntity(MonsterType);
    const auto C = Manager.CreateEntity(MonsterType);
    for (const auto Entity : {A, B, C})
    {
        Manager.GetFragmentDataChecked<FBBBMonsterNetworkFragment>(Entity).InstanceId = FGuid::NewGuid();
    }

    auto* Controller = World->SpawnActor<APlayerController>();
    auto* Player = World->SpawnActor<APlayerState>();
    Player->SetPlayerId(7);
    Controller->SetPlayerState(Player);
    FBBBProjectileSpawnLocalControlPacket Packet;
    Packet.Mesh = NewObject<UStaticMesh>(World);
    Packet.Speed = 1000.0f;
    Packet.Damage = 20.0f;
    Packet.Controller = Controller;
    Packet.bCanCauseDamage = true;
    TestTrue(TEXT("仅网格表现的弹丸出生有效"), Packet.IsValid());
    Packet.LocalDirection = FVector::ZeroVector;
    TestFalse(TEXT("无效发射方向拒绝"), Packet.IsValid());
    Packet.LocalDirection = FVector::ForwardVector;
    Packet.ExplosionRadiusCm = 40.0f;
    Packet.Penetrations = 1;
    TestFalse(TEXT("爆炸弹不得同时穿透计伤"), Packet.IsValid());
    Packet.Penetrations = 0;

    const auto Run = [&Manager, World](UClass* Type)
    {
        auto* Processor = NewObject<UMassProcessor>(World, Type);
        Processor->CallInitialize(World, Manager.AsShared());
        UE::Mass::FProcessingContext Context(Manager, 0.05f);
        UE::Mass::Executor::Run(*Processor, Context);
    };
    const auto Spawn = [&]()
    {
        const auto Entity = Manager.CreateEntity(ProjectileType);
        Mass->SubmitInput(Entity, Packet);
        Run(UBBBProjectileParseProcessor::StaticClass());
        return Entity;
    };
    const auto Damage = [&](FMassEntityHandle Target)
    {
        TArray<FBBBMonsterDamageContribution> Values;
        Mass->QueryDamage(Target, Values);
        double Total = 0.0;
        for (const auto& Value : Values)
        {
            Total += Value.Damage;
        }
        return Total;
    };
    const auto Reset = [&]()
    {
        for (const auto Target : {A, B, C})
        {
            Manager.GetFragmentDataChecked<FBBBMonsterHealthInputFragment>(Target).Damage.bActive = false;
        }
        Mass->BeginCollisionFrame();
        Mass->AddCollisionBody({A, FVector(40, 0, 0), 5.0f});
        Mass->AddCollisionBody({A, FVector(42, 0, 0), 5.0f});
        Mass->AddCollisionBody({B, FVector(60, 10, 0), 5.0f});
        Mass->AddCollisionBody({C, FVector(200, 0, 0), 5.0f});
    };

    Reset();
    TArray<FBBBMassCollisionBody> Overlap;
    Mass->OverlapEntities(FVector(33, 0, 0), 40.0f, Overlap);
    TestEqual(TEXT("空间桶与多部位碰撞体按实体去重"), Overlap.Num(), 2);
    const auto Rocket = Spawn();
    TestTrue(TEXT("弹体网格由出生输入交给表现片段"),
        Manager.GetFragmentDataChecked<FBBBProjectilePresentationFragment>(Rocket).Mesh == Packet.Mesh);
    Run(UBBBProjectileMovementProcessor::StaticClass());
    Run(UBBBProjectileCollisionProcessor::StaticClass());
    TestEqual(TEXT("直接命中只计算一次范围伤害"), Damage(A), 20.0);
    TestEqual(TEXT("旁边未直接命中的目标受到爆炸伤害"), Damage(B), 20.0);
    TestEqual(TEXT("范围外目标不受伤"), Damage(C), 0.0);
    Run(UBBBProjectileCollisionProcessor::StaticClass());
    TestEqual(TEXT("已引爆弹丸不能再次计伤"), Damage(B), 20.0);
    Manager.DestroyEntity(Rocket);

    Reset();
    Packet.bCanCauseDamage = false;
    const auto Mirror = Spawn();
    Run(UBBBProjectileMovementProcessor::StaticClass());
    Run(UBBBProjectileCollisionProcessor::StaticClass());
    TestEqual(TEXT("镜像爆炸不贡献范围伤害"), Damage(A) + Damage(B), 0.0);
    Manager.DestroyEntity(Mirror);
    Packet.bCanCauseDamage = true;

    Reset();
    const auto First = Spawn();
    const auto Second = Spawn();
    Run(UBBBProjectileMovementProcessor::StaticClass());
    Run(UBBBProjectileCollisionProcessor::StaticClass());
    TestEqual(TEXT("同轮两次爆炸完整合并累计贡献"), Damage(B), 40.0);
    Manager.DestroyEntity(First);
    Manager.DestroyEntity(Second);

    Reset();
    auto* Wall = World->SpawnActor<AActor>();
    auto* Box = NewObject<UBoxComponent>(Wall);
    Wall->SetRootComponent(Box);
    Box->SetBoxExtent(FVector(1, 30, 30));
    Box->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    Box->SetCollisionResponseToAllChannels(ECR_Ignore);
    Box->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
    Box->RegisterComponent();
    Wall->SetActorLocation(FVector(50, 0, 0));
    const auto Blocked = Spawn();
    Run(UBBBProjectileMovementProcessor::StaticClass());
    Run(UBBBProjectileCollisionProcessor::StaticClass());
    TestEqual(TEXT("墙前目标受到爆炸伤害"), Damage(A), 20.0);
    TestEqual(TEXT("墙后目标被遮挡"), Damage(B), 0.0);
    Manager.DestroyEntity(Blocked);
    Wall->Destroy();

    Reset();
    Packet.FuseSeconds = 0.1f;
    Packet.bDetonateOnImpact = false;
    const auto Fused = Spawn();
    Run(UBBBProjectileMovementProcessor::StaticClass());
    Run(UBBBProjectileCollisionProcessor::StaticClass());
    TestEqual(TEXT("引信弹接触时不立即造伤"), Damage(A), 0.0);
    TestTrue(TEXT("未反弹的引信弹接触后停止"), Manager.GetFragmentDataChecked<FBBBProjectileMotionFragment>(Fused).bResting);
    Run(UBBBProjectileLifetimeProcessor::StaticClass());
    Run(UBBBProjectileMovementProcessor::StaticClass());
    Run(UBBBProjectileCollisionProcessor::StaticClass());
    TestEqual(TEXT("停止运动后引信仍按时引爆"), Damage(B), 20.0);
    TestTrue(TEXT("引爆后请求回收"), Manager.GetFragmentDataChecked<FBBBProjectileLifetimeFragment>(Fused).bPendingDestroy);
    Manager.DestroyEntity(Fused);

    Reset();
    Packet.FuseSeconds = 1.0f;
    Packet.bBounceOnImpact = true;
    const auto Bounced = Spawn();
    Run(UBBBProjectileMovementProcessor::StaticClass());
    Run(UBBBProjectileCollisionProcessor::StaticClass());
    TestTrue(TEXT("反弹速度离开碰撞表面"), Manager.GetFragmentDataChecked<FMassVelocityFragment>(Bounced).Value.X < 0.0);
    TestFalse(TEXT("引信未到期的反弹弹丸继续存活"), Manager.GetFragmentDataChecked<FBBBProjectileLifetimeFragment>(Bounced).bPendingDestroy);
    Manager.DestroyEntity(Bounced);

    Packet.ExplosionRadiusCm = 0.0f;
    Packet.FuseSeconds = 0.0f;
    Packet.GravityScale = 1.0f;
    Packet.MuzzleTransform = FTransform(FRotator(0, 90, 0));
    Packet.LocalDirection = FVector(1, 1, 0).GetSafeNormal();
    const auto Ballistic = Spawn();
    Run(UBBBProjectileMovementProcessor::StaticClass());
    const auto Position = Manager.GetFragmentDataChecked<FTransformFragment>(Ballistic).GetTransform().GetLocation();
    TestTrue(TEXT("散射方向在枪口局部空间计算"), Position.X < 0.0 && Position.Y > 0.0);
    TestTrue(TEXT("重力轨迹使用世界重力和步进时间"), FMath::IsNearlyEqual(Position.Z, World->GetGravityZ() * 0.00125, 0.001));
    Manager.DestroyEntity(Ballistic);
    return true;
}

#endif
