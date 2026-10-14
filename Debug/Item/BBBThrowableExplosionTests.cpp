#if WITH_DEV_AUTOMATION_TESTS && WITH_EDITOR
#include "Misc/AutomationTest.h"
#include "Misc/ScopeExit.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "Engine/LocalPlayer.h"
#include "Engine/StaticMesh.h"
#include "Components/SphereComponent.h"
#include "GameFramework/PlayerState.h"
#include "MassEntitySubsystem.h"
#include "GameFramework/PlayerController.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Projectile/BBBThrowable.h"
#include "BBBWork/UBBBNexus/Mass/Core/BBBMassSubsystem.h"
#include "BBBWork/UBBBNexus/Mass/Network/BBBMassNetworkActor.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Config/BBBMonsterDefinition.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Network/BBBMonsterNetworkFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Health/BBBMonsterHealthInputFragment.h"

/** 真实爆炸链路验证 Mass 伤害累计 单次记账与刚体冲击 */
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBBBThrowableExplosionTest, "BBB.Item.Throwable.Explosion",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FBBBThrowableExplosionTest::RunTest(const FString&)
{
    const auto Options = UWorld::InitializationValues().AllowAudioPlayback(false).CreatePhysicsScene(true)
        .CreateNavigation(false).CreateAISystem(false).ShouldSimulatePhysics(true);
    UWorld* World = UWorld::CreateWorld(EWorldType::Game, false, NAME_None, nullptr, true, ERHIFeatureLevel::Num, &Options);
    if (!TestNotNull(TEXT("爆炸隔离世界"), World))
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
    auto* Controller = World->SpawnActor<APlayerController>();
    auto* Player = NewObject<ULocalPlayer>(GEngine);
    Player->PlayerController = Controller;
    Controller->Player = Player;
    Controller->SetPlayerState(World->SpawnActor<APlayerState>());
    Controller->PlayerState->SetPlayerId(17);
    auto& Manager = World->GetSubsystem<UMassEntitySubsystem>()->GetMutableEntityManager();
    const auto Archetype = Manager.CreateArchetype({FBBBMonsterNetworkFragment::StaticStruct(),
        FBBBMonsterDamageFragment::StaticStruct(), FBBBMonsterHealthInputFragment::StaticStruct()});
    const auto Entity = Manager.CreateEntity(Archetype);
    auto& Identity = Manager.GetFragmentDataChecked<FBBBMonsterNetworkFragment>(Entity);
    Identity.InstanceId = FGuid::NewGuid();
    Identity.Definition = NewObject<UBBBMonsterDefinition>(World);
    auto* Network = World->SpawnActor<ABBBMassNetworkActor>();
    FBBBMonsterReplicationItem Published;
    Published.InstanceId = Identity.InstanceId;
    Published.Definition = Identity.Definition.Get();
    Published.Location = FVector(100.0, 0.0, 100.0);
    Network->BeginPublish();
    Network->Publish(Published, Entity);
    Network->EndPublish();
    AActor* Prop = World->SpawnActor<AActor>();
    auto* Sphere = NewObject<USphereComponent>(Prop);
    Prop->SetRootComponent(Sphere);
    Prop->AddInstanceComponent(Sphere);
    Sphere->SetSphereRadius(8.0f);
    Sphere->SetCollisionProfileName(TEXT("PhysicsActor"));
    Sphere->RegisterComponent();
    Prop->SetActorLocation(FVector(30.0, 0.0, 100.0));
    Sphere->SetSimulatePhysics(true);
    Sphere->SetEnableGravity(false);
    World->InitializeActorsForPlay(FURL());
    World->SetBegunPlay(true);
    // 新建刚体进入物理查询场景后再执行爆炸
    ++GFrameCounter;
    World->Tick(LEVELTICK_All, 1.0f / 60.0f);
    Network->BeginPublish();
    Network->Publish(Published, Entity);
    Network->EndPublish();
    auto* Definition = NewObject<UBBBThrowableDefinition>(World);
    Definition->Presentation.Mesh = LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Sphere.Sphere"));
    Definition->Explosion.bCheckOcclusion = false;
    Definition->Physics.bEnableGravity = false;
    const FTransform Transform(FRotator::ZeroRotator, FVector(0.0, 0.0, 100.0));
    auto* Actor = World->SpawnActorDeferred<ABBBThrowable>(ABBBThrowable::StaticClass(), Transform, Controller);
    FBBBThrowableLaunchLocalControlPacket Launch;
    Launch.Definition = Definition;
    Launch.Transform = Transform;
    Actor->SubmitInput(Launch);
    Actor->FinishSpawning(Transform);
    Actor->Tick(0.0f);
    auto* Mass = World->GetSubsystem<UBBBMassSubsystem>();
    FBBBMassCollisionBody Body;
    Body.Entity = Entity;
    Body.Center = Published.Location;
    Body.Radius = 25.0f;
    Mass->AddCollisionBody(Body);
    Actor->SubmitInput(FBBBThrowableDetonateLocalControlPacket{});
    Actor->Tick(0.0f);
    const auto& Result = Actor->RuntimeData.Explosion.ReadExplosionState();
    TestEqual(TEXT("爆炸只产生一个逻辑目标结果"), Result.Fact.Targets.Num(), 1);
    TArray<FBBBMonsterDamageContribution> Contributions;
    TestTrue(TEXT("实际伤害累计入口可读"), Mass->QueryDamage(Entity, Contributions));
    const double Damage = Contributions.IsEmpty() ? 0.0 : Contributions[0].Parts.Sum();
    TestTrue(TEXT("爆炸对 Mass 目标实际产生伤害"), Damage > 0.0);
    TestEqual(TEXT("伤害属于投掷者"), Contributions.IsEmpty() ? INDEX_NONE : Contributions[0].PlayerId, 17);
    ++GFrameCounter;
    World->Tick(LEVELTICK_All, 1.0f / 60.0f);
    TestTrue(TEXT("物理道具收到向外冲击"), Sphere->GetPhysicsLinearVelocity().X > 0.0);
    Actor->SubmitInput(FBBBThrowableDetonateLocalControlPacket{});
    Actor->Tick(0.0f);
    Mass->QueryDamage(Entity, Contributions);
    TestEqual(TEXT("重复通知不会重复记账"), Contributions.IsEmpty() ? 0.0 : Contributions[0].Parts.Sum(), Damage);
    return !HasAnyErrors();
}
#endif
