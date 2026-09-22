#if WITH_DEV_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"
#include "Misc/ScopeExit.h"
#include "Animation/AnimSingleNodeInstance.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "MassActorSubsystem.h"
#include "MassCommonFragments.h"
#include "MassEntitySubsystem.h"
#include "MassExecutor.h"
#include "MassMovementFragments.h"
#include "MassProcessingContext.h"
#include "NavigationSystem.h"
#include "BBBWork/UBBBNexus/MonsterMass/Entity/MonsterRuntimeData.h"
#include "BBBWork/UBBBNexus/MonsterMass/Input/MonsterDamageInput.h"
#include "BBBWork/UBBBNexus/MonsterMass/Presentation/MonsterBasicActor.h"
#include "BBBWork/UBBBNexus/MonsterMass/Presentation/MonsterPresentationComponent.h"
#include "BBBWork/UBBBNexus/MonsterMass/Processors/MonsterAvoidanceProcessor.h"
#include "BBBWork/UBBBNexus/MonsterMass/Processors/MonsterCombatProcessor.h"
#include "BBBWork/UBBBNexus/MonsterMass/Processors/MonsterDamageProcessor.h"
#include "BBBWork/UBBBNexus/MonsterMass/Processors/MonsterLifecycleProcessor.h"
#include "BBBWork/UBBBNexus/MonsterMass/Processors/MonsterNavigationProcessor.h"
#include "BBBWork/UBBBNexus/MonsterMass/Processors/MonsterPerceptionProcessor.h"
#include "BBBWork/UBBBNexus/MonsterMass/Processors/MonsterPresentationStateProcessor.h"
#include "BBBWork/UBBBNexus/MonsterMass/Processors/MonsterPresentationProcessor.h"
#include "BBBWork/UBBBNexus/MonsterMass/Processors/MonsterStateProcessor.h"

/** 在隔离世界中执行真实处理器 不依赖可见窗口或动画通知 */
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FMonsterCombatRuntimeTest, "UBBB.MonsterMass.CombatRuntime", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FMonsterCombatRuntimeTest::RunTest(const FString& Parameters)
{
    const UWorld::InitializationValues Initialization = UWorld::InitializationValues()
        .AllowAudioPlayback(false)
        .CreatePhysicsScene(true)
        .CreateNavigation(false)
        .CreateAISystem(false)
        .ShouldSimulatePhysics(false);
    UWorld* World = UWorld::CreateWorld(EWorldType::Game, false, NAME_None, nullptr, true, ERHIFeatureLevel::Num, &Initialization);

    if (!TestNotNull(TEXT("Isolated world"), World))
    {
        return false;
    }

    GEngine->CreateNewWorldContext(EWorldType::Game).SetCurrentWorld(World);
    World->SetNavigationSystem(NewObject<UNavigationSystemV1>(World));

    ON_SCOPE_EXIT
    {
        GEngine->DestroyWorldContext(World);
        World->DestroyWorld(false);
    };

    UMassEntitySubsystem* EntitySubsystem = World->GetSubsystem<UMassEntitySubsystem>();

    if (!TestNotNull(TEXT("Mass subsystem"), EntitySubsystem))
    {
        return false;
    }

    FMassEntityManager& Manager = EntitySubsystem->GetMutableEntityManager();
    const FMassArchetypeHandle Archetype = Manager.CreateArchetype({
        FMonsterTag::StaticStruct(), FTransformFragment::StaticStruct(), FMassVelocityFragment::StaticStruct(),
        FMonsterHealthFragment::StaticStruct(), FMonsterStateFragment::StaticStruct(), FMonsterMovementFragment::StaticStruct(),
        FMonsterPerceptionFragment::StaticStruct(), FMonsterTargetRequestFragment::StaticStruct(), FMonsterCombatFragment::StaticStruct(),
        FMonsterAvoidanceFragment::StaticStruct(), FMonsterDamageEventFragment::StaticStruct(), FMonsterDeathEventFragment::StaticStruct(),
        FMonsterPresentationStateFragment::StaticStruct(), FMassActorFragment::StaticStruct()
    });
    const FMassEntityHandle Attacker = Manager.CreateEntity(Archetype);
    const FMassEntityHandle Victim = Manager.CreateEntity(Archetype);
    UClass* MonsterClass = LoadClass<AMonsterBasicActor>(nullptr, TEXT("/Game/MonsterMass/BP_MonsterBasic.BP_MonsterBasic_C"));

    if (!TestNotNull(TEXT("Configured monster blueprint"), MonsterClass))
    {
        return false;
    }

    FActorSpawnParameters SpawnParameters;
    SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
    AMonsterBasicActor* AttackerActor = World->SpawnActor<AMonsterBasicActor>(MonsterClass, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParameters);
    AMonsterBasicActor* VictimActor = World->SpawnActor<AMonsterBasicActor>(MonsterClass, FVector(100.0, 0.0, 0.0), FRotator::ZeroRotator, SpawnParameters);

    if (!TestNotNull(TEXT("Attacker actor"), AttackerActor) || !TestNotNull(TEXT("Victim actor"), VictimActor))
    {
        return false;
    }

    Manager.GetFragmentDataChecked<FMassActorFragment>(Attacker).SetAndUpdateHandleMap(Attacker, AttackerActor, false);
    Manager.GetFragmentDataChecked<FMassActorFragment>(Victim).SetAndUpdateHandleMap(Victim, VictimActor, false);
    Manager.GetFragmentDataChecked<FTransformFragment>(Victim).GetMutableTransform().SetLocation(VictimActor->GetActorLocation());

    // 自动化自行推进世界时间 只执行待测处理器 不运行项目角色或武器逻辑
    const auto Run = [&Manager, World](UClass* ProcessorClass)
    {
        UMassProcessor* Processor = NewObject<UMassProcessor>(World, ProcessorClass);
        Processor->CallInitialize(World, Manager.AsShared());
        UE::Mass::FProcessingContext Context(Manager, 0.05f);
        UE::Mass::Executor::Run(*Processor, Context);
    };
    const auto SelectVictim = [&Manager, Attacker, VictimActor]()
    {
        FMonsterTargetRequestFragment& Target = Manager.GetFragmentDataChecked<FMonsterTargetRequestFragment>(Attacker);
        Target.bHasTarget = true;
        Target.TargetActor = VictimActor;
        Target.TargetLocation = VictimActor->GetActorLocation();
    };

    SelectVictim();
    FMonsterDamageRequest Request;
    Request.Damage = 10.0f;
    Request.DamageCauser = VictimActor;
    Request.HitLocation = FVector(1.0, 2.0, 3.0);
    Request.HitDirection = FVector::ForwardVector;
    TestTrue(TEXT("First queued damage accepted"), UMonsterDamageInput::SubmitDamage(AttackerActor, Request));
    Request.Damage = 15.0f;
    TestTrue(TEXT("Second queued damage accepted"), UMonsterDamageInput::SubmitDamageToEntity(World, Attacker, Request));
    TestEqual(TEXT("Two requests retained"), Manager.GetFragmentDataChecked<FMonsterDamageEventFragment>(Attacker).PendingRequests.Num(), 2);
    TestEqual(TEXT("Input does not immediately deduct health"), Manager.GetFragmentDataChecked<FMonsterHealthFragment>(Attacker).CurrentHealth, 100.0f);

    // 迁移包含动态队列的实体 验证队列随实体移动且不会共享其它实体的存储
    Manager.RemoveFragmentFromEntity(Attacker, FMonsterPerceptionFragment::StaticStruct());
    Manager.AddFragmentToEntity(Attacker, FMonsterPerceptionFragment::StaticStruct());
    TestEqual(TEXT("Queue survives archetype migration"), Manager.GetFragmentDataChecked<FMonsterDamageEventFragment>(Attacker).PendingRequests.Num(), 2);
    Run(UMonsterDamageProcessor::StaticClass());
    Run(UMonsterStateProcessor::StaticClass());
    TestEqual(TEXT("Both hits deducted"), Manager.GetFragmentDataChecked<FMonsterHealthFragment>(Attacker).CurrentHealth, 75.0f);
    TestTrue(TEXT("Hurt has priority"), Manager.GetFragmentDataChecked<FMonsterStateFragment>(Attacker).State == EMonsterState::Hurt);
    TestEqual(TEXT("Source metadata retained"), Manager.GetFragmentDataChecked<FMonsterDamageEventFragment>(Attacker).LastDamageRequest.HitLocation, Request.HitLocation);
    Run(UMonsterPerceptionProcessor::StaticClass());
    Run(UMonsterStateProcessor::StaticClass());
    TestTrue(TEXT("Missing player does not overwrite hurt"), Manager.GetFragmentDataChecked<FMonsterStateFragment>(Attacker).State == EMonsterState::Hurt);

    SelectVictim();
    World->TimeSeconds = 1.0;
    Run(UMonsterStateProcessor::StaticClass());
    const uint32 FirstAttackId = Manager.GetFragmentDataChecked<FMonsterCombatFragment>(Attacker).AttackId;
    TestTrue(TEXT("Recovery can start an attack"), Manager.GetFragmentDataChecked<FMonsterStateFragment>(Attacker).State == EMonsterState::Attack);
    Run(UMonsterCombatProcessor::StaticClass());
    TestTrue(TEXT("No damage during windup"), Manager.GetFragmentDataChecked<FMonsterDamageEventFragment>(Victim).PendingRequests.IsEmpty());
    World->TimeSeconds = 1.36;
    Run(UMonsterCombatProcessor::StaticClass());
    Run(UMonsterCombatProcessor::StaticClass());
    TestEqual(TEXT("Only one damage request for repeated execution"), Manager.GetFragmentDataChecked<FMonsterDamageEventFragment>(Victim).PendingRequests.Num(), 1);
    Run(UMonsterPresentationStateProcessor::StaticClass());
    Run(UMonsterPresentationProcessor::StaticClass());
    UAnimSingleNodeInstance* Animation = AttackerActor->GetMonsterMesh()->GetSingleNodeInstance();
    TestNotNull(TEXT("Attack animation instance exists"), Animation);
    TestTrue(TEXT("Attack animation follows logical progress"), AttackerActor->GetMonsterMesh()->GetPosition() > 0.0f);

    World->TimeSeconds = 1.81;
    Run(UMonsterCombatProcessor::StaticClass());
    World->TimeSeconds = 2.3;
    Run(UMonsterStateProcessor::StaticClass());
    TestTrue(TEXT("Repeated attack gets a new identifier"), Manager.GetFragmentDataChecked<FMonsterCombatFragment>(Attacker).AttackId > FirstAttackId);
    Run(UMonsterPresentationStateProcessor::StaticClass());
    Run(UMonsterPresentationProcessor::StaticClass());
    TestTrue(TEXT("Same state new attack rewinds animation"), FMath::IsNearlyZero(AttackerActor->GetMonsterMesh()->GetPosition()));

    // 判定时使用目标实时位置 而不是攻击开始时缓存的位置
    VictimActor->SetActorLocation(FVector(1000.0, 0.0, 0.0));
    World->TimeSeconds = 2.66;
    Run(UMonsterCombatProcessor::StaticClass());
    TestEqual(TEXT("Moving out of range misses"), Manager.GetFragmentDataChecked<FMonsterDamageEventFragment>(Victim).PendingRequests.Num(), 1);
    VictimActor->SetActorLocation(FVector(100.0, 0.0, 0.0));
    Run(UMonsterCombatProcessor::StaticClass());
    TestEqual(TEXT("Returning does not retry a missed attack"), Manager.GetFragmentDataChecked<FMonsterDamageEventFragment>(Victim).PendingRequests.Num(), 1);

    World->TimeSeconds = 3.11;
    Run(UMonsterCombatProcessor::StaticClass());
    World->TimeSeconds = 3.6;
    Run(UMonsterStateProcessor::StaticClass());
    Request.Damage = 1.0f;
    UMonsterDamageInput::SubmitDamage(AttackerActor, Request);
    Run(UMonsterDamageProcessor::StaticClass());
    Run(UMonsterStateProcessor::StaticClass());
    World->TimeSeconds = 3.96;
    Run(UMonsterCombatProcessor::StaticClass());
    TestTrue(TEXT("Damage interrupts windup"), Manager.GetFragmentDataChecked<FMonsterStateFragment>(Attacker).State == EMonsterState::Hurt);
    TestTrue(TEXT("Interrupted attack sends no hit"), Manager.GetFragmentDataChecked<FMonsterDamageEventFragment>(Victim).PendingRequests.IsEmpty());

    // 超长帧仍只判定一次 逻辑伤害不依赖可见演员
    World->TimeSeconds = 5.0;
    Run(UMonsterStateProcessor::StaticClass());
    Manager.GetFragmentDataChecked<FMassActorFragment>(Attacker).ResetAndUpdateHandleMap();
    World->TimeSeconds = 8.0;
    Run(UMonsterCombatProcessor::StaticClass());
    Run(UMonsterCombatProcessor::StaticClass());
    TestEqual(TEXT("Invisible hitch attack still hits exactly once"), Manager.GetFragmentDataChecked<FMonsterDamageEventFragment>(Victim).PendingRequests.Num(), 1);
    Manager.GetFragmentDataChecked<FMassActorFragment>(Attacker).SetAndUpdateHandleMap(Attacker, AttackerActor, false);

    // 一百个实体同帧受击 不允许队列串到相邻实体
    TArray<FMassEntityHandle> Crowd;

    for (int32 Index = 0; Index < 100; ++Index)
    {
        const FMassEntityHandle Entity = Manager.CreateEntity(Archetype);
        Crowd.Add(Entity);
        Request.Damage = 3.0f;
        UMonsterDamageInput::SubmitDamageToEntity(World, Entity, Request);
        Request.Damage = 7.0f;
        UMonsterDamageInput::SubmitDamageToEntity(World, Entity, Request);
    }

    Run(UMonsterDamageProcessor::StaticClass());

    for (const FMassEntityHandle Entity : Crowd)
    {
        TestEqual(TEXT("Crowd independent damage queues"), Manager.GetFragmentDataChecked<FMonsterHealthFragment>(Entity).CurrentHealth, 90.0f);
        Manager.DestroyEntity(Entity);
    }

    Request.Damage = 1000.0f;
    TestTrue(TEXT("Lethal input accepted"), UMonsterDamageInput::SubmitDamage(AttackerActor, Request));
    Run(UMonsterDamageProcessor::StaticClass());
    Run(UMonsterStateProcessor::StaticClass());
    const float DestroyTime = Manager.GetFragmentDataChecked<FMonsterDeathEventFragment>(Attacker).DestroyAtTime;
    TestFalse(TEXT("Dead input rejected"), UMonsterDamageInput::SubmitDamage(AttackerActor, Request));
    TestEqual(TEXT("Standard UE damage also rejects dead target"), UGameplayStatics::ApplyDamage(AttackerActor, 1.0f, nullptr, VictimActor, nullptr), 0.0f);
    Manager.GetFragmentDataChecked<FMassVelocityFragment>(Attacker).Value = FVector(300.0, 0.0, 0.0);
    const FVector DeadLocation = Manager.GetFragmentDataChecked<FTransformFragment>(Attacker).GetTransform().GetLocation();
    Run(UMonsterNavigationProcessor::StaticClass());
    Run(UMonsterAvoidanceProcessor::StaticClass());
    Run(UMonsterCombatProcessor::StaticClass());
    Run(UMonsterPresentationStateProcessor::StaticClass());
    Run(UMonsterPresentationProcessor::StaticClass());
    TestTrue(TEXT("Dead movement stops"), Manager.GetFragmentDataChecked<FMassVelocityFragment>(Attacker).Value.IsZero());
    TestEqual(TEXT("Dead avoidance does not move corpse"), Manager.GetFragmentDataChecked<FTransformFragment>(Attacker).GetTransform().GetLocation(), DeadLocation);
    TestFalse(TEXT("Dead actor collision disabled"), AttackerActor->GetActorEnableCollision());
    float CurrentHealth = -1.0f;
    float MaxHealth = -1.0f;
    bool bDead = false;
    TestTrue(TEXT("Health query succeeds for corpse"), UMonsterDamageInput::TryGetHealth(AttackerActor, CurrentHealth, MaxHealth, bDead));
    TestTrue(TEXT("Health query reports dead"), bDead && CurrentHealth == 0.0f && MaxHealth == 100.0f);
    Run(UMonsterLifecycleProcessor::StaticClass());
    TestTrue(TEXT("Death presentation is retained"), Manager.IsEntityValid(Attacker));
    World->TimeSeconds = DestroyTime + 0.01;
    Run(UMonsterLifecycleProcessor::StaticClass());
    TestFalse(TEXT("Corpse reclaimed on schedule"), Manager.IsEntityValid(Attacker));
    TestFalse(TEXT("Stale entity input rejected"), UMonsterDamageInput::SubmitDamageToEntity(World, Attacker, Request));
    TestFalse(TEXT("Invalid actor input rejected"), UMonsterDamageInput::SubmitDamage(nullptr, Request));
    AddInfo(TEXT("Verified queued damage migration metadata crowd isolation windup hit recovery repeat miss interruption invisible hitch death collision and reclaim"));
    return !HasAnyErrors();
}

#endif
