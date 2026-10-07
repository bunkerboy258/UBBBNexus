#include "Misc/AutomationTest.h"

#if WITH_DEV_AUTOMATION_TESTS && WITH_EDITOR
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "BBBWork/UBBBNexus/Character/Input/AuthorityFact/Life/FBBBCharacterLifeAuthorityFactPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/LocalControl/Locomotion/FBBBJumpLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/LocalControl/Item/FBBBItemSelectLocalControlPacket.h"
#include "Engine/Engine.h"
#include "Engine/LocalPlayer.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/BoxComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/DamageEvents.h"
#include "EngineUtils.h"
#include "Misc/ScopeExit.h"

/** 经真实角色主管线验证生命与动作限制 */
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBBBCharacterLifeTest, "BBB.Character.Life",
                                 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FBBBCharacterLifeTest::RunTest(const FString &Parameters)
{
    const auto Initialization = UWorld::InitializationValues()
                                    .AllowAudioPlayback(false)
                                    .CreatePhysicsScene(true)
                                    .CreateNavigation(false)
                                    .CreateAISystem(false);
    UWorld *World =
        UWorld::CreateWorld(EWorldType::Game, false, NAME_None, nullptr, true, ERHIFeatureLevel::Num, &Initialization);
    if (!TestNotNull(TEXT("隔离角色世界"), World))
    {
        return false;
    }
    GEngine->CreateNewWorldContext(EWorldType::Game).SetCurrentWorld(World);
    ON_SCOPE_EXIT
    {
        World->EndPlay(EEndPlayReason::Quit);
        GEngine->DestroyWorldContext(World);
        World->DestroyWorld(false);
    };
    UClass *CharacterClass =
        LoadClass<ABBBCharacter>(nullptr, TEXT("/Game/_Project/Characters/BBBC_UA/BBBC_UA_0.BBBC_UA_0_C"));
    if (!TestNotNull(TEXT("生产角色蓝图"), CharacterClass))
    {
        return false;
    }
    AActor *Floor = World->SpawnActor<AActor>();
    UBoxComponent *Ground = NewObject<UBoxComponent>(Floor);
    Floor->SetRootComponent(Ground);
    Ground->SetBoxExtent(FVector(10000.0f, 10000.0f, 10.0f));
    Ground->SetCollisionProfileName(TEXT("BlockAll"));
    Ground->RegisterComponent();
    Floor->SetActorLocation(FVector(0.0f, 0.0f, -10.0f));
    APlayerController *Controller = World->SpawnActor<APlayerController>();
    ABBBCharacter *Character =
        World->SpawnActor<ABBBCharacter>(CharacterClass, FVector(0.0f, 0.0f, 100.0f), FRotator::ZeroRotator);
    if (!TestNotNull(TEXT("角色"), Character) || !TestNotNull(TEXT("控制者"), Controller))
    {
        return false;
    }
    ULocalPlayer *LocalPlayer = NewObject<ULocalPlayer>(GEngine);
    LocalPlayer->PlayerController = Controller;
    Controller->Player = LocalPlayer;
    Controller->Possess(Character);
    TestTrue(TEXT("因果角色具备真实本地玩家身份"), Character->IsLocallyControlled());
    World->InitializeActorsForPlay(FURL());
    World->SetBegunPlay(true);
    Controller->DispatchBeginPlay();
    Character->DispatchBeginPlay();
    const auto Step = [&]()
    {
        for (TActorIterator<ABBBCharacter> Iterator(World); Iterator; ++Iterator)
        {
            Iterator->Tick(0.1f);
            Iterator->GetCharacterMovement()->TickComponent(0.1f, LEVELTICK_All, nullptr);
        }
        World->Tick(LEVELTICK_All, 0.1f);
    };
    const auto Damage = [&](float Amount)
    {
        Character->SubmitInput(FBBBCharacterDamageLocalControlPacket{
            {Amount}, {TEXT("spine_03")}, {Character->GetActorLocation()}, {FVector::ForwardVector}, {nullptr}});
    };
    Step();
    TestEqual(TEXT("初始最大生命"), Character->GetHealth(), 500.0f);
    TestTrue(TEXT("出生为正常状态"), Character->GetLifePhase() == EBBBCharacterLifePhase::Alive);

    Character->SubmitInput(FBBBItemAddLocalControlPacket{{TEXT("ModernWeapons_Rifle_01")}});
    Step();
    Character->SubmitInput(FBBBItemSelectLocalControlPacket{{0}});
    Step();
    ABBBEquipment *SelectedEquipment = Character->GetActiveEquipment();
    TestNotNull(TEXT("真实装备已被选择"), SelectedEquipment);

    APlayerController *TeammateController = World->SpawnActor<APlayerController>();
    APawn *Teammate = World->SpawnActor<APawn>();
    TeammateController->Possess(Teammate);
    FHitResult Hit;
    Hit.BoneName = TEXT("spine_02");
    Hit.ImpactPoint = Character->GetActorLocation();
    FPointDamageEvent FriendlyHit(25.0f, Hit, FVector::ForwardVector, nullptr);
    Character->TakeDamage(25.0f, FriendlyHit, TeammateController, Teammate);
    Step();
    TestEqual(TEXT("队友引擎命中适配器正常扣血"), Character->GetHealth(), 475.0f);

    Character->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
    Character->Crouch();
    Character->GetCharacterMovement()->Crouch();
    TestTrue(TEXT("致倒地前确实处于蹲伏"), Character->bIsCrouched);
    AActor *Ceiling = World->SpawnActor<AActor>();
    UBoxComponent *Overhead = NewObject<UBoxComponent>(Ceiling);
    Ceiling->SetRootComponent(Overhead);
    Overhead->SetBoxExtent(FVector(200.0f, 200.0f, 10.0f));
    Overhead->SetCollisionProfileName(TEXT("BlockAll"));
    Overhead->RegisterComponent();
    Ceiling->SetActorLocation(FVector(0.0f, 0.0f, 140.0f));

    Damage(600.0f);
    Damage(50.0f);
    Step();
    TestTrue(TEXT("致倒地命中不溢出"), Character->GetLifePhase() == EBBBCharacterLifePhase::Downed);
    TestEqual(TEXT("同帧后续独立命中仍扣倒地生命"), Character->GetHealth(), 250.0f);
    TestFalse(TEXT("倒地收起装备"), Character->IsEquipmentUsable());
    TestTrue(TEXT("倒地保留原装备实例和绑定"), Character->GetActiveEquipment() == SelectedEquipment);
    Character->SubmitInput(FBBBItemSelectLocalControlPacket{{INDEX_NONE}});
    const float Yaw = Character->GetActorRotation().Yaw;
    FBBBCharacterMovementLocalControlPacket Move;
    Move.MoveWorld = FVector(1, 1, 0);
    Move.FacingWorld = FRotator(0, Yaw + 90.0f, 0);
    Character->SubmitInput(Move);
    Character->SubmitInput(FBBBJumpLocalControlPacket{});
    Character->SubmitInput(FBBBRunLocalControlPacket{true});
    Character->SubmitInput(FBBBCrouchLocalControlPacket{true});
    Step();
    TestEqual(TEXT("没有自动回血或自动流血"), Character->GetHealth(), 250.0f);
    TestTrue(TEXT("斜向速度不超过六十"), Character->GetVelocity().Size2D() <= 60.1f);
    TestTrue(TEXT("倒地确实能够缓慢移动"), Character->GetVelocity().Size2D() > 1.0f);
    TestTrue(TEXT("身体转向不超过每秒九十度"),
             FMath::Abs(FMath::FindDeltaAngleDegrees(Yaw, Character->GetActorRotation().Yaw)) <= 9.1f);
    TestFalse(TEXT("倒地不能奔跑"), Character->RuntimeData.Locomotion.ReadLocomotionState().bRun);
    TestFalse(TEXT("倒地不能蹲伏"), Character->bIsCrouched);
    TestEqual(TEXT("低顶蹲伏倒地后应用独立胶囊"), Character->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight(),
              40.0f);
    TestEqual(TEXT("低顶倒地保持正确网格底部偏移"), static_cast<float>(Character->GetMesh()->GetRelativeLocation().Z),
              -40.0f);
    TestTrue(TEXT("倒地拒绝选择变化并保留原选择"), Character->GetActiveEquipment() == SelectedEquipment);

    ABBBCharacter *Mirror =
        World->SpawnActor<ABBBCharacter>(CharacterClass, FVector(500.0f, 0.0f, 100.0f), FRotator::ZeroRotator);
    if (!TestNotNull(TEXT("中途加入的角色副本"), Mirror))
    {
        return false;
    }
    Mirror->SubmitInput(FBBBCharacterLifeAuthorityFactPacket{{EBBBCharacterLifePhase::Downed},
                                                             {200.0f},
                                                             {10},
                                                             {5},
                                                             {TEXT("spine_02")},
                                                             {Mirror->GetActorLocation()},
                                                             {FVector::ForwardVector}});
    Step();
    TestEqual(TEXT("首份当前快照还原倒地生命"), Mirror->GetHealth(), 200.0f);
    TestTrue(TEXT("首份当前快照不重演旧受击"),
             Mirror->RuntimeData.Life.ReadHitState().Time < World->GetTimeSeconds() - 0.25);
    Mirror->SubmitInput(FBBBCharacterLifeAuthorityFactPacket{{EBBBCharacterLifePhase::Alive},
                                                             {500.0f},
                                                             {9},
                                                             {4},
                                                             {TEXT("spine_02")},
                                                             {Mirror->GetActorLocation()},
                                                             {FVector::ForwardVector}});
    Step();
    TestEqual(TEXT("迟到结果不能覆盖较新生命"), Mirror->GetHealth(), 200.0f);

    Damage(250.0f);
    Step();
    TestTrue(TEXT("倒地生命清零后死亡"), Character->GetLifePhase() == EBBBCharacterLifePhase::Dead);
    TestEqual(TEXT("死亡生命为零"), Character->GetHealth(), 0.0f);
    TestTrue(TEXT("死亡关闭角色移动"), Character->GetCharacterMovement()->MovementMode == MOVE_None);
    TestTrue(TEXT("死亡移交全身物理表现"),
             Character->RuntimeData.PhysicalPresentation.ReadPhysicalPresentationState().bRagdoll);
    const uint64 Revision = Character->RuntimeData.Life.ReadLifeState().Revision;
    Damage(50.0f);
    Step();
    TestEqual(TEXT("死亡后重复命中不生成新生命结果"), Character->RuntimeData.Life.ReadLifeState().Revision, Revision);
    return true;
}
#endif
