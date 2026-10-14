#if WITH_DEV_AUTOMATION_TESTS && WITH_EDITOR
#include "Misc/AutomationTest.h"
#include "Misc/ScopeExit.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "Engine/StaticMesh.h"
#include "Components/SphereComponent.h"
#include "Components/BoxComponent.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Projectile/BBBThrowable.h"

/** 实际 Chaos 飞行和弹跳验证 房主引信及重复引爆验证 */
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBBBThrowablePhysicsTest, "BBB.Item.Throwable.Physics",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FBBBThrowablePhysicsTest::RunTest(const FString&)
{
    const auto Options = UWorld::InitializationValues().AllowAudioPlayback(false).CreatePhysicsScene(true)
        .CreateNavigation(false).CreateAISystem(false).ShouldSimulatePhysics(true);
    UWorld* World = UWorld::CreateWorld(EWorldType::Game, false, NAME_None, nullptr, true, ERHIFeatureLevel::Num, &Options);
    if (!TestNotNull(TEXT("隔离物理世界"), World))
    {
        return false;
    }
    GEngine->CreateNewWorldContext(EWorldType::Game).SetCurrentWorld(World);
    ON_SCOPE_EXIT
    {
        World->EndPlay(EEndPlayReason::Quit);
        GEngine->DestroyWorldContext(World);
        World->DestroyWorld(false);
        World->GetOutermost()->SetDirtyFlag(false);
    };
    auto* Definition = NewObject<UBBBThrowableDefinition>(World);
    Definition->Presentation.Mesh = LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Sphere.Sphere"));
    Definition->Launch.Speed = 100.0f;
    Definition->Launch.VelocityInheritance = 0.5f;
    Definition->Explosion.FuseSeconds = 1.5f;
    Definition->Explosion.Damage = 0.0f;
    Definition->Explosion.DurableDamage = 0.0f;
    auto* Material = NewObject<UPhysicalMaterial>(World);
    Material->Restitution = 0.65f;
    Material->Friction = 0.5f;
    Material->bOverrideRestitutionCombineMode = true;
    Material->RestitutionCombineMode = EFrictionCombineMode::Max;
    Definition->Physics.Material = Material;
    TestTrue(TEXT("有效物理投掷物配置"), Definition->IsValid());
    Definition->Explosion.FuseSeconds = 0.0f;
    TestFalse(TEXT("无引信且无接触引爆不能配置"), Definition->IsValid());
    Definition->Explosion.FuseSeconds = 1.5f;
    AActor* Floor = World->SpawnActor<AActor>();
    auto* Box = NewObject<UBoxComponent>(Floor);
    Floor->SetRootComponent(Box);
    Floor->AddInstanceComponent(Box);
    Box->SetBoxExtent(FVector(1000.0, 1000.0, 10.0));
    Box->SetCollisionProfileName(TEXT("BlockAll"));
    Box->RegisterComponent();
    World->InitializeActorsForPlay(FURL());
    World->SetBegunPlay(true);
    const FTransform Transform(FRotator::ZeroRotator, FVector(0.0, 0.0, 140.0));
    auto* Actor = World->SpawnActorDeferred<ABBBThrowable>(ABBBThrowable::StaticClass(), Transform);
    FBBBThrowableLaunchLocalControlPacket Launch;
    Launch.Definition = Definition;
    Launch.Transform = Transform;
    Launch.InheritedVelocity = FVector(100.0, 0.0, 0.0);
    TestTrue(TEXT("释放输入接受"), Actor->SubmitInput(Launch));
    Actor->FinishSpawning(Transform);
    Actor->Tick(0.0f);
    TestEqual(TEXT("引擎 Predictive Interpolation 模式"), Actor->GetPhysicsReplicationMode(), EPhysicsReplicationMode::PredictiveInterpolation);
    auto* Physics = Actor->FindComponentByClass<USphereComponent>();
    TestTrue(TEXT("Chaos 已启用"), Physics && Physics->IsSimulatingPhysics());
    TestTrue(TEXT("速度包含持有者继承"), FMath::IsNearlyEqual(Physics->GetPhysicsLinearVelocity().X, 150.0));
    Actor->SubmitInput(Launch);
    Actor->Tick(0.0f);
    TestTrue(TEXT("重复释放不重设出生时间"), Actor->RuntimeData.Lifetime.ReadLifetimeState().bLaunched);
    bool bFell = false;
    bool bBounced = false;
    for (int32 Frame = 0; Frame < 100; ++Frame)
    {
        // 独立测试在一次编辑器帧内推进多帧 必须推进引擎帧号让物理 Tick 再次调度
        ++GFrameCounter;
        World->Tick(LEVELTICK_All, 1.0f / 60.0f);
        const double Velocity = Physics->GetPhysicsLinearVelocity().Z;
        bFell |= Velocity < -50.0;
        bBounced |= bFell && Velocity > 50.0;
    }
    TestTrue(TEXT("世界重力实际下落"), bFell);
    TestTrue(TEXT("Chaos 碰撞实际反弹"), bBounced);
    TestTrue(TEXT("引信产生唯一终态"), Actor->RuntimeData.Explosion.ReadExplosionState().Fact.bExploded);
    const double ExplosionTime = Actor->RuntimeData.Explosion.ReadExplosionState().Fact.Time;
    Actor->SubmitInput(FBBBThrowableDetonateLocalControlPacket{});
    Actor->Tick(0.0f);
    TestEqual(TEXT("重复触发不重演爆炸"), Actor->RuntimeData.Explosion.ReadExplosionState().Fact.Time, ExplosionTime);
    TestFalse(TEXT("爆炸停止物理"), Physics->IsSimulatingPhysics());
    return !HasAnyErrors();
}
#endif
