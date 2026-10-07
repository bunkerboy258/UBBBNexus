#include "Misc/AutomationTest.h"

#if WITH_DEV_AUTOMATION_TESTS && WITH_EDITOR
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "Components/BoxComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/DamageEvents.h"
#include "Engine/Engine.h"
#include "Engine/LocalPlayer.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"

/** 在真实游戏帧中检查十六个角色同时倒地和死亡的骨骼物理稳定性 */
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBBBCharacterPhysicalCapacityTest, "BBB.Character.PhysicalCapacity16",
                                 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FBBBCharacterPhysicalCapacityTest::RunTest(const FString &Parameters)
{
    const auto Initialization = UWorld::InitializationValues()
                                    .AllowAudioPlayback(false)
                                    .CreatePhysicsScene(true)
                                    .CreateNavigation(false)
                                    .CreateAISystem(false);
    UWorld *World =
        UWorld::CreateWorld(EWorldType::Game, false, NAME_None, nullptr, true, ERHIFeatureLevel::Num, &Initialization);
    if (!TestNotNull(TEXT("物理基线世界"), World))
    {
        return false;
    }
    GEngine->CreateNewWorldContext(EWorldType::Game).SetCurrentWorld(World);
    const auto Cleanup = [World]()
    {
        World->EndPlay(EEndPlayReason::Quit);
        GEngine->DestroyWorldContext(World);
        World->DestroyWorld(false);
    };
    UClass *CharacterClass =
        LoadClass<ABBBCharacter>(nullptr, TEXT("/Game/_Project/Characters/BBBC_UA/BBBC_UA_0.BBBC_UA_0_C"));
    if (!TestNotNull(TEXT("生产角色蓝图"), CharacterClass))
    {
        Cleanup();
        return false;
    }
    AActor *Floor = World->SpawnActor<AActor>();
    UBoxComponent *Ground = NewObject<UBoxComponent>(Floor);
    Floor->SetRootComponent(Ground);
    Ground->SetBoxExtent(FVector(10000.0f, 10000.0f, 10.0f));
    Ground->SetCollisionProfileName(TEXT("BlockAll"));
    Ground->RegisterComponent();
    Floor->SetActorLocation(FVector(0.0f, 0.0f, -10.0f));
    TArray<ABBBCharacter *> Characters;
    for (int32 Index = 0; Index < 16; ++Index)
    {
        APlayerController *Controller = World->SpawnActor<APlayerController>();
        ABBBCharacter *Character = World->SpawnActor<ABBBCharacter>(
            CharacterClass, FVector((Index % 4) * 250.0f, (Index / 4) * 250.0f, 100.0f), FRotator::ZeroRotator);
        if (!TestNotNull(TEXT("物理角色"), Character) || !TestNotNull(TEXT("本地控制者"), Controller))
        {
            Cleanup();
            return false;
        }
        ULocalPlayer *LocalPlayer = NewObject<ULocalPlayer>(GEngine);
        LocalPlayer->PlayerController = Controller;
        Controller->Player = LocalPlayer;
        Controller->Possess(Character);
        Characters.Add(Character);
    }
    World->InitializeActorsForPlay(FURL());
    World->SetBegunPlay(true);
    for (ABBBCharacter *Character : Characters)
    {
        Character->DispatchBeginPlay();
        FHitResult Hit;
        Hit.BoneName = TEXT("spine_02");
        Hit.ImpactPoint = Character->GetActorLocation();
        FPointDamageEvent Damage(500.0f, Hit, FVector::ForwardVector, nullptr);
        Character->TakeDamage(500.0f, Damage, Character->GetController(), Character);
    }
    AddCommand(new FFunctionLatentCommand(
        [this, World, Characters, Cleanup, Frame = 0, LastFrame = GFrameCounter, TickCost = 0.0]() mutable
        {
            if (LastFrame == GFrameCounter)
            {
                return false;
            }
            LastFrame = GFrameCounter;
            const double Start = FPlatformTime::Seconds();
            World->Tick(LEVELTICK_All, 1.0f / 60.0f);
            TickCost += FPlatformTime::Seconds() - Start;
            ++Frame;
            if (Frame == 1)
            {
                for (ABBBCharacter *Character : Characters)
                {
                    TestTrue(TEXT("十六个角色均能进入倒地"),
                             Character->GetLifePhase() == EBBBCharacterLifePhase::Downed);
                    TestEqual(TEXT("倒地生命独立成立"), Character->GetHealth(), 300.0f);
                }
            }
            if (Frame == 30)
            {
                for (ABBBCharacter *Character : Characters)
                {
                    FHitResult Hit;
                    Hit.BoneName = TEXT("spine_02");
                    Hit.ImpactPoint = Character->GetActorLocation();
                    FPointDamageEvent Damage(300.0f, Hit, FVector::ForwardVector, nullptr);
                    Character->TakeDamage(300.0f, Damage, Character->GetController(), Character);
                }
            }
            if (Frame < 60)
            {
                return false;
            }
            for (ABBBCharacter *Character : Characters)
            {
                TestTrue(TEXT("十六个角色均能移交死亡物理"),
                         Character->GetLifePhase() == EBBBCharacterLifePhase::Dead &&
                             Character->GetMesh()->IsSimulatingPhysics());
                TestFalse(TEXT("尸体骨骼位置保持有限"),
                          Character->GetMesh()->GetSocketLocation(TEXT("pelvis")).ContainsNaN());
            }
            AddInfo(FString::Printf(TEXT("十六角色骨骼物理基线六十帧平均世界更新 %.3f ms"), TickCost * 1000.0 / Frame));
            Cleanup();
            return true;
        }));
    return true;
}
#endif
