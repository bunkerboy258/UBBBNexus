#if WITH_DEV_AUTOMATION_TESTS && WITH_EDITOR

#include "Misc/AutomationTest.h"
#include "Misc/ScopeExit.h"
#include "HAL/PlatformTime.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "Engine/StaticMesh.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "NavigationSystem.h"
#include "NavMesh/NavMeshBoundsVolume.h"
#include "NavMesh/RecastNavMesh.h"
#include "MassCommonFragments.h"
#include "MassMovementFragments.h"
#include "MassEntitySubsystem.h"
#include "MassExecutor.h"
#include "MassProcessingContext.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Config/BBBMonsterDefinition.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Behavior/BBBMonsterBehaviorFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Movement/BBBMonsterMovementFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Movement/BBBMonsterNavigationFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Movement/BBBMonsterGroundFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Movement/BBBMonsterMobilityFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Movement/BBBMonsterAvoidanceFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Network/BBBMonsterNetworkFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Perception/BBBMonsterTargetFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Tags/BBBMonsterTag.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Processors/Movement/BBBMonsterNavigationProcessor.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Processors/Movement/BBBMonsterLocomotionProcessor.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Processors/Movement/BBBMonsterAvoidanceProcessor.h"

/** 在隔离真实导航与碰撞世界验证绕墙 脱困 包围和五十只移动 */
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBBBMonsterNavigationTest, "UBBB.Mass.ZombieNavigation",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FBBBMonsterNavigationTest::RunTest(const FString& Parameters)
{
    const auto Initialization = UWorld::InitializationValues().AllowAudioPlayback(false).CreatePhysicsScene(true).CreateNavigation(true).CreateAISystem(true);
    UWorld* World = UWorld::CreateWorld(EWorldType::Game, false, NAME_None, nullptr, true, ERHIFeatureLevel::Num, &Initialization);
    if (!TestNotNull(TEXT("导航隔离世界"), World))
    {
        return false;
    }
    GEngine->CreateNewWorldContext(EWorldType::Game).SetCurrentWorld(World);
    ON_SCOPE_EXIT
    {
        GEngine->DestroyWorldContext(World);
        World->DestroyWorld(false);
    };
    auto* Cube = LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Cube.Cube"));
    const auto Geometry = [World, Cube](const FVector& Position, const FVector& Scale)
    {
        AActor* Actor = World->SpawnActor<AActor>();
        auto* Mesh = NewObject<UStaticMeshComponent>(Actor);
        Actor->SetRootComponent(Mesh);
        Mesh->SetStaticMesh(Cube);
        Mesh->SetCollisionProfileName(TEXT("BlockAll"));
        Mesh->SetMobility(EComponentMobility::Static);
        Mesh->RegisterComponent();
        Actor->SetActorLocation(Position);
        Actor->SetActorScale3D(Scale);
        return Actor;
    };
    Geometry(FVector(0.0f, 0.0f, -50.0f), FVector(80.0f, 80.0f, 1.0f));
    Geometry(FVector(0.0f, 0.0f, 150.0f), FVector(0.4f, 12.0f, 3.0f));
    Geometry(FVector(1800.0f, 1800.0f, 200.0f), FVector(8.0f, 8.0f, 4.0f));
    for (int32 Index = 0; Index < 3; ++Index)
    {
        const float Height = Index == 0 ? 5.0f : Index == 1 ? 15.0f : 30.0f;
        Geometry(FVector(300.0f, -1400.0f - Index * 800.0f, Height * 0.5f), FVector(10.0f, 4.0f, Height / 100.0f));
    }
    auto* Bounds = World->SpawnActor<ANavMeshBoundsVolume>();
    auto* BoundsBox = NewObject<UBoxComponent>(Bounds);
    BoundsBox->SetupAttachment(Bounds->GetRootComponent());
    BoundsBox->SetBoxExtent(FVector(3900.0f, 3900.0f, 500.0f));
    BoundsBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    BoundsBox->SetCanEverAffectNavigation(false);
    BoundsBox->RegisterComponent();
    auto* Nav = FNavigationSystem::GetCurrent<UNavigationSystemV1>(World);
    if (!TestNotNull(TEXT("真实 UE 导航系统"), Nav))
    {
        return false;
    }
    Nav->OnNavigationBoundsUpdated(Bounds);
    Nav->OnWorldInitDone(FNavigationSystemRunMode::GameMode);
    Nav->Build();
    auto* Mesh = Cast<ARecastNavMesh>(Nav->GetDefaultNavDataInstance(FNavigationSystem::Create));
    if (!TestNotNull(TEXT("真实 Recast 导航网格"), Mesh))
    {
        return false;
    }
    Mesh->EnsureBuildCompletion();
    FNavLocation Projected;
    if (!TestTrue(TEXT("地面已产生有效导航"), Nav->ProjectPointToNavigation(FVector(-1800.0f, 0.0f, 0.0f), Projected)))
    {
        return false;
    }
    auto& Manager = World->GetSubsystem<UMassEntitySubsystem>()->GetMutableEntityManager();
    const auto Type = Manager.CreateArchetype({FTransformFragment::StaticStruct(), FMassVelocityFragment::StaticStruct(),
        FBBBMonsterBehaviorFragment::StaticStruct(), FBBBMonsterMovementFragment::StaticStruct(), FBBBMonsterNavigationFragment::StaticStruct(),
        FBBBMonsterGroundFragment::StaticStruct(), FBBBMonsterMobilityFragment::StaticStruct(), FBBBMonsterAvoidanceFragment::StaticStruct(),
        FBBBMonsterNetworkFragment::StaticStruct(), FBBBMonsterTargetFragment::StaticStruct(), FBBBMonsterTag::StaticStruct()});
    auto* Settings = NewObject<UBBBMonsterDefinition>(World);
    auto* Navigation = NewObject<UBBBMonsterNavigationProcessor>(World);
    auto* Movement = NewObject<UBBBMonsterLocomotionProcessor>(World);
    auto* Avoidance = NewObject<UBBBMonsterAvoidanceProcessor>(World);
    for (UMassProcessor* Processor : TArray<UMassProcessor*>{Navigation, Movement, Avoidance})
    {
        Processor->CallInitialize(World, Manager.AsShared());
    }
    const auto Run = [&Manager](UMassProcessor* Processor)
    {
        UE::Mass::FProcessingContext Context(Manager, 0.033333f);
        UE::Mass::Executor::Run(*Processor, Context);
    };
    const auto Create = [&Manager, Type, Settings](const FVector& Position)
    {
        const auto Entity = Manager.CreateEntity(Type);
        Manager.GetFragmentDataChecked<FTransformFragment>(Entity).GetMutableTransform().SetLocation(Position);
        Manager.GetFragmentDataChecked<FBBBMonsterNetworkFragment>(Entity).Definition = Settings;
        auto& State = Manager.GetFragmentDataChecked<FBBBMonsterBehaviorFragment>(Entity);
        State.State = EBBBMonsterBehavior::Chase;
        State.ActionId = 1;
        State.StateEndsAtTime = 0.0f;
        Manager.GetFragmentDataChecked<FBBBMonsterGroundFragment>(Entity).bGrounded = true;
        auto& Target = Manager.GetFragmentDataChecked<FBBBMonsterTargetFragment>(Entity);
        Target.bHasTarget = true;
        Target.TargetLocation = FVector(1800.0f, 0.0f, 90.0f);
        return Entity;
    };
    TArray<FMassEntityHandle> StepWalkers;
    for (int32 Index = 0; Index < 3; ++Index)
    {
        const float Height = Index == 0 ? 5.0f : Index == 1 ? 15.0f : 30.0f;
        const float Y = -1400.0f - Index * 800.0f;
        const auto Walker = Create(FVector(-600.0f, Y, 90.5f));
        Manager.GetFragmentDataChecked<FBBBMonsterTargetFragment>(Walker).TargetLocation = FVector(650.0f, Y, 90.5f + Height);
        StepWalkers.Add(Walker);
    }
    Run(Navigation);
    for (const auto Walker : StepWalkers)
    {
        const auto& Route = Manager.GetFragmentDataChecked<FBBBMonsterNavigationFragment>(Walker);
        TestTrue(TEXT("小台阶生成直接完整路线 不绕开平台"), Route.bHasPath && !Route.bPartialPath &&
            Route.TailDistances.Num() > 0 && Route.TailDistances[0] < 1450.0f);
    }
    for (int32 Frame = 0; Frame < 420; ++Frame)
    {
        World->TimeSeconds += 1.0 / 30.0;
        Run(Navigation);
        Run(Avoidance);
        Run(Movement);
    }
    for (int32 Index = 0; Index < StepWalkers.Num(); ++Index)
    {
        const auto Walker = StepWalkers[Index];
        const FVector Position = Manager.GetFragmentDataChecked<FTransformFragment>(Walker).GetTransform().GetLocation();
        const float Height = Index == 0 ? 5.0f : Index == 1 ? 15.0f : 30.0f;
        TestTrue(FString::Printf(TEXT("真实导航与物理联合跨越%.0f厘米台阶"), Height), Position.X > 500.0f &&
            FMath::IsNearlyEqual(Position.Y, -1400.0f - Index * 800.0f, 40.0f) && FMath::IsNearlyEqual(Position.Z, 90.5f + Height, 2.0f));
        Manager.DestroyEntity(Walker);
    }
    World->TimeSeconds = 0.0;
    const auto Entity = Create(FVector(-1800.0f, 0.0f, 90.0f));
    Run(Navigation);
    auto& Path = Manager.GetFragmentDataChecked<FBBBMonsterNavigationFragment>(Entity);
    TestTrue(TEXT("绕墙完整路径成立"), Path.bHasPath && !Path.bPartialPath && Path.PathPoints.Num() >= 3);
    const auto Outside = Create(FVector(2000.0f, -1000.0f, 90.0f));
    auto& OutsideTarget = Manager.GetFragmentDataChecked<FBBBMonsterTargetFragment>(Outside);
    OutsideTarget.TargetLocation = FVector(4500.0f, -1000.0f, 90.0f);
    Run(Navigation);
    const auto& OutsidePath = Manager.GetFragmentDataChecked<FBBBMonsterNavigationFragment>(Outside);
    TestTrue(TEXT("网格外终点允许有实际推进的部分路径"), OutsidePath.bHasPath && OutsidePath.bPartialPath && OutsidePath.Destination.X < 4000.0f);
    Manager.DestroyEntity(Outside);

    const auto Platform = Create(FVector(800.0f, 1800.0f, 90.0f));
    Manager.GetFragmentDataChecked<FBBBMonsterTargetFragment>(Platform).TargetLocation = FVector(1800.0f, 1800.0f, 490.0f);
    Run(Navigation);
    auto& PlatformPath = Manager.GetFragmentDataChecked<FBBBMonsterNavigationFragment>(Platform);
    TestTrue(TEXT("高台不可直达时只走有推进的部分路径"), PlatformPath.bHasPath && PlatformPath.bPartialPath);
    if (PlatformPath.bHasPath)
    {
        Manager.GetFragmentDataChecked<FTransformFragment>(Platform).GetMutableTransform().SetLocation(
            FVector(PlatformPath.Destination.X, PlatformPath.Destination.Y, 90.0f));
        Run(Navigation);
        TestFalse(TEXT("部分路径边界不误报调查完成"), PlatformPath.bReachedDestination);
        for (int32 Retry = 0; Retry < 5; ++Retry)
        {
            World->TimeSeconds += 0.6;
            Run(Navigation);
        }
        TestTrue(TEXT("高台边界有限重试后排除而非永久拦停"), PlatformPath.ExcludedUntil > World->GetTimeSeconds() && !PlatformPath.bHasPath);
    }
    Manager.DestroyEntity(Platform);
    Path = FBBBMonsterNavigationFragment{};
    Run(Navigation);
    auto& Injury = Manager.GetFragmentDataChecked<FBBBMonsterMobilityFragment>(Entity);
    Injury.StaggerEndsAt = 10.0f;
    Path.ProgressTime = -10.0f;
    Run(Navigation);
    TestEqual(TEXT("踉跄不计为卡死"), Path.FailureCount, 0);
    Injury.StaggerEndsAt = 0.0f;
    World->TimeSeconds = 5.0;
    Path.ProgressTime = 1.0f;
    Path.ProgressPosition = FVector(-1800.0f, 0.0f, 90.0f);
    Run(Navigation);
    TestEqual(TEXT("无进展首先重算路径"), Path.FailureCount, 1);
    World->TimeSeconds = 10.0;
    Path.ProgressTime = 6.0f;
    Run(Navigation);
    TestTrue(TEXT("第二次失败尝试有限侧向绕行"), Path.FailureCount == 2 && Path.DetourEndsAt > World->GetTimeSeconds());
    World->TimeSeconds = 15.0;
    Path.ProgressTime = 11.0f;
    Run(Navigation);
    TestTrue(TEXT("持续失败暂时排除目标位置"), Path.ExcludedUntil > World->GetTimeSeconds() && !Path.bHasPath);
    Manager.DestroyEntity(Entity);

    AActor* Player = World->SpawnActor<AActor>();
    Player->SetActorLocation(FVector(1800.0f, 0.0f, 90.0f));
    TArray<FMassEntityHandle> Ring;
    for (int32 Index = 0; Index < 10; ++Index)
    {
        const float Angle = Index * UE_TWO_PI / 10.0f;
        const auto Monster = Create(FVector(1800.0f, 0.0f, 90.0f) + FVector(FMath::Cos(Angle), FMath::Sin(Angle), 0.0f) * 450.0f);
        auto& Target = Manager.GetFragmentDataChecked<FBBBMonsterTargetFragment>(Monster);
        Target.bTargetVisible = true;
        Target.TargetActor = Player;
        Ring.Add(Monster);
    }
    Run(Navigation);
    TSet<int32> Slots;
    int32 Waiting = 0;
    for (const auto Monster : Ring)
    {
        const auto& Route = Manager.GetFragmentDataChecked<FBBBMonsterNavigationFragment>(Monster);
        if (Route.ApproachSlot != INDEX_NONE)
        {
            TestFalse(TEXT("包围位置不重复分配"), Slots.Contains(Route.ApproachSlot));
            Slots.Add(Route.ApproachSlot);
        }
        Waiting += Route.bWaitingForSpace;
        Manager.DestroyEntity(Monster);
    }
    TestTrue(TEXT("近距离分散到多个接近方向"), Slots.Num() >= 4);
    TestTrue(TEXT("接近位置满时合理等待"), Waiting >= 2);

    TArray<FMassEntityHandle> WallCrowd;
    const FVector WallCenter(160.0f, 0.0f, 90.0f);
    for (int32 Index = 0; Index < 10; ++Index)
    {
        const float Angle = (Index / 9.0f - 0.5f) * UE_PI;
        const auto Monster = Create(WallCenter + FVector(FMath::Cos(Angle), FMath::Sin(Angle), 0.0f) * 450.0f);
        auto& Target = Manager.GetFragmentDataChecked<FBBBMonsterTargetFragment>(Monster);
        Target.TargetLocation = WallCenter;
        Target.bTargetVisible = true;
        Target.TargetActor = Player;
        WallCrowd.Add(Monster);
    }
    Run(Navigation);
    int32 WallSlots = 0;
    int32 WallWaiting = 0;
    for (const auto Monster : WallCrowd)
    {
        const auto& Route = Manager.GetFragmentDataChecked<FBBBMonsterNavigationFragment>(Monster);
        WallSlots += Route.ApproachSlot != INDEX_NONE;
        WallWaiting += Route.bWaitingForSpace;
        TestEqual(TEXT("墙边有效位置占满不计为寻路失败"), Route.FailureCount, 0);
        Manager.DestroyEntity(Monster);
    }
    TestTrue(TEXT("墙边只有部分接近位置可用"), WallSlots > 0 && WallSlots < 8);
    TestTrue(TEXT("墙边可用位置占满后合理等待"), WallWaiting == 10 - WallSlots);

    TArray<FMassEntityHandle> Crowd;
    for (int32 Index = 0; Index < 50; ++Index)
    {
        Crowd.Add(Create(FVector(-1800.0f - Index / 10 * 120.0f, (Index % 10 - 5) * 120.0f, 90.0f)));
    }
    const double Started = FPlatformTime::Seconds();
    for (int32 Frame = 0; Frame < 240; ++Frame)
    {
        World->TimeSeconds += 1.0 / 30.0;
        Run(Navigation);
        Run(Avoidance);
        Run(Movement);
    }
    const double Elapsed = FPlatformTime::Seconds() - Started;
    int32 Progressed = 0;
    for (const auto Monster : Crowd)
    {
        const FVector Position = Manager.GetFragmentDataChecked<FTransformFragment>(Monster).GetTransform().GetLocation();
        TestFalse(TEXT("五十只移动坐标保持有限"), Position.ContainsNaN());
        TestTrue(TEXT("批量移动没有下沉"), Position.Z >= 85.0f);
        Progressed += Position.X > -1600.0f;
        Manager.DestroyEntity(Monster);
    }
    TestTrue(TEXT("五十只真实路径移动都有进展"), Progressed == 50);
    AddInfo(FString::Printf(TEXT("[BBBNavigation50] Frames=240 Count=50 TotalMs=%.3f MeanMs=%.3f"), Elapsed * 1000.0, Elapsed * 1000.0 / 240.0));
    return true;
}
#endif
