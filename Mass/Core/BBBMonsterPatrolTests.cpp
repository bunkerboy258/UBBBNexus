#if WITH_DEV_AUTOMATION_TESTS && WITH_EDITOR

#include "Misc/AutomationTest.h"
#include "Misc/ScopeExit.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "MassCommonFragments.h"
#include "MassMovementFragments.h"
#include "MassEntitySubsystem.h"
#include "MassExecutor.h"
#include "MassProcessingContext.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Processors/Behavior/BBBMonsterBehaviorProcessor.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Processors/Combat/BBBMonsterCombatProcessor.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Processors/Movement/BBBMonsterLocomotionProcessor.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Behavior/BBBMonsterBehaviorFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Movement/BBBMonsterMovementFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Movement/BBBMonsterNavigationFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Movement/BBBMonsterAvoidanceFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Combat/BBBMonsterCombatFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Perception/BBBMonsterTargetFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Health/BBBMonsterHealthFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Health/BBBMonsterDamageFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Health/BBBMonsterDeathFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Tags/BBBMonsterTag.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Movement/BBBMonsterGroundFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Movement/BBBMonsterMobilityFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Network/BBBMonsterNetworkFragment.h"

/** 验证随机走停循环 警觉与受击死亡优先级 */
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBBBMonsterPatrolTest, "UBBB.Mass.ZombiePatrol",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FBBBMonsterPatrolTest::RunTest(const FString& Parameters)
{
    const auto Initialization = UWorld::InitializationValues()
        .AllowAudioPlayback(false).CreatePhysicsScene(true).CreateNavigation(false)
        .CreateAISystem(false).ShouldSimulatePhysics(false);
    UWorld* World = UWorld::CreateWorld(EWorldType::Game, false, NAME_None, nullptr, true, ERHIFeatureLevel::Num, &Initialization);
    if (!TestNotNull(TEXT("巡逻隔离世界"), World))
    {
        return false;
    }
    GEngine->CreateNewWorldContext(EWorldType::Game).SetCurrentWorld(World);
    ON_SCOPE_EXIT
    {
        GEngine->DestroyWorldContext(World);
        World->DestroyWorld(false);
    };
    AActor* Floor = World->SpawnActor<AActor>();
    UBoxComponent* FloorBody = NewObject<UBoxComponent>(Floor);
    Floor->SetRootComponent(FloorBody);
    FloorBody->SetBoxExtent(FVector(10000.0f, 10000.0f, 50.0f));
    FloorBody->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    FloorBody->SetCollisionResponseToAllChannels(ECR_Block);
    FloorBody->RegisterComponent();
    Floor->SetActorLocation(FVector(0.0f, 0.0f, -50.0f));
    FMassEntityManager& Manager = World->GetSubsystem<UMassEntitySubsystem>()->GetMutableEntityManager();
    const FMassArchetypeHandle Type = Manager.CreateArchetype({
        FTransformFragment::StaticStruct(), FBBBMonsterBehaviorFragment::StaticStruct(),
        FBBBMonsterMovementFragment::StaticStruct(), FBBBMonsterNavigationFragment::StaticStruct(),
        FBBBMonsterCombatFragment::StaticStruct(), FBBBMonsterTargetFragment::StaticStruct(),
        FBBBMonsterHealthFragment::StaticStruct(), FBBBMonsterDamageFragment::StaticStruct(),
        FBBBMonsterDeathFragment::StaticStruct(), FBBBMonsterTag::StaticStruct(),
        FMassVelocityFragment::StaticStruct(), FBBBMonsterAvoidanceFragment::StaticStruct(),
        FBBBMonsterGroundFragment::StaticStruct(), FBBBMonsterMobilityFragment::StaticStruct(), FBBBMonsterNetworkFragment::StaticStruct()
    });
    const FMassEntityHandle Entity = Manager.CreateEntity(Type);
    Manager.GetFragmentDataChecked<FBBBMonsterGroundFragment>(Entity).bGrounded = true;
    UBBBMonsterBehaviorProcessor* Processor = NewObject<UBBBMonsterBehaviorProcessor>(World);
    Processor->CallInitialize(World, Manager.AsShared());
    const auto Run = [&Manager, Processor]()
    {
        UE::Mass::FProcessingContext Context(Manager, 0.016f);
        UE::Mass::Executor::Run(*Processor, Context);
    };
    FBBBMonsterBehaviorFragment& State = Manager.GetFragmentDataChecked<FBBBMonsterBehaviorFragment>(Entity);
    FBBBMonsterNavigationFragment& Navigation = Manager.GetFragmentDataChecked<FBBBMonsterNavigationFragment>(Entity);
    Run();
    TestEqual(TEXT("出生待机"), State.State, EBBBMonsterBehavior::Idle);
    TestTrue(TEXT("待机随机时长范围"), State.StateEndsAtTime >= State.StateEnteredTime + 2.0f && State.StateEndsAtTime <= State.StateEnteredTime + 4.0f);
    TSet<float> Durations;
    for (int32 Cycle = 0; Cycle < 12; ++Cycle)
    {
        State.StateEndsAtTime = World->GetTimeSeconds();
        const uint32 Previous = State.ActionId;
        Run();
        TestEqual(TEXT("待机后巡逻"), State.State, EBBBMonsterBehavior::Patrol);
        TestEqual(TEXT("新巡逻新动作身份"), State.ActionId, Previous + 1);
        const float Duration = State.StateEndsAtTime - State.StateEnteredTime;
        TestTrue(TEXT("巡逻随机时长范围"), Duration >= 2.0f && Duration <= 5.0f);
        Durations.Add(Duration);
        Navigation.ActionId = State.ActionId;
        Navigation.bReachedDestination = true;
        Run();
        TestEqual(TEXT("到达后待机"), State.State, EBBBMonsterBehavior::Idle);
    }
    TestTrue(TEXT("巡逻时长确实变化"), Durations.Num() > 1);
    State.bHadTarget = true;
    Run();
    TestEqual(TEXT("丢失目标先警觉"), State.State, EBBBMonsterBehavior::Alert);
    TestTrue(TEXT("警觉独立计时"), State.StateEndsAtTime > State.StateEnteredTime);
    State.StateEndsAtTime = World->GetTimeSeconds();
    Run();
    TestEqual(TEXT("警觉结束恢复待机"), State.State, EBBBMonsterBehavior::Idle);

    AActor* TargetActor = World->SpawnActor<AActor>();
    if (!TestNotNull(TEXT("攻击检测目标"), TargetActor))
    {
        Manager.DestroyEntity(Entity);
        return false;
    }
    FBBBMonsterTargetFragment& Target = Manager.GetFragmentDataChecked<FBBBMonsterTargetFragment>(Entity);
    Target.bHasTarget = true;
    Target.TargetActor = TargetActor;
    Target.TargetLocation = TargetActor->GetActorLocation();
    FTransform& Transform = Manager.GetFragmentDataChecked<FTransformFragment>(Entity).GetMutableTransform();
    Transform.SetLocation(Target.TargetLocation - FVector(150.0f, 0.0f, 0.0f));
    Transform.SetLocation(FVector(Transform.GetLocation().X, Transform.GetLocation().Y, 90.5f));
    FBBBMonsterCombatFragment& Combat = Manager.GetFragmentDataChecked<FBBBMonsterCombatFragment>(Entity);
    Run();
    TestEqual(TEXT("发现目标先警觉"), State.State, EBBBMonsterBehavior::Alert);
    State.StateEndsAtTime = World->GetTimeSeconds();
    Run();
    TestEqual(TEXT("满足攻击范围直接攻击"), State.State, EBBBMonsterBehavior::Attack);
    TestEqual(TEXT("一次攻击独立编号"), Combat.AttackId, 1u);
    Manager.GetFragmentDataChecked<FBBBMonsterDamageFragment>(Entity).bReceivedDamage = true;
    Run();
    TestEqual(TEXT("普通受击保留当前攻击"), State.State, EBBBMonsterBehavior::Attack);
    TestEqual(TEXT("普通受击不重启攻击编号"), Combat.AttackId, 1u);
    TestFalse(TEXT("普通受击不取消攻击判定"), Combat.bAttackFinished);


    UBBBMonsterLocomotionProcessor* Locomotion = NewObject<UBBBMonsterLocomotionProcessor>(World);
    Locomotion->CallInitialize(World, Manager.AsShared());
    UBBBMonsterCombatProcessor* CombatProcessor = NewObject<UBBBMonsterCombatProcessor>(World);
    CombatProcessor->CallInitialize(World, Manager.AsShared());
    const auto RunProcessor = [&Manager](UMassProcessor* Current)
    {
        UE::Mass::FProcessingContext Context(Manager, 0.016f);
        UE::Mass::Executor::Run(*Current, Context);
    };
    FVector& Velocity = Manager.GetFragmentDataChecked<FMassVelocityFragment>(Entity).Value;
    const FVector AttackLocation = Transform.GetLocation();
    const FQuat AttackFacing = Transform.GetRotation();
    for (int32 Frame = 0; Frame < 5; ++Frame)
    {
        Velocity = FVector(500.0f, 0.0f, 0.0f);
        Run();
        RunProcessor(Locomotion);
        TestTrue(TEXT("攻击期间位置不改变"), Transform.GetLocation().Equals(AttackLocation, 0.01f));
        TestEqual(TEXT("攻击期间速度归零"), Velocity, FVector::ZeroVector);
        TestEqual(TEXT("攻击期间不受速度修正影响朝向"), Transform.GetRotation(), AttackFacing);
        TestEqual(TEXT("未完成攻击不得重启"), Combat.AttackId, 1u);
    }

    State.StateEnteredTime = World->GetTimeSeconds() - Combat.AttackWindup - Combat.AttackRecovery - 0.01f;
    RunProcessor(CombatProcessor);
    TestTrue(TEXT("战斗处理器确认一次攻击完成"), Combat.bAttackFinished);
    TestTrue(TEXT("每次攻击只判定一次命中"), Combat.bHitAttempted);
    Combat.NextAttackTime = World->GetTimeSeconds();
    Run();
    TestEqual(TEXT("攻击完成当帧重新检测并攻击"), State.State, EBBBMonsterBehavior::Attack);
    TestEqual(TEXT("下一次攻击使用新编号"), Combat.AttackId, 2u);
    TestFalse(TEXT("下一次攻击重新开放唯一命中"), Combat.bHitAttempted);
    RunProcessor(Locomotion);
    TestTrue(TEXT("连续攻击之间不插入前移"), Transform.GetLocation().Equals(AttackLocation, 0.01f));
    TestEqual(TEXT("连续攻击之间速度保持归零"), Velocity, FVector::ZeroVector);
    TestEqual(TEXT("恢复追击后的目标档位为跑步"), UBBBMonsterLocomotionProcessor::SelectGait(
        Manager.GetFragmentDataChecked<FBBBMonsterMovementFragment>(Entity), 150.0f, false), EBBBMonsterGait::Run);
    Run();
    TestEqual(TEXT("未完成的新攻击保持原地执行"), State.State, EBBBMonsterBehavior::Attack);
    TestEqual(TEXT("逐帧检测不重复开启新攻击"), Combat.AttackId, 2u);
    Combat.bAttackFinished = true;
    Combat.NextAttackTime = World->GetTimeSeconds() + 10.0f;
    Run();
    TestEqual(TEXT("范围内冷却期间仍追击而非待机"), State.State, EBBBMonsterBehavior::Chase);
    Target.TargetLocation.Z += Combat.AttackRange * 2.0f;
    Combat.NextAttackTime = World->GetTimeSeconds();
    Run();
    TestEqual(TEXT("高度超出范围不能触发攻击"), State.State, EBBBMonsterBehavior::Chase);
    TestEqual(TEXT("不满足攻击条件不新增攻击编号"), Combat.AttackId, 2u);

    Target.TargetLocation.Z -= Combat.AttackRange * 2.0f;
    FloorBody->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    State.State = EBBBMonsterBehavior::Attack;
    Combat.bHitAttempted = false;
    Combat.bAttackFinished = false;
    Combat.AttackTarget = TargetActor;
    Velocity = FVector::ZeroVector;
    RunProcessor(Locomotion);
    TestTrue(TEXT("攻击中失去支撑仍下落"), Transform.GetLocation().Z < AttackLocation.Z && Velocity.Z < 0.0f);
    RunProcessor(CombatProcessor);
    TestTrue(TEXT("失去支撑当帧取消命中"), Combat.bHitAttempted && Combat.bAttackFinished && !Combat.AttackTarget.IsValid());
    Run();
    TestEqual(TEXT("空中取消攻击恢复追击状态"), State.State, EBBBMonsterBehavior::Chase);
    Run();
    TestEqual(TEXT("空中处于攻击范围也不得攻击"), State.State, EBBBMonsterBehavior::Chase);
    TestEqual(TEXT("空中不新增攻击编号"), Combat.AttackId, 2u);

    Manager.GetFragmentDataChecked<FBBBMonsterDamageFragment>(Entity).bReceivedDamage = true;
    Run();
    TestEqual(TEXT("普通受击不打断追击"), State.State, EBBBMonsterBehavior::Chase);
    Manager.GetFragmentDataChecked<FBBBMonsterHealthFragment>(Entity).CurrentHealth = 0.0f;
    Run();
    TestEqual(TEXT("死亡优先打断"), State.State, EBBBMonsterBehavior::Dead);
    Run();
    TestEqual(TEXT("死亡不恢复"), State.State, EBBBMonsterBehavior::Dead);
    Manager.DestroyEntity(Entity);
    return true;
}

#endif
