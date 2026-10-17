#if WITH_DEV_AUTOMATION_TESTS && WITH_EDITOR
#include "Misc/AutomationTest.h"
#include "Misc/ScopeExit.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Projectile/BBBThrowable.h"

/** 正式手雷配置的投程 弹跳和薄墙连续碰撞验收 */
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBBBGrenadePresentationTest, "BBB.Item.Throwable.GrenadePresentation",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FBBBGrenadePresentationTest::RunTest(const FString&)
{
    auto* Definition = LoadObject<UBBBThrowableDefinition>(nullptr,
        TEXT("/Game/_Project/Characters/BBBC_UA/Items/Consumable/Grenade/Grenade_01/DA_Throwable_Grenade_01.DA_Throwable_Grenade_01"));
    if (!TestTrue(TEXT("正式手雷配置有效"), Definition && Definition->IsValid()))
    {
        return false;
    }
    const auto Options = UWorld::InitializationValues().AllowAudioPlayback(false).CreatePhysicsScene(true)
        .CreateNavigation(false).CreateAISystem(false).ShouldSimulatePhysics(true);
    auto* World = UWorld::CreateWorld(EWorldType::Game, false, NAME_None, nullptr, true, ERHIFeatureLevel::Num, &Options);
    if (!TestNotNull(TEXT("正式手雷验收物理世界"), World))
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
    const auto Box = [World](const FVector& Location, const FVector& Extent)
    {
        auto* Actor = World->SpawnActor<AActor>();
        auto* Component = NewObject<UBoxComponent>(Actor);
        Actor->SetRootComponent(Component);
        Actor->AddInstanceComponent(Component);
        Component->SetBoxExtent(Extent);
        Component->SetCollisionProfileName(TEXT("BlockAll"));
        Component->RegisterComponent();
        Actor->SetActorLocation(Location);
    };
    Box(FVector(0, 0, -10), FVector(20000, 20000, 10));
    Box(FVector(500, 6000, 500), FVector(2, 500, 500));
    World->InitializeActorsForPlay(FURL());
    World->SetBegunPlay(true);
    TArray<ABBBThrowable*> Actors;
    TArray<USphereComponent*> Bodies;
    for (int32 Index = 0; Index < 4; ++Index)
    {
        const float Pitch = Index == 1 ? 45.0f : Index == 2 ? -45.0f : 12.0f;
        const FTransform Transform(FRotator(Pitch, 0, 0), FVector(0, Index * 2000, 150));
        auto* Actor = World->SpawnActorDeferred<ABBBThrowable>(ABBBThrowable::StaticClass(), Transform);
        FBBBThrowableLaunchLocalControlPacket Packet;
        Packet.Definition = Definition;
        Packet.Transform = Transform;
        Packet.InheritedVelocity = Index == 0 ? FVector(600, 0, 0) : FVector::ZeroVector;
        Actor->SubmitInput(Packet);
        Actor->FinishSpawning(Transform);
        Actor->Tick(0);
        Actors.Add(Actor);
        Bodies.Add(Actor->FindComponentByClass<USphereComponent>());
    }
    bool bDropped = false;
    bool bBounced = false;
    bool bWallCrossed = false;
    bool bBelowFloor = false;
    double MaximumSpeed = 0;
    FVector PositionAtOneSecond = FVector::ZeroVector;
    for (int32 Frame = 0; Frame < 144; ++Frame)
    {
        ++GFrameCounter;
        World->Tick(LEVELTICK_All, 1.0f / 60.0f);
        const double Vertical = Bodies[2]->GetPhysicsLinearVelocity().Z;
        bDropped |= Vertical < -100;
        bBounced |= bDropped && Vertical > 80;
        bWallCrossed |= Actors[3]->GetActorLocation().X > 502;
        for (int32 Index = 0; Index < Actors.Num(); ++Index)
        {
            bBelowFloor |= Actors[Index]->GetActorLocation().Z < -1;
            MaximumSpeed = FMath::Max(MaximumSpeed, Bodies[Index]->GetPhysicsLinearVelocity().Size());
        }
        if (Frame == 59)
        {
            PositionAtOneSecond = Actors[0]->GetActorLocation();
        }
    }
    TestTrue(TEXT("移动平投一秒达到合理距离"), PositionAtOneSecond.X > 1200 && PositionAtOneSecond.X < 2400);
    TestTrue(TEXT("仰投具有二十五米以上投程"), Actors[1]->GetActorLocation().X > 2500);
    TestTrue(TEXT("俯投发生可见的衰减反弹"), bBounced);
    TestFalse(TEXT("连续碰撞没有穿过四厘米薄墙"), bWallCrossed);
    TestFalse(TEXT("地面碰撞没有下穿"), bBelowFloor);
    TestTrue(TEXT("碰撞未产生异常加速"), MaximumSpeed < 3000);
    AddInfo(FString::Printf(TEXT("移动平投一秒=%.1fcm 仰投二点四秒=%.1fcm 最大速度=%.1fcm/s"),
        PositionAtOneSecond.X, Actors[1]->GetActorLocation().X, MaximumSpeed));
    return !HasAnyErrors();
}
#endif
