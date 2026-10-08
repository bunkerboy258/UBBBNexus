#include "Misc/AutomationTest.h"

#if WITH_DEV_AUTOMATION_TESTS && WITH_EDITOR
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "BBBWork/UBBBNexus/Character/Input/LocalControl/Life/FBBBCharacterRescueBeginLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/LocalControl/Life/FBBBCharacterRescueCancelLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/LocalControl/Life/FBBBCharacterRescueEndLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/AuthorityFact/Life/FBBBCharacterRescueSnapshotAuthorityFactPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/AuthorityFact/Life/FBBBCharacterLifeAuthorityFactPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/LocalControl/Locomotion/FBBBJumpLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/LocalControl/Item/FBBBItemSelectLocalControlPacket.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "BBBWork/UBBBNexus/Character/Animation/BBBAnimInstance.h"
#include "Engine/Engine.h"
#include "EngineGlobals.h"
#include "Engine/LocalPlayer.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/PlayerState.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Misc/ScopeExit.h"

/** 用隔离世界中的真实角色和输入验证救援闭环 */
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBBBCharacterRescueTest, "BBB.Character.Rescue",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FBBBCharacterRescueTest::RunTest(const FString &Parameters)
{
    const auto Initialization = UWorld::InitializationValues().AllowAudioPlayback(false)
        .CreatePhysicsScene(true).CreateNavigation(false).CreateAISystem(false);
    UWorld *World = UWorld::CreateWorld(EWorldType::Game, false, NAME_None, nullptr, true,
        ERHIFeatureLevel::Num, &Initialization);
    if (!TestNotNull(TEXT("隔离救援世界"), World))
    {
        return false;
    }
    const uint64 OriginalFrame = GFrameCounter;
    GEngine->CreateNewWorldContext(EWorldType::Game).SetCurrentWorld(World);
    ON_SCOPE_EXIT
    {
        World->EndPlay(EEndPlayReason::Quit);
        GEngine->DestroyWorldContext(World);
        World->DestroyWorld(false);
        GFrameCounter = OriginalFrame;
    };
    UClass *CharacterClass = LoadClass<ABBBCharacter>(nullptr,
        TEXT("/Game/_Project/Characters/BBBC_UA/BBBC_UA_0.BBBC_UA_0_C"));
    if (!TestNotNull(TEXT("生产角色类"), CharacterClass))
    {
        return false;
    }
    AActor *Floor = World->SpawnActor<AActor>();
    UBoxComponent *Ground = NewObject<UBoxComponent>(Floor);
    Floor->SetRootComponent(Ground);
    Ground->SetBoxExtent(FVector(10000, 10000, 10));
    Ground->SetCollisionProfileName(TEXT("BlockAll"));
    Ground->RegisterComponent();
    Floor->SetActorLocation(FVector(0, 0, -10));
    TArray<ABBBCharacter *> Characters;
    TArray<APlayerController *> Controllers;
    for (float X : {0.0f, 80.0f, -80.0f})
    {
        ABBBCharacter *Character = World->SpawnActor<ABBBCharacter>(CharacterClass,
            FVector(X, 0, 86), FRotator::ZeroRotator);
        APlayerController *Controller = World->SpawnActor<APlayerController>();
        if (!Character || !Controller)
        {
            AddError(TEXT("角色或本地控制者创建失败"));
            return false;
        }
        ULocalPlayer *Player = NewObject<ULocalPlayer>(GEngine);
        Player->PlayerController = Controller;
        Controller->Player = Player;
        Controller->SetPlayerState(World->SpawnActor<APlayerState>());
        Controller->Possess(Character);
        Characters.Add(Character);
        Controllers.Add(Controller);
    }
    World->InitializeActorsForPlay(FURL());
    World->SetBegunPlay(true);
    for (int32 Index = 0; Index < Characters.Num(); ++Index)
    {
        Controllers[Index]->DispatchBeginPlay();
        Characters[Index]->DispatchBeginPlay();
        Characters[Index]->GetMesh()->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPoseAndRefreshBones;
        Characters[Index]->GetMesh()->bEnableUpdateRateOptimizations = false;
        Characters[Index]->SetActorTickEnabled(false);
        Characters[Index]->GetCharacterMovement()->SetComponentTickEnabled(false);
    }
    ABBBCharacter *Target = Characters[0];
    ABBBCharacter *Helper = Characters[1];
    ABBBCharacter *Competitor = Characters[2];
    const auto Step = [&](float Delta = 0.1f)
    {
        ++GFrameCounter;
        World->Tick(LEVELTICK_All, Delta);
        for (ABBBCharacter *Character : Characters)
        {
            if (IsValid(Character))
            {
                Character->Tick(Delta);
                Character->GetCharacterMovement()->TickComponent(Delta, LEVELTICK_All, nullptr);
            }
        }
    };
    const auto Damage = [&](ABBBCharacter *Character, float Amount)
    {
        Character->SubmitInput(FBBBCharacterDamageLocalControlPacket{{Amount}, {TEXT("spine_03")},
            {Character->GetActorLocation()}, {FVector::ForwardVector}, {nullptr}});
    };
    const auto Begin = [&]()
    {
        Helper->SubmitInput(FBBBCharacterRescueBeginLocalControlPacket{});
        Step();
        Step();
        Step();
    };
    Step();
    Helper->SubmitInput(FBBBItemAddLocalControlPacket{{TEXT("ModernWeapons_Rifle_01")}});
    Helper->SubmitInput(FBBBItemSelectLocalControlPacket{{0}});
    Step();
    ABBBEquipment *Equipment = Helper->GetActiveEquipment();
    TestNotNull(TEXT("救援者原装备"), Equipment);
    Damage(Target, 500);
    Step();
    Step();
    TestEqual(TEXT("倒地独立生命"), Target->GetHealth(), 300.0f);
    if (!TestTrue(TEXT("最近目标来自角色领域"), Helper->GetRescueTarget() == Target))
    {
        const auto &Candidate = Helper->RuntimeData.Life.ReadRescueCandidateState();
        AddInfo(FString::Printf(TEXT("目标 %s 玩家 %d 本地 %d 可恢复 %d 救援者 %s 玩家 %d 本地 %d 合格数 %d"),
            *Target->GetActorLocation().ToString(), Target->IsPlayerControlled(), Target->IsLocallyControlled(),
            Target->CanRecoverFromDowned(), *Helper->GetActorLocation().ToString(), Helper->IsPlayerControlled(),
            Helper->IsLocallyControlled(), Candidate.EligiblePartners.Num()));
        return false;
    }
    Begin();
    TestTrue(TEXT("双方救援关系成立"), Helper->IsRescueHelping() && Target->IsRescueReceiving());
    auto* HelperAnimation = Cast<UBBBAnimInstance>(Helper->GetMesh()->GetAnimInstance());
    if (TestNotNull(TEXT("生产救援动画实例"), HelperAnimation))
    {
        TestTrue(TEXT("延迟动画更新收到帮扶事实"), HelperAnimation->bSourceRescueHelping);
        Helper->GetMesh()->TickAnimation(0.1f, false);
        Helper->GetMesh()->RefreshBoneTransforms();
        const int32 Machine = HelperAnimation->GetStateMachineIndex(TEXT("LocomotionSM"));
        TestTrue(TEXT("生产移动状态机进入帮扶"), Machine != INDEX_NONE &&
            HelperAnimation->GetCurrentStateName(Machine) == TEXT("Rescue"));
    }

    TestFalse(TEXT("帮扶期间装备收起"), Helper->IsEquipmentUsable());
    TestTrue(TEXT("装备实例未重建"), Helper->GetActiveEquipment() == Equipment);
    Helper->SubmitInput(FBBBJumpLocalControlPacket{});
    Helper->SubmitInput(FBBBItemSelectLocalControlPacket{{INDEX_NONE}});
    Target->SubmitInput(FBBBCharacterMovementLocalControlPacket{FVector::ForwardVector, FRotator::ZeroRotator});
    Step();
    TestTrue(TEXT("帮扶期间拒绝物品选择"), Helper->GetActiveEquipment() == Equipment);
    TestFalse(TEXT("帮扶期间拒绝跳跃"), Helper->bPressedJump);
    TestTrue(TEXT("被救者移动输入不取消"), Target->IsRescueReceiving());
    TestTrue(TEXT("被救者暂停爬行"), Target->GetVelocity().IsNearlyZero());
    Helper->SubmitInput(FBBBCharacterRescueCancelLocalControlPacket{});
    Step();
    Step();
    TestFalse(TEXT("松键释放被救者"), Target->IsRescueReceiving());
    TestTrue(TEXT("取消恢复原装备"), Helper->IsEquipmentUsable() && Helper->GetActiveEquipment() == Equipment);
    Target->SubmitInput(FBBBCharacterMovementLocalControlPacket{});
    Begin();
    Damage(Helper, 10);
    Step();
    Step();
    TestFalse(TEXT("救援者受伤立即取消"), Helper->IsRescueHelping() || Target->IsRescueReceiving());
    Begin();
    Target->SubmitInput(FBBBCharacterRescueRequestLocalControlPacket{{Competitor}, {1}, {Target->GetDownedRevision()}});
    Damage(Target, 20);
    Step();
    TestTrue(TEXT("被救者非致命受伤仍继续"), Target->IsRescueReceiving());
    TestTrue(TEXT("第二救援者不能抢占"), Target->GetRescuePartner() == Helper);
    const uint64 OldOperation = Helper->GetRescueOperationId();
    const uint64 OldRound = Target->GetDownedRevision();
    for (int32 Index = 0; Index < 35; ++Index)
    {
        Step();
    }
    TestTrue(TEXT("救援完成恢复正常生命阶段"), Target->GetLifePhase() == EBBBCharacterLifePhase::Alive);
    TestEqual(TEXT("救援恢复生命"), Target->GetHealth(), 100.0f);
    TestFalse(TEXT("成功释放双方"), Helper->IsRescueHelping() || Target->IsRescueReceiving());
    TestTrue(TEXT("成功立即开放操作"), Target->RuntimeData.Life.ReadLifeState().bActionsAllowed);
    TestEqual(TEXT("恢复站立胶囊体"), Target->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight(), 86.0f);
    TestEqual(TEXT("恢复台阶高度"), Target->GetCharacterMovement()->MaxStepHeight, 45.0f);
    TestEqual(TEXT("恢复模拟最小速度"), Target->GetCharacterMovement()->MinAnalogWalkSpeed, 150.0f);
    Target->SubmitInput(FBBBCharacterRescueEndLocalControlPacket{{Helper}, {OldOperation}, {OldRound}});
    Step();
    TestEqual(TEXT("晚到取消不撤销完成"), Target->GetHealth(), 100.0f);
    Damage(Target, 100);
    Step();
    Step();
    TestTrue(TEXT("重新倒地使用新轮次"), Target->GetDownedRevision() > OldRound);
    Begin();
    Target->SubmitInput(FBBBCharacterRescueEndLocalControlPacket{{Helper}, {OldOperation}, {OldRound}});
    Step();
    TestTrue(TEXT("旧取消不影响新救援"), Target->IsRescueReceiving());
    Helper->SubmitInput(FBBBCharacterMovementLocalControlPacket{FVector::RightVector, FRotator::ZeroRotator});
    Step();
    Step();
    TestFalse(TEXT("救援者移动立即取消双方"), Helper->IsRescueHelping() || Target->IsRescueReceiving());
    Helper->SubmitInput(FBBBCharacterMovementLocalControlPacket{});
    Helper->SetActorLocation(FVector(80, 0, 86));
    Begin();
    Helper->SetActorLocation(FVector(180, 0, 86));
    Step();
    Step();
    TestFalse(TEXT("持续超过一米立即取消双方"), Helper->IsRescueHelping() || Target->IsRescueReceiving());
    Helper->SetActorLocation(FVector(80, 0, 86));
    AActor *Obstacle = World->SpawnActor<AActor>();
    UBoxComponent *Block = NewObject<UBoxComponent>(Obstacle);
    Obstacle->SetRootComponent(Block);
    Block->SetBoxExtent(FVector(3, 32, 90));
    Block->SetCollisionProfileName(TEXT("BlockAll"));
    Block->RegisterComponent();
    Obstacle->SetActorLocation(FVector(500, 0, 90));
    Begin();
    Obstacle->SetActorLocation(FVector(40, 0, 90));
    Step();
    Step();
    TestFalse(TEXT("持续视线遮挡立即取消双方"), Helper->IsRescueHelping() || Target->IsRescueReceiving());
    Obstacle->SetActorLocation(FVector(500, 0, 90));
    Block->SetBoxExtent(FVector(32, 32, 10));
    Obstacle->SetActorLocation(FVector(0, 0, 150));
    Step();
    Step();
    Begin();
    for (int32 Index = 0; Index < 35; ++Index)
    {
        Step();
    }
    TestTrue(TEXT("低顶恢复存活与蹲伏"), Target->GetLifePhase() == EBBBCharacterLifePhase::Alive && Target->bIsCrouched);
    TestEqual(TEXT("低顶使用蹲伏胶囊体"), Target->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight(), 60.0f);
    Damage(Target, 100);
    Step();
    Step();
    Obstacle->SetActorLocation(FVector(0, 0, 110));
    Begin();
    TestFalse(TEXT("站蹲均不容纳时拒绝救援"), Helper->IsRescueHelping() || Target->IsRescueReceiving());
    TestTrue(TEXT("空间不足保持倒地"), Target->GetLifePhase() == EBBBCharacterLifePhase::Downed);
    Obstacle->SetActorLocation(FVector(0, 0, 150));
    Begin();
    Obstacle->SetActorLocation(FVector(0, 0, 110));
    Step();
    Step();
    TestFalse(TEXT("救援期间空间不足取消并清空进度"), Target->IsRescueReceiving());
    TestEqual(TEXT("空间取消清空进度"), Target->GetRescueProgress(), 0.0f);
    Obstacle->Destroy();
    Begin();
    Damage(Target, 300);
    Step(3.1f);
    TestTrue(TEXT("同帧致命伤害优先于完成"), Target->GetLifePhase() == EBBBCharacterLifePhase::Dead);
    TestEqual(TEXT("死亡不能被救援恢复"), Target->GetHealth(), 0.0f);
    Step();
    TestFalse(TEXT("死亡释放救援者"), Helper->IsRescueHelping());
    ABBBCharacter *Mirror = World->SpawnActor<ABBBCharacter>(CharacterClass, FVector(0, 0, 40), FRotator::ZeroRotator);
    APlayerController *RemoteController = World->SpawnActor<APlayerController>();
    if (!TestNotNull(TEXT("镜像角色"), Mirror) || !TestNotNull(TEXT("远端控制者"), RemoteController))
    {
        return false;
    }
    RemoteController->SetPlayerState(World->SpawnActor<APlayerState>());
    RemoteController->Possess(Mirror);
    Mirror->SetActorTickEnabled(false);
    Mirror->GetCharacterMovement()->SetComponentTickEnabled(false);
    Characters.Add(Mirror);
    Mirror->SubmitInput(FBBBCharacterLifeAuthorityFactPacket{{EBBBCharacterLifePhase::Downed}, {300}, {20},
        {0}, {NAME_None}, {FVector::ZeroVector}, {FVector::ZeroVector}, {20}, {false}});
    Step();
    Step();
    TestTrue(TEXT("镜像倒地角色仍可作为本地救援候选"), Helper->GetRescueTarget() == Mirror);
    Mirror->SubmitInput(FBBBCharacterRescueSnapshotAuthorityFactPacket{{Helper}, {1}, {20}, {10},
        {false}, {true}, {true}, {1.5f}, {3.0f}, {NAME_None}});
    Step();
    TestTrue(TEXT("镜像只恢复被救事实"), Mirror->IsRescueReceiving());
    TestEqual(TEXT("镜像恢复当前进度"), Mirror->GetRescueProgress(), 0.5f);
    Mirror->SubmitInput(FBBBCharacterRescueSnapshotAuthorityFactPacket{{nullptr}, {0}, {0}, {9},
        {false}, {false}, {false}, {0}, {3.0f}, {TEXT("Cancelled")}});
    Step();
    TestTrue(TEXT("旧镜像结果不能覆盖新状态"), Mirror->IsRescueReceiving());
    TestEqual(TEXT("镜像计时不产生治疗"), Mirror->GetHealth(), 300.0f);
    Mirror->SubmitInput(FBBBCharacterRescueSnapshotAuthorityFactPacket{{Helper}, {2}, {20}, {11},
        {true}, {false}, {false}, {2.0f}, {3.0f}, {NAME_None}});
    Step();
    Step();
    TestTrue(TEXT("镜像保留等待接受的帮扶表现"), Mirror->IsRescueHelping());
    TestEqual(TEXT("等待接受时镜像进度不提前增长"), Mirror->GetRescueProgress(), 0.0f);
    Mirror->SubmitInput(FBBBCharacterRescueSnapshotAuthorityFactPacket{{Helper}, {2}, {20}, {13},
        {true}, {false}, {true}, {0.0f}, {3.0f}, {NAME_None}});
    Mirror->SubmitInput(FBBBCharacterRescueSnapshotAuthorityFactPacket{{Helper}, {2}, {20}, {12},
        {true}, {false}, {false}, {2.0f}, {3.0f}, {NAME_None}});
    Step();
    Step();
    TestTrue(TEXT("收到接受事实后镜像进度才增长"), Mirror->GetRescueProgress() > 0.0f);

    return !HasAnyErrors();
}
#endif
