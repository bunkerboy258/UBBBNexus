#if WITH_DEV_AUTOMATION_TESTS && WITH_EDITOR

#include "Misc/AutomationTest.h"
#include "Misc/ScopeExit.h"
#include "Engine/World.h"
#include "MassCommonFragments.h"
#include "MassMovementFragments.h"
#include "MassEntitySubsystem.h"
#include "MassExecutor.h"
#include "MassProcessingContext.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Config/BBBMonsterDefinition.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Behavior/BBBMonsterBehaviorFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Movement/BBBMonsterAvoidanceFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Movement/BBBMonsterMovementFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Movement/BBBMonsterNavigationFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Network/BBBMonsterNetworkFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Processors/Movement/BBBMonsterAvoidanceProcessor.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Processors/Movement/BBBMonsterSteering.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Tags/BBBMonsterTag.h"

/** 连续扰动下检查真实避让处理器的方向保持与停止结果 */
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBBBMonsterSteeringStabilityTest, "UBBB.Mass.ZombieSteering.Stability",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FBBBMonsterSteeringStabilityTest::RunTest(const FString& Parameters)
{
    const auto Initialization = UWorld::InitializationValues().AllowAudioPlayback(false)
        .CreatePhysicsScene(false).CreateNavigation(false).CreateAISystem(false);
    UWorld* World = UWorld::CreateWorld(EWorldType::Game, false, NAME_None, nullptr, true,
        ERHIFeatureLevel::Num, &Initialization);
    if (!TestNotNull(TEXT("避让测试世界"), World))
    {
        return false;
    }
    ON_SCOPE_EXIT
    {
        World->DestroyWorld(false);
        World->GetOutermost()->SetDirtyFlag(false);
    };
    auto& Manager = World->GetSubsystem<UMassEntitySubsystem>()->GetMutableEntityManager();
    const auto Type = Manager.CreateArchetype({FTransformFragment::StaticStruct(), FMassVelocityFragment::StaticStruct(),
        FBBBMonsterBehaviorFragment::StaticStruct(), FBBBMonsterNavigationFragment::StaticStruct(),
        FBBBMonsterMovementFragment::StaticStruct(), FBBBMonsterAvoidanceFragment::StaticStruct(),
        FBBBMonsterNetworkFragment::StaticStruct(), FBBBMonsterTag::StaticStruct()});
    auto* Definition = NewObject<UBBBMonsterDefinition>(World);
    const auto Walker = Manager.CreateEntity(Type);
    const auto Neighbor = Manager.CreateEntity(Type);
    Manager.GetFragmentDataChecked<FBBBMonsterNetworkFragment>(Walker).Definition = Definition;
    Manager.GetFragmentDataChecked<FBBBMonsterNetworkFragment>(Neighbor).Definition = Definition;
    Manager.GetFragmentDataChecked<FBBBMonsterBehaviorFragment>(Walker).State = EBBBMonsterBehavior::Chase;
    Manager.GetFragmentDataChecked<FBBBMonsterBehaviorFragment>(Neighbor).State = EBBBMonsterBehavior::Attack;
    auto& Path = Manager.GetFragmentDataChecked<FBBBMonsterNavigationFragment>(Walker);
    Path.bHasFollowTarget = true;
    Path.FollowTarget = FVector(1000.0f, 0.0f, 0.0f);
    auto& Body = Manager.GetFragmentDataChecked<FTransformFragment>(Neighbor).GetMutableTransform();
    auto* Avoidance = NewObject<UBBBMonsterAvoidanceProcessor>(World);
    Avoidance->CallInitialize(World, Manager.AsShared());
    auto& Result = Manager.GetFragmentDataChecked<FBBBMonsterAvoidanceFragment>(Walker);
    auto& Velocity = Manager.GetFragmentDataChecked<FMassVelocityFragment>(Walker).Value;
    Velocity = FVector(300.0f, 0.0f, 0.0f);
    for (int32 Frame = 0; Frame < 240; ++Frame)
    {
        World->TimeSeconds = Frame / 60.0;
        Body.SetLocation(FVector(100.0f, Frame % 2 == 0 ? 0.2f : -0.2f, 0.0f));
        UE::Mass::FProcessingContext Context(Manager, 1.0f / 60.0f);
        UE::Mass::Executor::Run(*Avoidance, Context);
        TestTrue(TEXT("微小邻居扰动保持前进半平面"), Result.SteeringDirection.X >= -KINDA_SMALL_NUMBER);
        TestFalse(TEXT("连续避让结果保持有限"), Result.SteeringDirection.ContainsNaN());
        TestTrue(TEXT("本轮许可距离满足身体间距"), Result.MaxTravelDistance >= 0.0f);
        Velocity = Result.SteeringDirection * 300.0f;
    }
    TestTrue(TEXT("四秒微小扰动至多确认一次换边"), Result.SideChanges <= 1u);
    Body.SetLocation(FVector(80.0f, 0.0f, 0.0f));
    Velocity = FVector::ZeroVector;
    UE::Mass::FProcessingContext BlockedContext(Manager, 1.0f / 60.0f);
    UE::Mass::Executor::Run(*Avoidance, BlockedContext);
    TestTrue(TEXT("身体重叠时安全等待而非掉头穿过邻居"), Result.bWaitingForNeighbor && Result.MaxTravelDistance == 0.0f);
    Manager.DestroyEntity(Neighbor);
    UE::Mass::FProcessingContext ClearContext(Manager, 1.0f / 60.0f);
    UE::Mass::Executor::Run(*Avoidance, ClearContext);
    TestTrue(TEXT("邻居离开后立即恢复前进"), !Result.bWaitingForNeighbor && Result.SteeringDirection.X > 0.9f);
    return true;
}

/** 朝向模式在攻击距离边界和短时停走扰动下保持稳定 */
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBBBMonsterSteeringFacingTest, "UBBB.Mass.ZombieSteering.Facing",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FBBBMonsterSteeringFacingTest::RunTest(const FString& Parameters)
{
    FBBBMonsterMovementFragment Movement;
    FBBBMonsterSteering::UpdateFacingMode(Movement, true, false, true, false, 109.0f, 110.0f, 45.0f, 0.0f);
    TestTrue(TEXT("进入近战接近距离后面向玩家"), Movement.bFacingTarget);
    for (int32 Frame = 1; Frame < 300; ++Frame)
    {
        const float Distance = Frame % 2 == 0 ? 109.0f : 111.0f;
        FBBBMonsterSteering::UpdateFacingMode(Movement, true, false, true, Frame % 2 == 0,
            Distance, 110.0f, 45.0f, Frame / 60.0f);
        TestTrue(TEXT("距离边界和单帧停走不反复切换朝向依据"), Movement.bFacingTarget);
    }
    FBBBMonsterSteering::UpdateFacingMode(Movement, true, false, true, false, 200.0f, 110.0f, 45.0f, 6.0f);
    TestFalse(TEXT("离开退出缓冲后恢复移动朝向"), Movement.bFacingTarget);
    for (int32 Frame = 0; Frame < 180; ++Frame)
    {
        FBBBMonsterSteering::UpdateFacingMode(Movement, true, false, true, Frame % 12 < 6,
            400.0f, 110.0f, 45.0f, 7.0f + Frame / 60.0f);
        TestFalse(TEXT("不足四分之一秒的拥挤停走保持行走朝向"), Movement.bFacingTarget);
    }
    FBBBMonsterSteering::UpdateFacingMode(Movement, true, false, true, true, 400.0f, 110.0f, 45.0f, 11.0f);
    FBBBMonsterSteering::UpdateFacingMode(Movement, true, false, true, true, 400.0f, 110.0f, 45.0f, 11.3f);
    TestTrue(TEXT("持续拥挤等待后面向玩家"), Movement.bFacingTarget);
    FBBBMonsterSteering::UpdateFacingMode(Movement, false, false, false, false, 0.0f, 110.0f, 45.0f, 12.0f);
    TestFalse(TEXT("丢失目标清除目标朝向模式"), Movement.bFacingTarget);
    for (const float FramesPerSecond : {30.0f, 60.0f, 120.0f})
    {
        const float Step = 1.0f / FramesPerSecond;
        const FVector Direction = FBBBMonsterSteering::ConstrainDirection(FVector(300.0f, 0.0f, 0.0f),
            FVector::RightVector, 600.0f, 260.0f, Step);
        TestTrue(TEXT("运动转向受横向加速度约束"), FMath::Abs(Direction.Rotation().Yaw)
            <= FMath::RadiansToDegrees(600.0f / 300.0f) * Step + 0.001f);
    }
    return true;
}

#endif
