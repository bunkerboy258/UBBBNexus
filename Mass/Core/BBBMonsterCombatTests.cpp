#include "Misc/AutomationTest.h"

#if WITH_DEV_AUTOMATION_TESTS && WITH_EDITOR
#include "Misc/ScopeExit.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "UObject/Package.h"
#include "Engine/LocalPlayer.h"
#include "GameFramework/PlayerController.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "MassCommonFragments.h"
#include "MassEntitySubsystem.h"
#include "MassExecutor.h"
#include "MassProcessingContext.h"
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Processors/Combat/BBBMonsterCombatProcessor.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Processors/Perception/BBBMonsterPerceptionProcessor.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Processors/Behavior/BBBMonsterBehaviorProcessor.h"
#include "MassMovementFragments.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Behavior/BBBMonsterBehaviorFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Combat/BBBMonsterCombatFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Movement/BBBMonsterGroundFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Movement/BBBMonsterMobilityFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Movement/BBBMonsterNavigationFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Perception/BBBMonsterPerceptionFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Perception/BBBMonsterStimulusFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Perception/BBBMonsterTargetFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Health/BBBMonsterHealthFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Health/BBBMonsterDamageFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Health/BBBMonsterDeathFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Network/BBBMonsterNetworkFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Tags/BBBMonsterTag.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Config/BBBMonsterDefinition.h"

/** 验证真实角色接收僵尸近战的几何边界和生命转换 */
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBBBMonsterCombatTest, "UBBB.Mass.ZombieCombat",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FBBBMonsterCombatTest::RunTest(const FString& Parameters)
{
    const auto Initialization = UWorld::InitializationValues()
        .AllowAudioPlayback(false).CreatePhysicsScene(true).CreateNavigation(false).CreateAISystem(false);
    UWorld* World = UWorld::CreateWorld(EWorldType::Game, false, NAME_None, nullptr, true,
        ERHIFeatureLevel::Num, &Initialization);
    if (!TestNotNull(TEXT("战斗隔离世界"), World))
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

    UClass* CharacterClass = LoadClass<ABBBCharacter>(nullptr,
        TEXT("/Game/_Project/Characters/BBBC_UA/BBBC_UA_0.BBBC_UA_0_C"));
    if (!TestNotNull(TEXT("正式玩家蓝图"), CharacterClass))
    {
        return false;
    }
    AActor* Floor = World->SpawnActor<AActor>();
    UBoxComponent* FloorBody = NewObject<UBoxComponent>(Floor);
    Floor->SetRootComponent(FloorBody);
    FloorBody->SetBoxExtent(FVector(1000.0f, 1000.0f, 10.0f));
    FloorBody->SetCollisionProfileName(TEXT("BlockAll"));
    FloorBody->RegisterComponent();
    Floor->SetActorLocation(FVector(0.0f, 0.0f, -10.0f));
    APlayerController* Controller = World->SpawnActor<APlayerController>();
    ABBBCharacter* Player = World->SpawnActor<ABBBCharacter>(CharacterClass,
        FVector(150.0f, 0.0f, 90.0f), FRotator::ZeroRotator);
    if (!TestNotNull(TEXT("真实玩家"), Player) || !TestNotNull(TEXT("本机控制者"), Controller))
    {
        return false;
    }
    ULocalPlayer* LocalPlayer = NewObject<ULocalPlayer>(GEngine);
    LocalPlayer->PlayerController = Controller;
    Controller->Player = LocalPlayer;
    Controller->Possess(Player);
    World->InitializeActorsForPlay(FURL());
    World->SetBegunPlay(true);
    Controller->DispatchBeginPlay();
    Player->DispatchBeginPlay();
    Player->Tick(1.0f / 60.0f);
    TestEqual(TEXT("正式玩家初始生命"), Player->GetHealth(), 500.0f);

    FMassEntityManager& Manager = World->GetSubsystem<UMassEntitySubsystem>()->GetMutableEntityManager();
    const FMassArchetypeHandle Type = Manager.CreateArchetype({FTransformFragment::StaticStruct(), FMassVelocityFragment::StaticStruct(),
        FBBBMonsterBehaviorFragment::StaticStruct(), FBBBMonsterCombatFragment::StaticStruct(),
        FBBBMonsterGroundFragment::StaticStruct(), FBBBMonsterMobilityFragment::StaticStruct(),
        FBBBMonsterNavigationFragment::StaticStruct(), FBBBMonsterPerceptionFragment::StaticStruct(),
        FBBBMonsterTargetFragment::StaticStruct(), FBBBMonsterHealthFragment::StaticStruct(),
        FBBBMonsterDamageFragment::StaticStruct(), FBBBMonsterDeathFragment::StaticStruct(),
        FBBBMonsterNetworkFragment::StaticStruct(), FBBBMonsterStimulusFragment::StaticStruct(), FBBBMonsterTag::StaticStruct()});
    const FMassEntityHandle Entity = Manager.CreateEntity(Type);
    auto* PerceptionSettings = NewObject<UBBBMonsterDefinition>(World);
    PerceptionSettings->SightConfirmMin = 0.01f;
    PerceptionSettings->SightConfirmMax = 0.01f;
    Manager.GetFragmentDataChecked<FBBBMonsterNetworkFragment>(Entity).Definition = PerceptionSettings;
    FTransform& Transform = Manager.GetFragmentDataChecked<FTransformFragment>(Entity).GetMutableTransform();
    Transform.SetLocation(FVector(0.0f, 0.0f, 90.0f));
    auto& State = Manager.GetFragmentDataChecked<FBBBMonsterBehaviorFragment>(Entity);
    auto& Combat = Manager.GetFragmentDataChecked<FBBBMonsterCombatFragment>(Entity);
    auto& Ground = Manager.GetFragmentDataChecked<FBBBMonsterGroundFragment>(Entity);
    auto& Target = Manager.GetFragmentDataChecked<FBBBMonsterTargetFragment>(Entity);
    auto& Health = Manager.GetFragmentDataChecked<FBBBMonsterHealthFragment>(Entity);
    Ground.bGrounded = true;
    Health.CurrentHealth = 100.0f;
    UBBBMonsterCombatProcessor* Attack = NewObject<UBBBMonsterCombatProcessor>(World);
    UBBBMonsterPerceptionProcessor* Perception = NewObject<UBBBMonsterPerceptionProcessor>(World);
    UBBBMonsterBehaviorProcessor* Behavior = NewObject<UBBBMonsterBehaviorProcessor>(World);
    for (UMassProcessor* Processor : TArray<UMassProcessor*>{Attack, Perception, Behavior})
    {
        Processor->CallInitialize(World, Manager.AsShared());
    }
    const auto Run = [&Manager](UMassProcessor* Processor)
    {
        UE::Mass::FProcessingContext Context(Manager, 1.0f / 60.0f);
        UE::Mass::Executor::Run(*Processor, Context);
    };
    const auto BeginHit = [&]()
    {
        State.State = EBBBMonsterBehavior::Attack;
        State.StateEnteredTime = World->GetTimeSeconds() - Combat.AttackWindup;
        Combat.AttackTarget = Player;
        Combat.bHitAttempted = false;
        Combat.bAttackFinished = false;
        ++Combat.AttackId;
    };
    const auto Hit = [&]()
    {
        Run(Attack);
        Player->Tick(1.0f / 60.0f);
    };

    BeginHit();
    Hit();
    TestEqual(TEXT("正面有效攻击扣一次伤害"), Player->GetHealth(), 490.0f);
    TestTrue(TEXT("近战提供有效冲击方向"),
        Player->RuntimeData.Life.ReadHitState().Direction.X > 0.9f);
    Hit();
    TestEqual(TEXT("同一攻击不能重复扣血"), Player->GetHealth(), 490.0f);
    BeginHit();
    State.StateEnteredTime = World->GetTimeSeconds();
    Hit();
    TestEqual(TEXT("前摇结束前不扣血"), Player->GetHealth(), 490.0f);

    Player->SetActorLocation(FVector(500.0f, 0.0f, 90.0f));
    BeginHit();
    Hit();
    TestEqual(TEXT("前摇期间离开范围挥空"), Player->GetHealth(), 490.0f);
    Player->SetActorLocation(FVector(150.0f, 0.0f, 90.0f));
    Player->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    BeginHit();
    Hit();
    TestEqual(TEXT("目标未提供碰撞最近点时仍使用有效目标位置"), Player->GetHealth(), 480.0f);
    Player->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    Player->SetActorLocation(FVector(-150.0f, 0.0f, 90.0f));
    float BeforeMiss = Player->GetHealth();
    BeginHit();
    Hit();
    TestEqual(TEXT("绕到背后不被距离判定击中"), Player->GetHealth(), BeforeMiss);
    Player->SetActorLocation(FVector(0.0f, 150.0f, 90.0f));
    BeforeMiss = Player->GetHealth();
    BeginHit();
    Hit();
    TestEqual(TEXT("侧方超过挥击角度不扣血"), Player->GetHealth(), BeforeMiss);
    Player->SetActorLocation(FVector(150.0f, 0.0f, 90.0f));

    AActor* Wall = World->SpawnActor<AActor>();
    UBoxComponent* WallBody = NewObject<UBoxComponent>(Wall);
    Wall->SetRootComponent(WallBody);
    WallBody->SetBoxExtent(FVector(10.0f, 100.0f, 150.0f));
    WallBody->SetCollisionProfileName(TEXT("BlockAll"));
    WallBody->RegisterComponent();
    Wall->SetActorLocation(FVector(75.0f, 0.0f, 150.0f));
    BeforeMiss = Player->GetHealth();
    BeginHit();
    Hit();
    TestEqual(TEXT("隔墙近战不扣血"), Player->GetHealth(), BeforeMiss);
    Wall->Destroy();
    Ground.bGrounded = false;
    BeforeMiss = Player->GetHealth();
    BeginHit();
    Hit();
    TestEqual(TEXT("失去支撑取消伤害"), Player->GetHealth(), BeforeMiss);
    TestTrue(TEXT("空中攻击机会已消耗"), Combat.bHitAttempted);
    Ground.bGrounded = true;

    State.State = EBBBMonsterBehavior::Chase;
    State.StateEndsAtTime = 0.0f;
    State.bHadTarget = true;
    Target.bHasTarget = true;
    Target.bTargetVisible = true;
    Target.TargetActor = Player;
    Target.TargetLocation = Player->GetActorLocation();
    Transform.SetRotation(FRotator(0.0f, 90.0f, 0.0f).Quaternion());
    Combat.NextAttackTime = World->GetTimeSeconds();
    Run(Behavior);
    TestEqual(TEXT("未面向目标先继续追击"), State.State, EBBBMonsterBehavior::Chase);
    TestTrue(TEXT("攻击裁决不得瞬间转身"), FMath::IsNearlyEqual(Transform.Rotator().Yaw, 90.0f));
    Transform.SetRotation(FQuat::Identity);
    Run(Behavior);
    TestEqual(TEXT("满足条件启动攻击"), State.State, EBBBMonsterBehavior::Attack);
    TestTrue(TEXT("已面向目标后才能开始攻击"), Transform.GetRotation().GetForwardVector().X > 0.99f);

    Combat.AttackDamage = 500.0f;
    BeginHit();
    Hit();
    TestEqual(TEXT("僵尸攻击确实导致玩家倒地"), Player->GetLifePhase(), EBBBCharacterLifePhase::Downed);
    TestEqual(TEXT("致倒地伤害不溢出"), Player->GetHealth(), 300.0f);
    TestTrue(TEXT("倒地玩家仍可被攻击"), Player->CanBeDamaged());
    Run(Perception);
    TestTrue(TEXT("倒地玩家保留索敌资格"), Target.bHasTarget && Target.TargetActor == Player);
    Combat.AttackDamage = 300.0f;
    BeginHit();
    Hit();
    TestEqual(TEXT("僵尸攻击倒地玩家导致死亡"), Player->GetLifePhase(), EBBBCharacterLifePhase::Dead);
    TestFalse(TEXT("死亡玩家撤销受伤资格"), Player->CanBeDamaged());
    Run(Perception);
    TestFalse(TEXT("死亡玩家退出索敌"), Target.bHasTarget);
    BeginHit();
    Run(Behavior);
    TestTrue(TEXT("目标死亡立即取消当前攻击"), Combat.bAttackFinished && !Combat.AttackTarget.IsValid());
    Health.CurrentHealth = 0.0f;
    BeginHit();
    Run(Behavior);
    TestEqual(TEXT("僵尸前摇中死亡立即打断"), State.State, EBBBMonsterBehavior::Dead);
    TestTrue(TEXT("死亡僵尸无剩余命中机会"), Combat.bHitAttempted && !Combat.AttackTarget.IsValid());
    Manager.DestroyEntity(Entity);
    return true;
}
#endif
