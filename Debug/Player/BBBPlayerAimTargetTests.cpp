#include "CoreMinimal.h"

#if WITH_DEV_AUTOMATION_TESTS && WITH_EDITOR

#include "Misc/AutomationTest.h"
#include "Misc/ScopeExit.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "Components/BoxComponent.h"
#include "MassCommonFragments.h"
#include "BBBWork/UBBBNexus/Player/Input/BBBPlayerInputSystem.h"
#include "BBBWork/UBBBNexus/Mass/Core/BBBMassSubsystem.h"

/** 验证准星目标选择 无表现实体与世界遮挡 不改变枪口发射规则 */
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBBBPlayerAimTargetTest, "UBBB.PlayerInput.AimTarget",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FBBBPlayerAimTargetTest::RunTest(const FString& Parameters)
{
    const auto Initialization = UWorld::InitializationValues()
        .AllowAudioPlayback(false).CreatePhysicsScene(true).CreateNavigation(false)
        .CreateAISystem(false).ShouldSimulatePhysics(false);
    UWorld* World = UWorld::CreateWorld(EWorldType::Game, false, NAME_None, nullptr, true,
        ERHIFeatureLevel::Num, &Initialization);
    if (!TestNotNull(TEXT("隔离瞄准世界"), World))
    {
        return false;
    }

    GEngine->CreateNewWorldContext(EWorldType::Game).SetCurrentWorld(World);
    ON_SCOPE_EXIT
    {
        GEngine->DestroyWorldContext(World);
        World->DestroyWorld(false);
        World->GetOutermost()->SetDirtyFlag(false);
    };

    AActor* Owner = World->SpawnActor<AActor>();
    UBBBPlayerInputSystem* Input = NewObject<UBBBPlayerInputSystem>(Owner);
    Input->RegisterComponent();
    Input->AimTargetDistance = 1000.0f;
    const FVector Start(0.0, 50.0, 153.0);
    const FVector Direction = FVector::ForwardVector;
    UBBBMassSubsystem* Mass = World->GetSubsystem<UBBBMassSubsystem>();
    if (!TestNotNull(TEXT("公开实体碰撞查询"), Mass))
    {
        return false;
    }

    TestTrue(TEXT("空射线使用远端目标"),
        Input->ResolveAimTargetWorld(Start, Direction).Equals(FVector(1000.0, 50.0, 153.0), 0.01));
    FMassEntityManager& Manager = World->GetSubsystem<UMassEntitySubsystem>()->GetMutableEntityManager();
    const FMassArchetypeHandle Archetype = Manager.CreateArchetype({FTransformFragment::StaticStruct()});
    const FMassEntityHandle Entity = Manager.CreateEntity(Archetype);
    FBBBMassCollisionBody Body;
    Body.Entity = Entity;
    Body.Center = FVector(500.0, 50.0, 153.0);
    Body.Radius = 14.0f;
    Mass->AddCollisionBody(Body);
    const FVector NearTarget(486.0, 50.0, 153.0);
    TestTrue(TEXT("没有表现 Actor 的实体也提供近距离目标"),
        Input->ResolveAimTargetWorld(Start, Direction).Equals(NearTarget, 0.01));

    AActor* Wall = World->SpawnActor<AActor>();
    UBoxComponent* Box = NewObject<UBoxComponent>(Wall);
    Wall->SetRootComponent(Box);
    Box->SetBoxExtent(FVector(10.0, 100.0, 100.0));
    Box->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    Box->SetCollisionResponseToAllChannels(ECR_Ignore);
    Box->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
    Box->RegisterComponent();
    Wall->SetActorLocation(FVector(300.0, 50.0, 153.0));
    TestTrue(TEXT("实体之前的墙面优先 不向墙后目标瞄准"),
        Input->ResolveAimTargetWorld(Start, Direction).Equals(FVector(290.0, 50.0, 153.0), 0.01));
    Wall->SetActorLocation(FVector(700.0, 50.0, 153.0));
    TestTrue(TEXT("实体之后的墙面不覆盖近距离目标"),
        Input->ResolveAimTargetWorld(Start, Direction).Equals(NearTarget, 0.01));

    UBoxComponent* OwnerBox = NewObject<UBoxComponent>(Owner);
    Owner->SetRootComponent(OwnerBox);
    OwnerBox->SetBoxExtent(FVector(30.0, 100.0, 100.0));
    OwnerBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    OwnerBox->SetCollisionResponseToAllChannels(ECR_Block);
    OwnerBox->RegisterComponent();
    Owner->SetActorLocation(Start);
    TestTrue(TEXT("输入持有者不遮挡自身瞄准"),
        Input->ResolveAimTargetWorld(Start, Direction).Equals(NearTarget, 0.01));
    Mass->BeginCollisionFrame();
    TestTrue(TEXT("实体离开后不保留历史瞄准点"),
        Input->ResolveAimTargetWorld(Start, Direction).Equals(FVector(690.0, 50.0, 153.0), 0.01));
    TestTrue(TEXT("零方向不生成无效目标"), Input->ResolveAimTargetWorld(Start, FVector::ZeroVector).Equals(Start));
    return true;
}

#endif
