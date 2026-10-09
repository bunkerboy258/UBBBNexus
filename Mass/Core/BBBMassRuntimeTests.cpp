#include "CoreMinimal.h"

#if WITH_DEV_AUTOMATION_TESTS && WITH_EDITOR

#include "Misc/AutomationTest.h"
#include "HAL/IConsoleManager.h"
#include "Misc/ScopeExit.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/PlayerState.h"
#include "MassCommonFragments.h"
#include "MassMovementFragments.h"
#include "MassExecutor.h"
#include "MassProcessingContext.h"
#include "MassExecutionContext.h"
#include "NiagaraDataChannel.h"
#include "NiagaraSystem.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraDataChannelFunctionLibrary.h"
#include "NiagaraDataChannelHandler.h"
#include "NiagaraDataChannelData.h"
#include "NiagaraDataSet.h"
#include "NiagaraDataSetAccessor.h"
#include "NiagaraSystemInstanceController.h"
#include "NiagaraEmitterInstance.h"
#include "Misc/App.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Projectile/Presentation/BBBProjectilePresentation.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Projectile/Fragments/Collision/BBBProjectileImpact.h"
#include "BBBWork/UBBBNexus/Mass/Core/BBBMassSubsystem.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Health/BBBMonsterHealthInputFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Health/BBBMonsterHealthFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Network/BBBMonsterNetworkInputFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Processors/Input/BBBMonsterParseProcessor.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Processors/Health/BBBMonsterDamageProcessor.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Processors/Health/BBBMonsterLifecycleProcessor.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Health/BBBMonsterDeathFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Projectile/Fragments/Spawn/BBBProjectileSpawnInputFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Projectile/Processors/Spawn/BBBProjectileParseProcessor.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Projectile/Processors/Movement/BBBProjectileMovementProcessor.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Projectile/Processors/Collision/BBBProjectileCollisionProcessor.h"
#include "MassActorSubsystem.h"
#include "Components/SkeletalMeshComponent.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Presentation/BBBMonsterPresentationSmoothingFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Presentation/BBBMonsterPresentationStateFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Movement/BBBMonsterAvoidanceFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Tags/BBBMonsterTag.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Presentation/BBBMonsterPresentationActor.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Presentation/BBBMonsterPresentationComponent.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Presentation/BBBMonsterFactAnimInstance.h"
#include "UObject/UnrealType.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Processors/Presentation/BBBMonsterPresentationProcessor.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Processors/Collision/BBBMonsterCollisionProcessor.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/HitReaction/BBBMonsterHitReactionInputFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Movement/BBBMonsterMobilityFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Perception/BBBMonsterPerceptionInputFragment.h"

/** 隔离世界验证覆盖输入 枪口运动 逻辑碰撞与伤害权限 */
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBBBMassRuntimeTest, "UBBB.Mass.Runtime",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FBBBMassRuntimeTest::RunTest(const FString& Parameters)
{
    const auto Initialization = UWorld::InitializationValues()
        .AllowAudioPlayback(false).CreatePhysicsScene(true).CreateNavigation(false)
        .CreateAISystem(false).ShouldSimulatePhysics(false);
    UWorld* World = UWorld::CreateWorld(EWorldType::Game, false, NAME_None, nullptr, true, ERHIFeatureLevel::Num, &Initialization);
    if (!TestNotNull(TEXT("隔离世界"), World))
    {
        return false;
    }
    GEngine->CreateNewWorldContext(EWorldType::Game).SetCurrentWorld(World);
    ON_SCOPE_EXIT
    {
        World->WorldType = EWorldType::Game;
        GEngine->DestroyWorldContext(World);
        World->DestroyWorld(false);
    };
    UBBBMassSubsystem* Mass = World->GetSubsystem<UBBBMassSubsystem>();
    FMassEntityManager& Manager = World->GetSubsystem<UMassEntitySubsystem>()->GetMutableEntityManager();
    const FMassArchetypeHandle MonsterType = Manager.CreateArchetype({
        FTransformFragment::StaticStruct(), FMassVelocityFragment::StaticStruct(),
        FBBBMonsterHealthFragment::StaticStruct(), FBBBMonsterDamageFragment::StaticStruct(),
        FBBBMonsterHealthInputFragment::StaticStruct(), FBBBMonsterNetworkInputFragment::StaticStruct(),
        FBBBMonsterNetworkFragment::StaticStruct(), FBBBMonsterBehaviorFragment::StaticStruct(),
        FBBBMonsterDeathFragment::StaticStruct(), FBBBMonsterTag::StaticStruct(), FBBBMonsterMobilityFragment::StaticStruct(),
        FBBBMonsterHitReactionFragment::StaticStruct(), FBBBMonsterHitReactionInputFragment::StaticStruct(),
        FBBBMonsterPerceptionInputFragment::StaticStruct(), FBBBMonsterStimulusFragment::StaticStruct()
    });
    const FMassEntityHandle Monster = Manager.CreateEntity(MonsterType);
    const FGuid InstanceId = FGuid::NewGuid();
    Manager.GetFragmentDataChecked<FBBBMonsterNetworkFragment>(Monster).InstanceId = InstanceId;
    const auto Run = [&Manager, World](UClass* Type)
    {
        UMassProcessor* Processor = NewObject<UMassProcessor>(World, Type);
        Processor->CallInitialize(World, Manager.AsShared());
        UE::Mass::FProcessingContext Context(Manager, 0.05f);
        UE::Mass::Executor::Run(*Processor, Context);
    };
    const auto Health = [&Manager, Monster]()
    {
        return Manager.GetFragmentDataChecked<FBBBMonsterHealthFragment>(Monster).CurrentHealth;
    };
    const auto Consume = [&Run]()
    {
        Run(UBBBMonsterParseProcessor::StaticClass());
        Run(UBBBMonsterDamageProcessor::StaticClass());
    };

    FBBBMonsterDamageLocalControlPacket Damage;
    Damage.Include({1, 10.0});
    TestTrue(TEXT("首次投递"), Mass->SubmitInput(Monster, Damage));
    Damage.Include({1, 25.0});
    Mass->SubmitInput(Monster, Damage);
    TestEqual(TEXT("投递不直接扣血"), Health(), 100.0f);
    Manager.AddFragmentToEntity(Monster, FMassActorFragment::StaticStruct());
    TestTrue(TEXT("实体换 Archetype 后覆盖快照仍完整"),
        Manager.GetFragmentDataChecked<FBBBMonsterHealthInputFragment>(Monster).Damage.Packet.Contributions[0].Damage == 25.0);
    Consume();
    TestEqual(TEXT("覆盖后的累计结果包含前一次命中"), Health(), 75.0f);
    Consume();
    TestEqual(TEXT("消费后不重复扣血"), Health(), 75.0f);
    TestEqual(TEXT("字典原生深拷贝保留贡献"),
        Manager.GetFragmentDataChecked<FBBBMonsterDamageFragment>(Monster).Contributions.FindChecked(1).Damage, 25.0);

    // 测试夹具重新开始独立场景 运行时不允许清空累计贡献
    Manager.GetFragmentDataChecked<FBBBMonsterDamageFragment>(Monster).Contributions.Reset();
    Consume();
    Damage.Contributions.Reset();
    Damage.Include({1, 20.0});
    Mass->SubmitInput(Monster, Damage);
    FBBBMonsterDamageRemoteMessagePacket Remote;
    Remote.InstanceId = InstanceId;
    TestTrue(TEXT("查询包含待消费的本机贡献"), Mass->QueryDamage(Monster, Remote.Contributions));
    Remote.Include({2, 20.0});
    Mass->SubmitInput(Monster, Remote);
    Consume();
    TestEqual(TEXT("两名玩家同时各造成二十伤害"), Health(), 60.0f);
    Mass->SubmitInput(Monster, Remote);
    Consume();
    TestEqual(TEXT("重复累计消息不重复扣血"), Health(), 60.0f);
    Remote.Contributions.Reset();
    Remote.Include({1, 5.0});
    Remote.Include({2, 10.0});
    Mass->SubmitInput(Monster, Remote);
    Consume();
    TestEqual(TEXT("迟到的小累计值不恢复生命"), Health(), 60.0f);

    Damage.Include({1, 40.0});
    Mass->SubmitInput(Monster, Damage);
    Remote.Contributions.Reset();
    Remote.Include({1, 20.0});
    Remote.Include({2, 20.0});
    Mass->SubmitInput(Monster, Remote);
    Consume();
    TestEqual(TEXT("主机旧字典不能覆盖本机未回显贡献"), Health(), 40.0f);

    const FMassArchetypeHandle ProjectileType = Manager.CreateArchetype({
        FTransformFragment::StaticStruct(), FMassVelocityFragment::StaticStruct(),
        FBBBProjectileMotionFragment::StaticStruct(), FBBBProjectileCollisionFragment::StaticStruct(),
        FBBBProjectileLifetimeFragment::StaticStruct(), FBBBProjectilePresentationFragment::StaticStruct(),
        FBBBProjectileSpawnInputFragment::StaticStruct()
    });
    UNiagaraDataChannelAsset* TestChannel = NewObject<UNiagaraDataChannelAsset>(World);
    UNiagaraSystem* TestSystem = NewObject<UNiagaraSystem>(World);
    UNiagaraDataChannelAsset* TestImpactChannel = LoadObject<UNiagaraDataChannelAsset>(nullptr,
        TEXT("/Game/_Project/System/Mass/Projectile/NDC_BBBProjectileImpact.NDC_BBBProjectileImpact"));
    if (!TestNotNull(TEXT("正式命中通道"), TestImpactChannel))
    {
        return false;
    }
    APlayerController* Controller = World->SpawnActor<APlayerController>();
    APlayerState* Player = World->SpawnActor<APlayerState>();
    Player->SetPlayerId(1);
    Controller->SetPlayerState(Player);
    const auto Shoot = [&](bool bDamage, int32 Count = 1, float Amount = 20.0f)
    {
        TArray<FMassEntityHandle> Projectiles;
        for (int32 Index = 0; Index < Count; ++Index)
        {
            const FMassEntityHandle Projectile = Manager.CreateEntity(ProjectileType);
            Projectiles.Add(Projectile);
            FBBBProjectileSpawnLocalControlPacket Spawn;
            Spawn.MuzzleTransform = FTransform(FRotator(0.0f, 90.0f, 0.0f), FVector::ZeroVector);
            Spawn.Speed = 1000.0f;
            Spawn.Damage = Amount;
            Spawn.Channel = TestChannel;
            Spawn.System = TestSystem;
            Spawn.ImpactChannel = TestImpactChannel;
            Spawn.Controller = Controller;
            Spawn.bCanCauseDamage = bDamage;
            Mass->SubmitInput(Projectile, Spawn);
        }
        Run(UBBBProjectileParseProcessor::StaticClass());
        Run(UBBBProjectileMovementProcessor::StaticClass());
        Mass->BeginCollisionFrame();
        FBBBMassCollisionBody Body;
        Body.Entity = Monster;
        Body.Center = FVector(0.0f, 40.0f, 0.0f);
        Body.Radius = 5.0f;
        Body.Surface = SurfaceType2;
        Mass->AddCollisionBody(Body);
        FMassEntityHandle HitEntity;
        float HitTime;
        FVector HitPosition;
        FVector HitNormal;
        EPhysicalSurface HitSurface;
        uint8 HitPart = 0;
        TestTrue(TEXT("球体扫掠返回接触几何"), Mass->TraceEntities(FVector::ZeroVector,
            FVector(0.0f, 50.0f, 0.0f), 2.0f, {}, HitEntity, HitTime, HitPosition, HitNormal, HitSurface, HitPart));
        TestTrue(TEXT("表面接触点不是扫掠球心"), HitPosition.Equals(FVector(0.0f, 35.0f, 0.0f), 0.001));
        TestTrue(TEXT("表面法线朝向入射侧"), HitNormal.Equals(FVector(0.0f, -1.0f, 0.0f), 0.001));
        TestEqual(TEXT("查询不读取小怪内部数据即可取得血肉表面"), HitSurface, SurfaceType2);
        TestTrue(TEXT("球心命中时间保持原有连续扫掠语义"), FMath::IsNearlyEqual(HitTime, 0.66f, 0.001f));
        Run(UBBBProjectileCollisionProcessor::StaticClass());
        for (const auto Projectile : Projectiles)
        {
            TestTrue(TEXT("沿本机枪口 X 运动并在逻辑碰撞处结束"),
                Manager.GetFragmentDataChecked<FBBBProjectileMotionFragment>(Projectile).PreviousLocation.IsNearlyZero()
                && Manager.GetFragmentDataChecked<FBBBProjectileLifetimeFragment>(Projectile).bPendingDestroy);
        }
        Consume();
        for (const auto Projectile : Projectiles)
        {
            Manager.DestroyEntity(Projectile);
        }
    };
    Shoot(false);
    TestEqual(TEXT("镜像子弹不造伤"), Health(), 40.0f);
    Shoot(true);
    TestEqual(TEXT("本机控制子弹贡献二十"), Health(), 20.0f);
    Shoot(true, 2, 5.0f);
    TestEqual(TEXT("同轮两颗子弹全部计入累计值"), Health(), 10.0f);

    Remote.Include({2, 40.0});
    Mass->SubmitInput(Monster, Remote);
    Consume();
    TestEqual(TEXT("合计贡献达到上限立即归零"), Health(), 0.0f);
    Remote.Contributions.Reset();
    Remote.Include({1, 20.0});
    Remote.Include({2, 20.0});
    Mass->SubmitInput(Monster, Remote);
    Consume();
    TestEqual(TEXT("旧字典不能复活"), Health(), 0.0f);

    FBBBMonsterStateAuthorityFactPacket Fact;
    Fact.InstanceId = InstanceId;
    Fact.Revision = 2;
    Fact.Behavior = EBBBMonsterBehavior::Dead;
    Mass->SubmitInput(Monster, Fact);
    Run(UBBBMonsterParseProcessor::StaticClass());
    TestTrue(TEXT("远端死亡标记不代替本机生命判定"),
        Manager.GetFragmentDataChecked<FBBBMonsterBehaviorFragment>(Monster).State != EBBBMonsterBehavior::Dead);
    Manager.GetFragmentDataChecked<FBBBMonsterBehaviorFragment>(Monster).State = EBBBMonsterBehavior::Dead;
    Fact.Behavior = EBBBMonsterBehavior::Chase;
    Fact.Revision = 3;
    Mass->SubmitInput(Monster, Fact);
    Run(UBBBMonsterParseProcessor::StaticClass());
    TestEqual(TEXT("迟到行动不能恢复死亡"),
        Manager.GetFragmentDataChecked<FBBBMonsterBehaviorFragment>(Monster).State, EBBBMonsterBehavior::Dead);
    Fact.Revision = 1;
    Mass->SubmitInput(Monster, Fact);
    Run(UBBBMonsterParseProcessor::StaticClass());
    TestEqual(TEXT("过期版本不覆盖"),
        Manager.GetFragmentDataChecked<FBBBMonsterNetworkFragment>(Monster).ReceivedRevision, 3u);
    Fact.InstanceId = FGuid::NewGuid();
    Fact.Revision = 4;
    Mass->SubmitInput(Monster, Fact);
    Run(UBBBMonsterParseProcessor::StaticClass());
    TestEqual(TEXT("错误实例身份不覆盖"),
        Manager.GetFragmentDataChecked<FBBBMonsterNetworkFragment>(Monster).ReceivedRevision, 3u);

    World->WorldType = EWorldType::PIE;
    World->SetPlayInEditorInitialNetMode(NM_Client);
    Manager.GetFragmentDataChecked<FBBBMonsterDeathFragment>(Monster).DestroyAtTime = 0.0f;
    Run(UBBBMonsterLifecycleProcessor::StaticClass());
    TestTrue(TEXT("尚未交接最终贡献不能回收"), Manager.IsEntityValid(Monster));
    Manager.GetFragmentDataChecked<FBBBMonsterNetworkFragment>(Monster).bDamageSubmitted = true;
    Run(UBBBMonsterLifecycleProcessor::StaticClass());
    TestFalse(TEXT("交接后允许安全回收"), Manager.IsEntityValid(Monster));
    TestFalse(TEXT("回收实体拒绝旧输入"), Mass->SubmitInput(Monster, Damage));
    World->WorldType = EWorldType::Game;

    const FMassEntityHandle NewMonster = Manager.CreateEntity(MonsterType);
    Manager.GetFragmentDataChecked<FBBBMonsterNetworkFragment>(NewMonster).InstanceId = FGuid::NewGuid();
    Mass->SubmitInput(NewMonster, Remote);
    Consume();
    TestEqual(TEXT("旧怪消息不能影响新怪"),
        Manager.GetFragmentDataChecked<FBBBMonsterHealthFragment>(NewMonster).CurrentHealth, 100.0f);
    Manager.DestroyEntity(NewMonster);
    return true;
}


/** 隔离客机世界验证显示追靠 首次对齐 碰撞分离与即时动画状态 */
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBBBMonsterPresentationSmoothingTest, "UBBB.Mass.PresentationSmoothing",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FBBBMonsterPresentationSmoothingTest::RunTest(const FString& Parameters)
{
    const auto Initialization = UWorld::InitializationValues()
        .AllowAudioPlayback(false)
        .CreatePhysicsScene(true)
        .CreateNavigation(false)
        .CreateAISystem(false)
        .ShouldSimulatePhysics(false);
    UWorld* World = UWorld::CreateWorld(EWorldType::Game, false, NAME_None, nullptr, true, ERHIFeatureLevel::Num, &Initialization);

    if (!TestNotNull(TEXT("平滑验证世界"), World))
    {
        return false;
    }

    GEngine->CreateNewWorldContext(EWorldType::Game).SetCurrentWorld(World);
    ON_SCOPE_EXIT
    {
        World->WorldType = EWorldType::Game;
        GEngine->DestroyWorldContext(World);
        World->DestroyWorld(false);
    };

    // 使用引擎的 PIE 初始网络模式测试真实客机分支 不建立测试网络连接
    World->WorldType = EWorldType::PIE;
    World->SetPlayInEditorInitialNetMode(NM_Client);
    TestEqual(TEXT("进入客机分支"), World->GetNetMode(), NM_Client);

    UClass* ActorClass = LoadClass<ABBBMonsterPresentationActor>(nullptr,
        TEXT("/Game/_Project/System/Mass/Monster/Zombie/Male/BP_BBBZombieMalePresentation.BP_BBBZombieMalePresentation_C"));

    if (!TestNotNull(TEXT("使用正式男丧尸表现蓝图"), ActorClass))
    {
        return false;
    }

    ABBBMonsterPresentationActor* Actor = World->SpawnActor<ABBBMonsterPresentationActor>(ActorClass);

    if (!TestNotNull(TEXT("生成测试表现演员"), Actor))
    {
        return false;
    }

    FMassEntityManager& Manager = World->GetSubsystem<UMassEntitySubsystem>()->GetMutableEntityManager();
    const FMassArchetypeHandle Type = Manager.CreateArchetype({
        FTransformFragment::StaticStruct(), FMassVelocityFragment::StaticStruct(),
        FMassActorFragment::StaticStruct(), FBBBMonsterTag::StaticStruct(),
        FBBBMonsterAvoidanceFragment::StaticStruct(), FBBBMonsterHealthFragment::StaticStruct(),
        FBBBMonsterPresentationStateFragment::StaticStruct(), FBBBMonsterPresentationSmoothingFragment::StaticStruct(),
        FBBBMonsterHitReactionFragment::StaticStruct(), FBBBMonsterMobilityFragment::StaticStruct(), FBBBMonsterNetworkFragment::StaticStruct()
    });
    const FMassEntityHandle Entity = Manager.CreateEntity(Type);
    Manager.GetFragmentDataChecked<FMassActorFragment>(Entity).SetNoHandleMapUpdate(Entity, Actor, true);
    Manager.GetFragmentDataChecked<FBBBMonsterAvoidanceFragment>(Entity).CollisionRadius = 1.0f;
    FTransform& Target = Manager.GetFragmentDataChecked<FTransformFragment>(Entity).GetMutableTransform();
    Target = FTransform(FRotator(0.0f, 179.0f, 0.0f), FVector(100.0f, 20.0f, 90.0f));
    const auto Run = [&Manager, World](UClass* ProcessorClass, const float DeltaTime = 0.05f)
    {
        UMassProcessor* Processor = NewObject<UMassProcessor>(World, ProcessorClass);
        Processor->CallInitialize(World, Manager.AsShared());
        UE::Mass::FProcessingContext Context(Manager, DeltaTime);
        UE::Mass::Executor::Run(*Processor, Context);
    };

    Run(UBBBMonsterPresentationProcessor::StaticClass());
    TestTrue(TEXT("首次生成直接对齐"), Actor->GetActorLocation().Equals(Target.GetLocation(), 0.001));
    TestTrue(TEXT("首次朝向直接对齐"), Actor->GetActorQuat().Equals(Target.GetRotation(), 0.0001));

    Target.SetLocation(FVector(200.0f, 20.0f, 90.0f));
    Target.SetRotation(FRotator(0.0f, -179.0f, 0.0f).Quaternion());
    Manager.GetFragmentDataChecked<FMassVelocityFragment>(Entity).Value = FVector(100.0f, 0.0f, 0.0f);
    Run(UBBBMonsterPresentationProcessor::StaticClass());
    const FVector FirstDisplay = Actor->GetActorLocation();
    TestTrue(TEXT("客机显示在旧位置与最新结果之间"), FirstDisplay.X > 100.0 && FirstDisplay.X < 200.0);
    TestTrue(TEXT("显示步长符合零点一秒时间尺度"), FMath::IsNearlyEqual(FirstDisplay.X, 100.0 + 100.0 * (1.0 - FMath::Exp(-0.5)), 0.001));
    TestTrue(TEXT("逻辑位置不受显示平滑影响"), Target.GetLocation().Equals(FVector(200.0f, 20.0f, 90.0f), 0.001));
    TestTrue(TEXT("跨正负一百八十度走最短转向"), FMath::Abs(Actor->GetActorRotation().Yaw) > 179.0);
    TestTrue(TEXT("逻辑速度不被改写"), Manager.GetFragmentDataChecked<FMassVelocityFragment>(Entity).Value.Equals(FVector(100.0f, 0.0f, 0.0f)));

    Run(UBBBMonsterCollisionProcessor::StaticClass());
    UBBBMassSubsystem* Mass = World->GetSubsystem<UBBBMassSubsystem>();
    FMassEntityHandle Hit;
    float HitTime = 0.0f;
    FVector HitPosition;
    FVector HitNormal;
    EPhysicalSurface HitSurface;
    uint8 HitPart = 0;
    TestTrue(TEXT("碰撞立即使用最新逻辑位置"), Mass->TraceEntities(
        Target.GetLocation() - FVector(0.0f, 0.0f, 2.0f), Target.GetLocation() + FVector(0.0f, 0.0f, 2.0f),
        0.0f, {}, Hit, HitTime, HitPosition, HitNormal, HitSurface, HitPart));
    TestTrue(TEXT("逻辑碰撞命中正确实体"), Hit == Entity);
    TestFalse(TEXT("显示位置没有生成第二套碰撞"), Mass->TraceEntities(
        FirstDisplay - FVector(0.0f, 0.0f, 2.0f), FirstDisplay + FVector(0.0f, 0.0f, 2.0f),
        0.0f, {}, Hit, HitTime, HitPosition, HitNormal, HitSurface, HitPart));

    auto& State = Manager.GetFragmentDataChecked<FBBBMonsterPresentationStateFragment>(Entity);
    State.State = EBBBMonsterBehavior::Alert;
    State.ActionId = 1;
    State.ActionProgress = 0.5f;
    Run(UBBBMonsterPresentationProcessor::StaticClass());
    TestEqual(TEXT("警觉状态不等待位置收敛"), Actor->GetMonsterPresentation()->GetBBBMonsterBehavior(), EBBBMonsterBehavior::Alert);
    UBBBMonsterFactAnimInstance* const Animation = Cast<UBBBMonsterFactAnimInstance>(Actor->GetMonsterMesh()->GetAnimInstance());
    if (!TestNotNull(TEXT("正式表现仅使用动画蓝图实例"), Animation))
    {
        return false;
    }

    Animation->NativeUpdateAnimation(0.0f);
    const FFloatProperty* const Progress = FindFProperty<FFloatProperty>(Animation->GetClass(), TEXT("ActionProgressFact"));
    if (!TestNotNull(TEXT("正式动画的动作进度事实"), Progress))
    {
        return false;
    }
    TestEqual(TEXT("警觉循环姿势不使用动作进度"), Progress->GetPropertyValue_InContainer(Animation), 0.0f);
    TestTrue(TEXT("警觉时显示仍在追靠"), Actor->GetActorLocation().X < 200.0);
    State.State = EBBBMonsterBehavior::Attack;
    ++State.ActionId;
    Run(UBBBMonsterPresentationProcessor::StaticClass());
    TestEqual(TEXT("攻击状态立即响应"), Actor->GetMonsterPresentation()->GetBBBMonsterBehavior(), EBBBMonsterBehavior::Attack);
    Animation->NativeUpdateAnimation(0.0f);
    TestEqual(TEXT("攻击动画立即应用逻辑进度"), Progress->GetPropertyValue_InContainer(Animation), 0.5f);
    State.State = EBBBMonsterBehavior::Dead;
    ++State.ActionId;
    Run(UBBBMonsterPresentationProcessor::StaticClass());
    TestEqual(TEXT("死亡状态立即响应"), Actor->GetMonsterPresentation()->GetBBBMonsterBehavior(), EBBBMonsterBehavior::Dead);

    for (int32 Index = 0; Index < 20; ++Index)
    {
        Run(UBBBMonsterPresentationProcessor::StaticClass());
    }

    TestTrue(TEXT("目标停止后显示收敛且不外推"), Actor->GetActorLocation().Equals(Target.GetLocation(), 0.01));
    TestTrue(TEXT("停止后朝向收敛"), Actor->GetActorQuat().Equals(Target.GetRotation(), 0.0001));
    Target.SetLocation(FVector(800.0f, 20.0f, 90.0f));
    Run(UBBBMonsterPresentationProcessor::StaticClass());
    TestTrue(TEXT("超过五百厘米直接纠正"), Actor->GetActorLocation().Equals(Target.GetLocation(), 0.001));

    Target.SetLocation(FVector(810.0f, 20.0f, 90.0f));
    Run(UBBBMonsterPresentationProcessor::StaticClass(), 0.0f);
    TestTrue(TEXT("零时间步不移动显示"), Actor->GetActorLocation().Equals(FVector(800.0f, 20.0f, 90.0f), 0.001));
    Run(UBBBMonsterPresentationProcessor::StaticClass(), 1.0f);
    TestTrue(TEXT("长帧不越过目标"), Actor->GetActorLocation().X <= 810.0);

    ABBBMonsterPresentationActor* Replacement = World->SpawnActor<ABBBMonsterPresentationActor>(ActorClass);

    if (!TestNotNull(TEXT("生成替换表现演员"), Replacement))
    {
        return false;
    }

    Manager.GetFragmentDataChecked<FMassActorFragment>(Entity).ResetNoHandleMapUpdate();
    Manager.GetFragmentDataChecked<FMassActorFragment>(Entity).SetNoHandleMapUpdate(Entity, Replacement, true);
    Target.SetLocation(FVector(850.0f, 20.0f, 90.0f));
    Run(UBBBMonsterPresentationProcessor::StaticClass());
    TestTrue(TEXT("演员替换直接对齐而非继承旧显示位置"), Replacement->GetActorLocation().Equals(Target.GetLocation(), 0.001));
    Manager.GetFragmentDataChecked<FMassActorFragment>(Entity).ResetNoHandleMapUpdate();
    Run(UBBBMonsterPresentationProcessor::StaticClass());
    TestFalse(TEXT("失去表现演员后清空初始化身份"), Manager.GetFragmentDataChecked<FBBBMonsterPresentationSmoothingFragment>(Entity).LastActor.IsValid());
    Manager.GetFragmentDataChecked<FMassActorFragment>(Entity).SetNoHandleMapUpdate(Entity, Replacement, true);
    Target.SetLocation(FVector(900.0f, 20.0f, 90.0f));
    Run(UBBBMonsterPresentationProcessor::StaticClass());
    TestTrue(TEXT("同一演员重新出现也直接对齐"), Replacement->GetActorLocation().Equals(Target.GetLocation(), 0.001));

    for (const ENetMode Mode : {NM_ListenServer, NM_Standalone})
    {
        World->SetPlayInEditorInitialNetMode(Mode);
        Target.SetLocation(Target.GetLocation() + FVector(10.0f, 0.0f, 0.0f));
        Run(UBBBMonsterPresentationProcessor::StaticClass());
        TestTrue(TEXT("房主与单机不使用客机平滑"), Replacement->GetActorLocation().Equals(Target.GetLocation(), 0.001));
    }

    Manager.DestroyEntity(Entity);
    return true;
}

/** 正式曳光资产验证首帧截断 结束定格 槽位复用与粒子回收 */
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBBBProjectilePresentationTest, "UBBB.Mass.ProjectilePresentation",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FBBBProjectilePresentationTest::RunTest(const FString& Parameters)
{
    if (!FApp::CanEverRender())
    {
        AddWarning(TEXT("曳光粒子回归需要启用渲染的宿主"));
        return true;
    }
    const auto Initialization = UWorld::InitializationValues()
        .AllowAudioPlayback(false).CreatePhysicsScene(true).CreateNavigation(false)
        .CreateAISystem(false).ShouldSimulatePhysics(false);
    UWorld* World = UWorld::CreateWorld(EWorldType::Game, false, NAME_None, nullptr, true, ERHIFeatureLevel::Num, &Initialization);
    if (!TestNotNull(TEXT("曳光验证世界"), World))
    {
        return false;
    }
    GEngine->CreateNewWorldContext(EWorldType::Game).SetCurrentWorld(World);
    ON_SCOPE_EXIT
    {
        GEngine->DestroyWorldContext(World);
        World->DestroyWorld(false);
    };
    auto* Channel = LoadObject<UNiagaraDataChannelAsset>(nullptr,
        TEXT("/Game/_Project/System/Mass/Projectile/NDC_BBBProjectile.NDC_BBBProjectile"));
    auto* System = LoadObject<UNiagaraSystem>(nullptr,
        TEXT("/Game/_Project/System/Mass/Projectile/NS_BBBProjectile.NS_BBBProjectile"));
    if (!TestNotNull(TEXT("正式曳光通道"), Channel) || !TestNotNull(TEXT("正式曳光系统"), System))
    {
        return false;
    }
    FTransformFragment Transform;
    Transform.GetMutableTransform().SetLocation(FVector(10.0f, 0.0f, 0.0f));
    FBBBProjectileMotionFragment Motion;
    Motion.bInitialized = true;
    FBBBProjectilePresentationFragment Visual;
    Visual.Channel = Channel;
    Visual.System = System;
    Visual.LengthCm = 1000.0f;
    Visual.WidthCm = 2.5f;
    Visual.Slot = 0;
    Visual.bSpawnPending = true;
    Visual.bVisualAlive = false;
    FBBBProjectilePresentation::Publish(*World, {&Transform, 1}, {&Motion, 1}, {&Visual, 1});
    auto* Handler = UNiagaraDataChannelLibrary::FindDataChannelHandler(World, Channel->Get());
    FNDCAccessContextInst AccessContext(Channel->Get()->GetAccessContextType());
    auto Data = Handler->FindData(AccessContext, ENiagaraResourceAccess::ReadOnly);
    {
        auto Buffer = Data->GetCPUData(false);
        const auto Sizes = FNiagaraDataSetAccessor<FVector2f>::CreateReader(Buffer.GetReference(), TEXT("SpriteSize"));
        const auto Endings = FNiagaraDataSetAccessor<FNiagaraBool>::CreateReader(Buffer.GetReference(), TEXT("Ending"));
        if (!TestTrue(TEXT("发布截断尺寸与结束标记"), Sizes.IsValid() && Endings.IsValid()))
        {
            return false;
        }
        TestEqual(TEXT("首帧光段只覆盖实际飞过的十厘米"), Sizes.Get(0).Y, 10.0f);
        TestTrue(TEXT("首帧命中仍然发布末帧光段"), Endings.Get(0).GetValue());
    }
    auto* Component = UNiagaraFunctionLibrary::SpawnSystemAtLocation(World, System, FVector::ZeroVector,
        FRotator::ZeroRotator, FVector::OneVector, false, false, ENCPoolMethod::None, false);
    if (!TestNotNull(TEXT("正式共享光效组件"), Component))
    {
        return false;
    }
    Component->SetForceSolo(true);
    Component->Activate();
    const auto ParticleData = [Component]() -> FNiagaraDataBuffer*
    {
        const auto Controller = Component->GetSystemInstanceController();
        if (!Controller.IsValid() || !Controller->IsValid())
        {
            return nullptr;
        }
        Controller->WaitForConcurrentTickAndFinalize();
        const auto& Emitters = Controller->GetSystemInstance_Unsafe()->GetEmitters();
        return Emitters.IsEmpty() ? nullptr : Emitters[0]->GetParticleData().GetCurrentData();
    };
    Component->AdvanceSimulation(1, 0.016f);
    FNiagaraDataBuffer* Particles = ParticleData();
    if (!TestNotNull(TEXT("曳光模拟数据"), Particles) || !TestEqual(TEXT("出生当帧命中仍生成一段光效"), Particles->GetNumInstances(), 1u))
    {
        return false;
    }
    const auto ReportTracer = [this](FNiagaraDataBuffer* Buffer)
    {
        const auto Position = FNiagaraDataSetAccessor<FNiagaraPosition>::CreateReader(Buffer, TEXT("Position"));
        const auto Ending = FNiagaraDataSetAccessor<FNiagaraBool>::CreateReader(Buffer, TEXT("TracerEnding"));
        const auto FadeAge = FNiagaraDataSetAccessor<float>::CreateReader(Buffer, TEXT("TracerFadeAge"));
        if (Buffer->GetNumInstances() > 0 && Position.IsValid() && Ending.IsValid() && FadeAge.IsValid())
        {
            AddInfo(FString::Printf(TEXT("曳光 Position=%s Ending=%d FadeAge=%f"),
                *Position.Get(0).ToString(), Ending.Get(0).GetValue(), FadeAge.Get(0)));
        }
    };
    ReportTracer(Particles);
    Data->BeginFrame(Handler);
    Transform.GetMutableTransform().SetLocation(FVector(1000.0f, 0.0f, 0.0f));
    Visual.bSpawnPending = false;
    Visual.bVisualAlive = true;
    FBBBProjectilePresentation::Publish(*World, {&Transform, 1}, {&Motion, 1}, {&Visual, 1});
    Component->AdvanceSimulation(1, 0.016f);
    Particles = ParticleData();
    if (!TestNotNull(TEXT("槽位复用后的粒子数据"), Particles) || Particles->GetNumInstances() != 1)
    {
        AddError(TEXT("结束粒子没有按短淡出生命周期保留"));
        return false;
    }
    const auto Positions = FNiagaraDataSetAccessor<FNiagaraPosition>::CreateReader(Particles, TEXT("Position"));
    ReportTracer(Particles);
    TestTrue(TEXT("旧光段不跟随复用槽位的新位置"), Positions.IsValid() && Positions.Get(0).Equals(FVector3f(5.0f, 0.0f, 0.0f), 0.001f));
    Component->AdvanceSimulation(5, 0.016f);
    Particles = ParticleData();
    ReportTracer(Particles);
    const auto& TracerEmitters = Component->GetSystemInstanceController()->GetSystemInstance_Unsafe()->GetEmitters();
    TestEqual(TEXT("短淡出结束后粒子归零"), TracerEmitters[0]->GetNumParticles(), 0);
    Component->DestroyComponent();

    auto* ImpactChannel = LoadObject<UNiagaraDataChannelAsset>(nullptr,
        TEXT("/Game/_Project/System/Mass/Projectile/NDC_BBBProjectileImpact.NDC_BBBProjectileImpact"));
    auto* ImpactSystem = LoadObject<UNiagaraSystem>(nullptr,
        TEXT("/Game/_Project/System/Mass/Projectile/NS_BBBProjectileImpact.NS_BBBProjectileImpact"));
    if (!TestNotNull(TEXT("正式命中通道"), ImpactChannel) || !TestNotNull(TEXT("正式命中系统"), ImpactSystem))
    {
        return false;
    }
    const FBBBProjectileImpact Impacts[] = {
        {FVector::ZeroVector, FVector::UpVector, SurfaceType_Default},
        {FVector(50.0f, 0.0f, 0.0f), FVector::UpVector, SurfaceType1},
        {FVector(100.0f, 0.0f, 0.0f), FVector::UpVector, SurfaceType2}
    };
    FBBBProjectilePresentation::PublishImpacts(*World, *ImpactChannel, Impacts);
    auto* ImpactHandler = UNiagaraDataChannelLibrary::FindDataChannelHandler(World, ImpactChannel->Get());
    FNDCAccessContextInst ImpactContext(ImpactChannel->Get()->GetAccessContextType());
    ImpactContext.GetChecked<FNDCAccessContextLegacy>() = FNDCAccessContextLegacy(FVector::ZeroVector);
    auto ImpactData = ImpactHandler->FindData(ImpactContext, ENiagaraResourceAccess::ReadOnly);
    ImpactData->ConsumePublishRequests(ImpactHandler, TG_LastDemotable);
    ImpactData->BeginFrame(ImpactHandler);
    UNiagaraComponent* ImpactComponent = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
        World, ImpactSystem, FVector::ZeroVector, FRotator::ZeroRotator, FVector::OneVector,
        false, false, ENCPoolMethod::None, false);
    if (!TestNotNull(TEXT("命中验证组件"), ImpactComponent))
    {
        return false;
    }
    ImpactComponent->SetForceSolo(true);
    ImpactComponent->Activate();
    ImpactComponent->AdvanceSimulation(1, 0.016f);
    auto ImpactController = ImpactComponent->GetSystemInstanceController();
    ImpactController->WaitForConcurrentTickAndFinalize();
    const auto& ImpactEmitters = ImpactController->GetSystemInstance_Unsafe()->GetEmitters();
    TestEqual(TEXT("三类反馈发射器"), ImpactEmitters.Num(), 3);
    const int32 ExpectedCounts[] = {10, 8, 10};
    for (int32 Index = 0; Index < ImpactEmitters.Num(); ++Index)
    {
        TestEqual(FString::Printf(TEXT("表面分类%d仅生成对应粒子"), Index),
            ImpactEmitters[Index]->GetNumParticles(), ExpectedCounts[Index]);
    }
    ImpactData->BeginFrame(ImpactHandler);
    ImpactComponent->AdvanceSimulation(20, 0.016f);
    ImpactController->WaitForConcurrentTickAndFinalize();
    for (const auto& ImpactEmitter : ImpactEmitters)
    {
        TestEqual(TEXT("反馈生命周期结束后归零"), ImpactEmitter->GetNumParticles(), 0);
    }
    ImpactComponent->DestroyComponent();
    return true;
}

namespace
{
    FAutoConsoleCommand CheckMassPIE(
        TEXT("bbb.mass.CheckPIE"), TEXT("检查所有 PIE 世界的小怪数量与跨机身份"),
        FConsoleCommandWithArgsDelegate::CreateLambda([](const TArray<FString>& Args)
        {
            const int32 Expected = Args.IsEmpty() ? 100 : FCString::Atoi(*Args[0]);
            int32 Worlds = 0;
            bool bValid = true;
            TArray<TSet<FGuid>> Identities;
            for (const FWorldContext& Entry : GEngine->GetWorldContexts())
            {
                UWorld* World = Entry.World();
                if (World == nullptr || World->WorldType != EWorldType::PIE)
                {
                    continue;
                }

                ++Worlds;
                FMassEntityManager& Manager = World->GetSubsystem<UMassEntitySubsystem>()->GetMutableEntityManager();
                FMassEntityQuery Query(Manager.AsShared());
                Query.AddRequirement<FBBBMonsterNetworkFragment>(EMassFragmentAccess::ReadOnly);
                Query.AddRequirement<FBBBMonsterHealthFragment>(EMassFragmentAccess::ReadOnly);
                FMassExecutionContext Context(Manager, 0.0f);
                TSet<FGuid>& Ids = Identities.AddDefaulted_GetRef();
                int32 Count = 0;
                Query.ForEachEntityChunk(Context, [&](FMassExecutionContext& Chunk)
                {
                    const auto Network = Chunk.GetFragmentView<FBBBMonsterNetworkFragment>();
                    const auto Health = Chunk.GetFragmentView<FBBBMonsterHealthFragment>();
                    for (int32 Index = 0; Index < Chunk.GetNumEntities(); ++Index)
                    {
                        ++Count;
                        bValid &= Health[Index].CurrentHealth >= 0.0f && Network[Index].InstanceId.IsValid();
                        Ids.Add(Network[Index].InstanceId);
                    }
                });
                UE_LOG(LogTemp, Display, TEXT("[BBBMassCheck] World=%s NetMode=%d Monsters=%d"), *World->GetPathName(), int32(World->GetNetMode()), Count);
                bValid &= Count == Expected;
            }

            bValid &= Worlds > 0;
            for (int32 Index = 1; Index < Identities.Num(); ++Index)
            {
                bValid &= Identities[0].Intersect(Identities[Index]).Num() == Identities[0].Num();
            }
            UE_LOG(LogTemp, Display, TEXT("[BBBMassCheck] Result=%s Worlds=%d Expected=%d"), bValid ? TEXT("PASS") : TEXT("FAIL"), Worlds, Expected);
        }));

    FAutoConsoleCommand KillMassClientMonster(
        TEXT("bbb.mass.KillClientMonster"), TEXT("仅在 PIE 客机投递一次致死输入 检查房主接受结果"),
        FConsoleCommandDelegate::CreateLambda([]()
        {
            for (const FWorldContext& Entry : GEngine->GetWorldContexts())
            {
                UWorld* World = Entry.World();
                if (World == nullptr || World->WorldType != EWorldType::PIE || World->GetNetMode() != NM_Client)
                {
                    continue;
                }

                FMassEntityManager& Manager = World->GetSubsystem<UMassEntitySubsystem>()->GetMutableEntityManager();
                FMassEntityQuery Query(Manager.AsShared());
                Query.AddRequirement<FBBBMonsterHealthFragment>(EMassFragmentAccess::ReadOnly);
                FMassExecutionContext Context(Manager, 0.0f);
                FMassEntityHandle Target;
                Query.ForEachEntityChunk(Context, [&](FMassExecutionContext& Chunk)
                {
                    if (!Target.IsSet() && Chunk.GetNumEntities() > 0)
                    {
                        Target = Chunk.GetEntity(0);
                    }
                });
                if (Target.IsSet())
                {
                    FBBBMonsterDamageLocalControlPacket Packet;
                    UBBBMassSubsystem* Mass = World->GetSubsystem<UBBBMassSubsystem>();
                    const APlayerController* Controller = World->GetFirstPlayerController();
                    const APlayerState* Player = Controller != nullptr ? Controller->GetPlayerState<APlayerState>() : nullptr;
                    if (Player == nullptr || !Mass->QueryDamage(Target, Packet.Contributions))
                    {
                        return;
                    }
                    Packet.Include({Player->GetPlayerId(), 1000000.0});
                    Mass->SubmitInput(Target, MoveTemp(Packet));
                    UE_LOG(LogTemp, Display, TEXT("[BBBMassCheck] Client lethal input submitted"));
                    return;
                }
            }
            UE_LOG(LogTemp, Error, TEXT("[BBBMassCheck] No client monster to test"));
        }));

    FAutoConsoleCommand CheckMassProjectiles(
        TEXT("bbb.mass.CheckProjectiles"), TEXT("检查 PIE 正在飞行的子弹与造伤权限"),
        FConsoleCommandDelegate::CreateLambda([]()
        {
            for (const FWorldContext& Entry : GEngine->GetWorldContexts())
            {
                UWorld* World = Entry.World();
                if (World == nullptr || World->WorldType != EWorldType::PIE)
                {
                    continue;
                }

                FMassEntityManager& Manager = World->GetSubsystem<UMassEntitySubsystem>()->GetMutableEntityManager();
                FMassEntityQuery Query(Manager.AsShared());
                Query.AddRequirement<FBBBProjectileMotionFragment>(EMassFragmentAccess::ReadOnly);
                Query.AddRequirement<FBBBProjectileCollisionFragment>(EMassFragmentAccess::ReadOnly);
                Query.AddRequirement<FBBBProjectilePresentationFragment>(EMassFragmentAccess::ReadOnly);
                FMassExecutionContext Context(Manager, 0.0f);
                int32 Count = 0;
                int32 DamageCount = 0;
                int32 ChannelCount = 0;
                Query.ForEachEntityChunk(Context, [&](FMassExecutionContext& Chunk)
                {
                    const auto Collision = Chunk.GetFragmentView<FBBBProjectileCollisionFragment>();
                    const auto Presentation = Chunk.GetFragmentView<FBBBProjectilePresentationFragment>();
                    for (int32 Index = 0; Index < Chunk.GetNumEntities(); ++Index)
                    {
                        ++Count;
                        DamageCount += Collision[Index].bCanCauseDamage ? 1 : 0;
                        ChannelCount += Presentation[Index].Channel.IsValid() ? 1 : 0;
                    }
                });
                UE_LOG(LogTemp, Display, TEXT("[BBBMassCheck] World=%s Projectiles=%d DamageAllowed=%d VFXChannel=%d"), *World->GetPathName(), Count, DamageCount, ChannelCount);
            }
        }));
}

#endif
