#include "Misc/AutomationTest.h"

#if WITH_DEV_AUTOMATION_TESTS && WITH_EDITOR
#include "Templates/SubclassOf.h"
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "BBBWork/UBBBNexus/Character/Animation/BBBAnimInstance.h"
#include "BBBWork/UBBBNexus/Character/Input/AuthorityFact/Locomotion/FBBBAccelerationAuthorityFactPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/RemoteMessage/Locomotion/FBBBAccelerationRemoteMessagePacket.h"
#include "BBBWork/UBBBNexus/Character/Input/AuthorityFact/Traversal/FBBBTraversalEndAuthorityFactPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/RemoteMessage/Traversal/FBBBTraversalEndRemoteMessagePacket.h"
#include "BBBWork/UBBBNexus/Character/Input/RemoteMessage/Traversal/FBBBTraversalStartRemoteMessagePacket.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/Engine.h"
#include "Engine/LocalPlayer.h"
#include "Engine/World.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/PlayerController.h"
#include "Misc/ScopeExit.h"

/** 经真实输入提交与解析验证移动事实的还原边界 */
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBBBCharacterAccelerationTest, "BBB.Character.AccelerationFacts",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FBBBCharacterAccelerationTest::RunTest(const FString &Parameters)
{
    const auto Initialization = UWorld::InitializationValues()
        .AllowAudioPlayback(false).CreatePhysicsScene(true).CreateNavigation(false).CreateAISystem(false);
    UWorld *World = UWorld::CreateWorld(EWorldType::Game, false, NAME_None, nullptr, true,
        ERHIFeatureLevel::Num, &Initialization);
    if (!TestNotNull(TEXT("隔离事实还原世界"), World))
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
    UClass *CharacterClass = LoadClass<ABBBCharacter>(nullptr,
        TEXT("/Game/_Project/Characters/BBBC_UA/BBBC_UA_0.BBBC_UA_0_C"));
    ABBBCharacter *Character = CharacterClass
        ? World->SpawnActor<ABBBCharacter>(CharacterClass, FVector(0, 0, 100), FRotator::ZeroRotator) : nullptr;
    if (!TestNotNull(TEXT("生产角色"), Character))
    {
        return false;
    }
    World->InitializeActorsForPlay(FURL());
    World->SetBegunPlay(true);
    Character->DispatchBeginPlay();
    // 生产蓝图可能自动生成 AI 控制器 隔离镜像必须先解除本机控制身份
    if (AController *AutomaticController = Character->GetController())
    {
        AutomaticController->UnPossess();
    }
    TestFalse(TEXT("隔离副本不具备本机控制身份"), Character->IsLocallyControlled());
    const auto Step = [&]()
    {
        Character->Tick(0.02f);
        Character->GetCharacterMovement()->TickComponent(0.02f, LEVELTICK_All, nullptr);
        World->Tick(LEVELTICK_All, 0.02f);
    };
    Step();
    TestTrue(TEXT("解析前具备镜像还原身份"), Character->IsNetworkMirror());

    // 同帧合并保留全部版本 解析只能收敛到最新结果
    Character->SubmitInput(FBBBAccelerationRemoteMessagePacket{
        {2, 4, 3}, {FVector(1200, 0, 0), FVector(0, 1200, 0), FVector::ZeroVector},
        {FVector(1, 0, 0), FVector(0, 1, 0), FVector::ZeroVector}});
    Step();
    const auto &Locomotion = Character->RuntimeData.Locomotion.ReadLocomotionState();
    TestEqual(TEXT("乱序结果取最新版本"), Locomotion.AccelerationRevision, uint64(4));
    TestEqual(TEXT("最新方向保持原始加速度"), Locomotion.RestoredAcceleration, FVector(0, 1200, 0));
    TestEqual(TEXT("移动输入与加速度按同一版本还原"), Locomotion.RestoredMovementInput, FVector(0, 1, 0));
    TestEqual(TEXT("还原事实不产生实际移动输入"),
        Character->GetCharacterMovement()->GetCurrentAcceleration(), FVector::ZeroVector);

    Character->SubmitInput(FBBBAccelerationAuthorityFactPacket{{4, 3},
        {FVector::ZeroVector, FVector::ZeroVector}, {FVector::ZeroVector, FVector::ZeroVector}});
    Step();
    TestEqual(TEXT("重复和迟到结果不得覆盖当前状态"), Locomotion.RestoredAcceleration, FVector(0, 1200, 0));
    // 根运动期间加速度为零也不能抹掉已经解析的持续移动输入
    Character->SubmitInput(FBBBAccelerationAuthorityFactPacket{{5}, {FVector::ZeroVector}, {FVector(0, 1, 0)}});
    Step();
    TestEqual(TEXT("根运动零加速度保留持续输入"), Locomotion.RestoredMovementInput, FVector(0, 1, 0));
    Character->SubmitInput(FBBBAccelerationAuthorityFactPacket{{6}, {FVector::ZeroVector}, {FVector::ZeroVector}});
    Step();
    TestEqual(TEXT("松开移动明确还原零加速度"), Locomotion.RestoredAcceleration, FVector::ZeroVector);
    TestEqual(TEXT("松开输入明确还原零移动输入"), Locomotion.RestoredMovementInput, FVector::ZeroVector);

    TestFalse(TEXT("数组错位被结构校验拒绝"),
        FBBBAccelerationRemoteMessagePacket{{1}, {}}.IsValid());
    TestFalse(TEXT("未生成版本被结构校验拒绝"),
        FBBBAccelerationAuthorityFactPacket{{0}, {FVector::ZeroVector}, {FVector::ZeroVector}}.IsValid());
    TestFalse(TEXT("超出传输向量范围被拒绝"),
        FBBBAccelerationAuthorityFactPacket{{1}, {FVector(100001, 0, 0)}, {FVector::ZeroVector}}.IsValid());
    TestFalse(TEXT("异常移动输入被结构校验拒绝"),
        FBBBAccelerationAuthorityFactPacket{{1}, {FVector::ZeroVector}, {FVector(2, 0, 0)}}.IsValid());

    // 结束结果与交权速度按同一动作合并 镜像不从校正尾速或移动输入重新推导
    Character->SubmitInput(FBBBTraversalEndRemoteMessagePacket{{10}, {FVector(0, 200, 0)}});
    Character->SubmitInput(FBBBTraversalEndRemoteMessagePacket{{11}, {FVector(-150, 0, -420)}});
    Character->SubmitInput(FBBBTraversalEndRemoteMessagePacket{{9}, {FVector(300, 0, 0)}});
    Step();
    TestEqual(TEXT("同帧结束结果取最新动作"), Character->RuntimeData.Traversal.ReadTraversalState().ActionId, uint32(11));
    TestEqual(TEXT("交权速度与最新动作严格对应"), Locomotion.TraversalExitVelocity, FVector(-150, 0, -420));
    TestTrue(TEXT("镜像已经接收交权结果"), Locomotion.bTraversalExitPrepared);
    Character->SubmitInput(FBBBTraversalEndAuthorityFactPacket{{10}, {FVector(0, 300, 0)}});
    Step();
    TestEqual(TEXT("迟到结束不得覆盖交权方向"), Locomotion.TraversalExitVelocity, FVector(-150, 0, -420));
    Character->SubmitInput(FBBBTraversalEndAuthorityFactPacket{{12}, {FVector::ZeroVector}});
    Step();
    TestEqual(TEXT("无输入结束明确还原零尾速"), Locomotion.TraversalExitVelocity, FVector::ZeroVector);
    TestEqual(TEXT("交权事实不重演镜像加速度"),
        Character->GetCharacterMovement()->GetCurrentAcceleration(), FVector::ZeroVector);
    // 上升与下降都由同一交权结果还原 远端不根据自身高度重新计算垂直速度
    Character->SubmitInput(FBBBTraversalEndAuthorityFactPacket{{13}, {FVector(200, 0, 180)}});
    Step();
    TestEqual(TEXT("权威交权结果保留完整空中速度"), Locomotion.TraversalExitVelocity, FVector(200, 0, 180));
    TestTrue(TEXT("空中下降交权速度符合传输边界"),
        FBBBTraversalEndRemoteMessagePacket{{14}, {FVector(200, 0, -420)}}.IsValid());
    TestFalse(TEXT("动作与交权速度数量错位被拒绝"), FBBBTraversalEndRemoteMessagePacket{{1}, {}}.IsValid());
    TestFalse(TEXT("未生成动作的交权结果被拒绝"), FBBBTraversalEndAuthorityFactPacket{{0}, {FVector::ZeroVector}}.IsValid());
    TestFalse(TEXT("异常交权速度被拒绝"), FBBBTraversalEndAuthorityFactPacket{{1}, {FVector(10001, 0, 0)}}.IsValid());

    // 开始与结束同帧到达时没有根运动贡献 不等待一个从未创建的播放实例
    Character->SubmitInput(FBBBTraversalStartRemoteMessagePacket{{20}, {EBBBTraversalAction::ClimbLow},
        {FTransform::Identity}, {FTransform::Identity}, {0.0f}});
    Character->SubmitInput(FBBBTraversalEndRemoteMessagePacket{{20}, {FVector::ZeroVector}});
    Step();
    TestEqual(TEXT("尚未播放就结束的动作立即完成清理"),
        Character->RuntimeData.Traversal.ReadTraversalState().Action, EBBBTraversalAction::None);
    TestFalse(TEXT("已结束动作不再占用根运动控制"), Locomotion.bTraversalControlled);

    // 本机控制者继续使用自身 CMC 结果 不接收镜像还原输入
    APlayerController *Controller = World->SpawnActor<APlayerController>();
    ULocalPlayer *LocalPlayer = NewObject<ULocalPlayer>(GEngine);
    LocalPlayer->PlayerController = Controller;
    Controller->Player = LocalPlayer;
    Controller->Possess(Character);
    Controller->DispatchBeginPlay();
    Character->SubmitInput(FBBBAccelerationAuthorityFactPacket{{7}, {FVector(1200, 0, 0)}, {FVector(1, 0, 0)}});
    Step();
    TestEqual(TEXT("控制者拒绝镜像事实"), Locomotion.AccelerationRevision, uint64(6));
    Character->SubmitInput(FBBBTraversalEndAuthorityFactPacket{{21}, {FVector(300, 0, 0)}});
    Step();
    TestEqual(TEXT("控制者拒绝镜像交权结果"), Character->RuntimeData.Traversal.ReadTraversalState().ActionId, uint32(20));
    return true;
}
#endif
