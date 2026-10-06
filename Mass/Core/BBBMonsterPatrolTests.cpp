#if WITH_DEV_AUTOMATION_TESTS && WITH_EDITOR

#include "Misc/AutomationTest.h"
#include "Misc/ScopeExit.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "MassCommonFragments.h"
#include "MassEntitySubsystem.h"
#include "MassExecutor.h"
#include "MassProcessingContext.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Processors/Behavior/BBBMonsterBehaviorProcessor.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Behavior/BBBMonsterBehaviorFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Movement/BBBMonsterMovementFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Movement/BBBMonsterNavigationFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Combat/BBBMonsterCombatFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Perception/BBBMonsterTargetFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Health/BBBMonsterHealthFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Health/BBBMonsterDamageFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Health/BBBMonsterDeathFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Tags/BBBMonsterTag.h"

/** 验证随机走停循环 警觉与受击死亡优先级 */
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBBBMonsterPatrolTest, "UBBB.Mass.ZombiePatrol",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FBBBMonsterPatrolTest::RunTest(const FString& Parameters)
{
    const auto Initialization = UWorld::InitializationValues()
        .AllowAudioPlayback(false).CreatePhysicsScene(false).CreateNavigation(false)
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
    FMassEntityManager& Manager = World->GetSubsystem<UMassEntitySubsystem>()->GetMutableEntityManager();
    const FMassArchetypeHandle Type = Manager.CreateArchetype({
        FTransformFragment::StaticStruct(), FBBBMonsterBehaviorFragment::StaticStruct(),
        FBBBMonsterMovementFragment::StaticStruct(), FBBBMonsterNavigationFragment::StaticStruct(),
        FBBBMonsterCombatFragment::StaticStruct(), FBBBMonsterTargetFragment::StaticStruct(),
        FBBBMonsterHealthFragment::StaticStruct(), FBBBMonsterDamageFragment::StaticStruct(),
        FBBBMonsterDeathFragment::StaticStruct(), FBBBMonsterTag::StaticStruct()
    });
    const FMassEntityHandle Entity = Manager.CreateEntity(Type);
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
    Manager.GetFragmentDataChecked<FBBBMonsterDamageFragment>(Entity).bReceivedDamage = true;
    Run();
    TestEqual(TEXT("受击优先打断"), State.State, EBBBMonsterBehavior::Hurt);
    Manager.GetFragmentDataChecked<FBBBMonsterHealthFragment>(Entity).CurrentHealth = 0.0f;
    Run();
    TestEqual(TEXT("死亡优先打断"), State.State, EBBBMonsterBehavior::Dead);
    Run();
    TestEqual(TEXT("死亡不恢复"), State.State, EBBBMonsterBehavior::Dead);
    Manager.DestroyEntity(Entity);
    return true;
}

#endif
