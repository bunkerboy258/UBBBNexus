#if WITH_DEV_AUTOMATION_TESTS && WITH_EDITOR

#include "Misc/AutomationTest.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Movement/BBBMonsterMobilityFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Network/BBBMonsterNetworkFragment.h"
#include "Misc/ScopeExit.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "MassCommonFragments.h"
#include "MassMovementFragments.h"
#include "MassEntitySubsystem.h"
#include "MassExecutor.h"
#include "MassProcessingContext.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Processors/Movement/BBBMonsterLocomotionProcessor.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Movement/BBBMonsterGroundFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Movement/BBBMonsterNavigationFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Movement/BBBMonsterAvoidanceFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Behavior/BBBMonsterBehaviorFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Tags/BBBMonsterTag.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Input/AuthorityFact/Network/FBBBMonsterStateAuthorityFactPacket.h"

/** 验证真实碰撞地面的重力 支撑 台阶 坡度与死亡事实还原 */
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBBBMonsterGroundTest, "UBBB.Mass.ZombieGround",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FBBBMonsterGroundTest::RunTest(const FString& Parameters)
{
    const auto Initialization = UWorld::InitializationValues()
        .AllowAudioPlayback(false).CreatePhysicsScene(true).CreateNavigation(false)
        .CreateAISystem(false).ShouldSimulatePhysics(false);
    UWorld* World = UWorld::CreateWorld(EWorldType::Game, false, NAME_None, nullptr, true, ERHIFeatureLevel::Num, &Initialization);
    if (!TestNotNull(TEXT("地面隔离世界"), World))
    {
        return false;
    }
    GEngine->CreateNewWorldContext(EWorldType::Game).SetCurrentWorld(World);
    ON_SCOPE_EXIT
    {
        GEngine->DestroyWorldContext(World);
        World->DestroyWorld(false);
    };
    const auto MakeBox = [World](const FVector& Position, const FVector& Extent, const FRotator& Rotation = FRotator::ZeroRotator)
    {
        AActor* Actor = World->SpawnActor<AActor>();
        UBoxComponent* Body = NewObject<UBoxComponent>(Actor);
        Actor->SetRootComponent(Body);
        Body->SetBoxExtent(Extent);
        Body->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
        Body->SetCollisionResponseToAllChannels(ECR_Block);
        Body->RegisterComponent();
        Actor->SetActorLocationAndRotation(Position, Rotation);
        return Body;
    };
    UBoxComponent* Floor = MakeBox(FVector(0.0f, 0.0f, -50.0f), FVector(2000.0f, 2000.0f, 50.0f));
    FBBBMonsterMovementFragment Movement;
    FBBBMonsterGroundFragment Ground;
    FVector Location(0.0f, 0.0f, 1000.0f);
    FVector Velocity = FVector::ZeroVector;
    const auto Solve = [&](const float Seconds, const FVector& Horizontal = FVector::ZeroVector)
    {
        UBBBMonsterLocomotionProcessor::SolveGroundMotion(*World, Movement, Ground, Location, Velocity, Horizontal, Seconds);
    };
    Solve(0.1f);
    TestTrue(TEXT("高处出生开始下落"), Location.Z < 1000.0f && Velocity.Z < 0.0f && !Ground.bGrounded);
    TestTrue(TEXT("使用世界重力"), FMath::IsNearlyEqual(Velocity.Z, World->GetGravityZ() * 0.1f, 0.01f));
    Solve(2.0f);
    TestTrue(TEXT("两秒长帧不会穿过地面"), Ground.bGrounded && FMath::IsNearlyEqual(Location.Z, 90.5f, 0.1f));
    TestEqual(TEXT("落地消除向下速度"), Velocity.Z, 0.0);
    const FVector Landed = Location;
    for (int32 Frame = 0; Frame < 180; ++Frame)
    {
        Solve(1.0f / 60.0f);
    }
    TestTrue(TEXT("站立三秒无高度漂移"), Location.Equals(Landed, 0.01f));
    Solve(0.0f);
    TestTrue(TEXT("零时长不移动"), Location.Equals(Landed, 0.01f));
    Floor->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    Solve(0.1f);
    TestTrue(TEXT("移除支撑立即下落"), !Ground.bGrounded && Location.Z < Landed.Z && Velocity.Z < 0.0f);
    Floor->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    Location = FVector(0.0f, 0.0f, 90.5f);
    Velocity = FVector::ZeroVector;
    Ground.bGrounded = true;

    UBoxComponent* Step = MakeBox(FVector(200.0f, 0.0f, 15.0f), FVector(100.0f, 200.0f, 15.0f));
    for (int32 Frame = 0; Frame < 50; ++Frame)
    {
        Solve(0.02f, FVector(4.0f, 0.0f, 0.0f));
    }
    TestTrue(TEXT("可跨越三十厘米台阶"), Location.X > 180.0f && Location.Z > 119.0f && Ground.bGrounded);
    UE_LOG(LogTemp, Display, TEXT("[BBBMonsterGroundCheck] Step X=%.3f Z=%.3f Grounded=%d NormalZ=%.3f"), Location.X, Location.Z, Ground.bGrounded, Ground.SupportNormal.Z);
    Step->DestroyComponent();
    for (const float Height : {5.0f, 15.0f, 30.0f})
    {
        for (const float Lateral : {0.0f, 2.0f})
        {
            auto* SmallStep = MakeBox(FVector(200.0f, 0.0f, Height * 0.5f), FVector(100.0f, 300.0f, Height * 0.5f));
            Location = FVector(0.0f, 0.0f, 90.5f);
            Velocity = FVector::ZeroVector;
            Ground = {};
            for (int32 Frame = 0; Frame < 60; ++Frame)
            {
                Solve(1.0f / 30.0f, FVector(4.0f, Lateral, 0.0f));
            }
            TestTrue(FString::Printf(TEXT("台阶%.0f厘米斜向%.0f可跨越"), Height, Lateral),
                Location.X > 220.0f && Ground.bGrounded && FMath::IsNearlyEqual(Location.Z, 90.5f + Height, 1.0f));
            SmallStep->DestroyComponent();
        }
    }
    Movement.CapsuleHalfHeight = 45.0f;
    Movement.MaxStepHeight = 10.0f;
    for (const float Height : {5.0f, 15.0f})
    {
        auto* CrawlStep = MakeBox(FVector(200.0f, 0.0f, Height * 0.5f), FVector(100.0f, 300.0f, Height * 0.5f));
        Location = FVector(0.0f, 0.0f, 45.5f);
        Velocity = FVector::ZeroVector;
        Ground = {};
        for (int32 Frame = 0; Frame < 60; ++Frame)
        {
            Solve(1.0f / 30.0f, FVector(4.0f, 0.0f, 0.0f));
        }
        AddInfo(FString::Printf(TEXT("[BBBCrawlStep] Height=%.1f Position=%s Grounded=%d Normal=%s"),
            Height, *Location.ToString(), Ground.bGrounded, *Ground.SupportNormal.ToString()));
        TestTrue(FString::Printf(TEXT("爬行台阶%.0f厘米遵守十厘米上限"), Height),
            Height <= 10.0f ? Location.X > 220.0f && Ground.bGrounded : Location.X < 100.0f && Ground.bGrounded);
        CrawlStep->DestroyComponent();
    }
    Movement = {};
    UBoxComponent* Ceiling = MakeBox(FVector(200.0f, 0.0f, 190.0f), FVector(100.0f, 300.0f, 5.0f));
    UBoxComponent* CeilingStep = MakeBox(FVector(200.0f, 0.0f, 15.0f), FVector(100.0f, 300.0f, 15.0f));
    Location = FVector(0.0f, 0.0f, 90.5f);
    Velocity = FVector::ZeroVector;
    Ground = {};
    for (int32 Frame = 0; Frame < 60; ++Frame)
    {
        Solve(1.0f / 30.0f, FVector(4.0f, 0.0f, 0.0f));
    }
    TestTrue(TEXT("低顶空间不能穿顶登上台阶"), Location.X < 100.0f && Ground.bGrounded);
    Ceiling->DestroyComponent();
    CeilingStep->DestroyComponent();
    UBoxComponent* Wall = MakeBox(FVector(200.0f, 0.0f, 150.0f), FVector(50.0f, 200.0f, 150.0f));
    Location = FVector(0.0f, 0.0f, 90.5f);
    Velocity = FVector::ZeroVector;
    for (int32 Frame = 0; Frame < 50; ++Frame)
    {
        Solve(0.02f, FVector(5.0f, 0.0f, 0.0f));
    }
    TestTrue(TEXT("高墙不能当台阶穿过"), Location.X <= 106.0f && Ground.bGrounded);
    UE_LOG(LogTemp, Display, TEXT("[BBBMonsterGroundCheck] Wall X=%.3f Z=%.3f Grounded=%d"), Location.X, Location.Z, Ground.bGrounded);
    Solve(0.2f, FVector(10.0f, 60.0f, 0.0f));
    TestTrue(TEXT("贴墙仍可沿切线滑动"), Location.X <= 106.0f && Location.Y > 50.0f && Ground.bGrounded);
    Wall->DestroyComponent();
    Floor->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    UBoxComponent* Slope = MakeBox(FVector(0.0f, 0.0f, -50.0f), FVector(1000.0f, 1000.0f, 50.0f), FRotator(25.0f, 0.0f, 0.0f));
    Location = FVector(0.0f, 0.0f, 1000.0f);
    Velocity = FVector::ZeroVector;
    Solve(2.0f);
    TestTrue(TEXT("可行走坡面提供支撑"), Ground.bGrounded && Ground.SupportNormal.Z >= Movement.WalkableFloorZ);
    const float SlopeStart = Location.Z;
    Solve(0.2f, FVector(50.0f, 0.0f, 0.0f));
    TestTrue(TEXT("坡面移动保持支撑并改变高度"), Ground.bGrounded && FMath::Abs(Location.Z - SlopeStart) > 10.0f);
    Slope->SetWorldRotation(FRotator(70.0f, 0.0f, 0.0f));
    Location = FVector(0.0f, 0.0f, 1000.0f);
    Velocity = FVector::ZeroVector;
    Solve(1.5f);
    TestFalse(TEXT("陡坡不能判定站立"), Ground.bGrounded);
    Slope->DestroyComponent();
    Floor->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

    FMassEntityManager& Manager = World->GetSubsystem<UMassEntitySubsystem>()->GetMutableEntityManager();
    const FMassArchetypeHandle Type = Manager.CreateArchetype({
        FTransformFragment::StaticStruct(), FMassVelocityFragment::StaticStruct(),
        FBBBMonsterMovementFragment::StaticStruct(), FBBBMonsterGroundFragment::StaticStruct(),
        FBBBMonsterNavigationFragment::StaticStruct(), FBBBMonsterAvoidanceFragment::StaticStruct(),
        FBBBMonsterBehaviorFragment::StaticStruct(), FBBBMonsterTag::StaticStruct(),
        FBBBMonsterMobilityFragment::StaticStruct(), FBBBMonsterNetworkFragment::StaticStruct()
    });
    const FMassEntityHandle Entity = Manager.CreateEntity(Type);
    UBBBMonsterLocomotionProcessor* Processor = NewObject<UBBBMonsterLocomotionProcessor>(World);
    Processor->CallInitialize(World, Manager.AsShared());
    FTransform& Transform = Manager.GetFragmentDataChecked<FTransformFragment>(Entity).GetMutableTransform();
    FVector& EntityVelocity = Manager.GetFragmentDataChecked<FMassVelocityFragment>(Entity).Value;
    FBBBMonsterBehaviorFragment& State = Manager.GetFragmentDataChecked<FBBBMonsterBehaviorFragment>(Entity);
    for (uint8 Value = 0; Value <= static_cast<uint8>(EBBBMonsterBehavior::Dead); ++Value)
    {
        State.State = static_cast<EBBBMonsterBehavior>(Value);
        Transform.SetLocation(FVector(0.0f, 0.0f, 1000.0f));
        EntityVelocity = FVector(500.0f, 0.0f, 0.0f);
        UE::Mass::FProcessingContext Context(Manager, 0.1f);
        UE::Mass::Executor::Run(*Processor, Context);
        TestTrue(FString::Printf(TEXT("状态%d无路径仍受重力影响"), Value), Transform.GetLocation().Z < 1000.0f && EntityVelocity.Z < 0.0f);
        TestEqual(TEXT("非移动状态不保留主动水平位移"), Transform.GetLocation().X, 0.0);
        TestEqual(TEXT("下落不会产生俯仰"), Transform.Rotator().Pitch, 0.0);
        UE::Mass::FProcessingContext LandingContext(Manager, 2.0f);
        UE::Mass::Executor::Run(*Processor, LandingContext);
        TestTrue(TEXT("全部状态可落地而非悬空"), Manager.GetFragmentDataChecked<FBBBMonsterGroundFragment>(Entity).bGrounded &&
            FMath::IsNearlyEqual(Transform.GetLocation().Z, 90.5f, 0.1f) && EntityVelocity.Z == 0.0f);
    }
    State.State = EBBBMonsterBehavior::Chase;
    Transform.SetLocation(FVector(0.0f, 0.0f, 1000.0f));
    EntityVelocity = FVector(500.0f, 0.0f, -100.0f);
    Manager.GetFragmentDataChecked<FBBBMonsterGroundFragment>(Entity).bGrounded = false;
    auto& Path = Manager.GetFragmentDataChecked<FBBBMonsterNavigationFragment>(Entity);
    Path.bHasPath = true;
    Path.PathPointIndex = 0;
    Path.PathPoints = {FVector(2000.0f, 0.0f, 90.0f)};
    Path.TailDistances = {0.0f};
    Manager.GetFragmentDataChecked<FBBBMonsterMobilityFragment>(Entity).HitStopEndsAt = World->GetTimeSeconds() + 1.0f;
    UE::Mass::FProcessingContext StopContext(Manager, 0.1f);
    UE::Mass::Executor::Run(*Processor, StopContext);
    TestEqual(TEXT("空中受击只停止水平移动"), Transform.GetLocation().X, 0.0);
    TestTrue(TEXT("受击停顿仍持续重力和落地求解"), Transform.GetLocation().Z < 1000.0f && EntityVelocity.Z < -100.0f);
    Manager.DestroyEntity(Entity);

    FBBBMonsterNetworkFragment Network;
    FTransformFragment Position;
    FMassVelocityFragment Speed;
    FBBBMonsterBehaviorFragment DeadState;
    DeadState.State = EBBBMonsterBehavior::Dead;
    DeadState.ActionId = 17;
    FBBBMonsterStateAuthorityFactPacket Fact;
    Fact.InstanceId = FGuid::NewGuid();
    Fact.Revision = 1;
    Fact.Behavior = EBBBMonsterBehavior::Dead;
    Fact.Transform.SetLocation(FVector(1.0f, 2.0f, 400.0f));
    Fact.Velocity = FVector(0.0f, 0.0f, -200.0f);
    Fact.Apply(Network, Position, Speed, DeadState);
    TestEqual(TEXT("死亡实体可还原下落位置"), Position.GetTransform().GetLocation(), Fact.Transform.GetLocation());
    TestEqual(TEXT("死亡实体可还原下落速度"), Speed.Value, Fact.Velocity);
    TestEqual(TEXT("位置事实不重启死亡动作"), DeadState.ActionId, 17u);
    Fact.Revision = 2;
    Fact.Behavior = EBBBMonsterBehavior::Chase;
    Fact.Transform.SetLocation(FVector(999.0f));
    Fact.Apply(Network, Position, Speed, DeadState);
    TestEqual(TEXT("迟到活体事实不能覆盖尸体位置"), Position.GetTransform().GetLocation(), FVector(1.0f, 2.0f, 400.0f));
    TestEqual(TEXT("迟到事实不能复活"), DeadState.State, EBBBMonsterBehavior::Dead);
    return true;
}

#endif
