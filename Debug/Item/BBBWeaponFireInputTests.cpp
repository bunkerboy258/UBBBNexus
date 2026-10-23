#include "Misc/AutomationTest.h"

#if WITH_DEV_AUTOMATION_TESTS && WITH_EDITOR
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Pistol/BBBPistolEquipment.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Pistol/Config/BBBPistolDefinition.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Pistol/Logic/System/ActionSystem/DomainData/Context/BBBPistolActionUpdateContext.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Pistol/Logic/System/ActionSystem/Processors/BBBPistolActionProcessor.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Pistol/Logic/System/ParseSystem/DomainData/Context/BBBPistolParseUpdateContext.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Pistol/Logic/System/ParseSystem/Processors/BBBPistolParseProcessor.h"
#include "BBBWork/UBBBNexus/Player/Camera/BBBPlayerCameraSystem.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "Misc/ScopeExit.h"

/** 使用真实输入槽和动作处理验证连点缓冲与连续按住的区别 */
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBBBWeaponClickTest, "BBB.Equipment.Fire.ClickResponsiveness",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FBBBWeaponClickTest::RunTest(const FString &Parameters)
{
    const auto Initialization = UWorld::InitializationValues().AllowAudioPlayback(false)
        .CreatePhysicsScene(true).CreateNavigation(false).CreateAISystem(false).ShouldSimulatePhysics(false);
    UWorld *World = UWorld::CreateWorld(EWorldType::Game, false, NAME_None, nullptr,
        true, ERHIFeatureLevel::Num, &Initialization);
    if (!TestNotNull(TEXT("输入验收世界"), World))
    {
        return false;
    }
    GEngine->CreateNewWorldContext(EWorldType::Game).SetCurrentWorld(World);
    ON_SCOPE_EXIT
    {
        GEngine->DestroyWorldContext(World);
        World->DestroyWorld(false);
    };
    auto *Character = World->SpawnActor<ABBBCharacter>();
    auto *Equipment = World->SpawnActor<ABBBPistolEquipment>();
    const auto *SourceDefinition = LoadObject<UBBBPistolDefinition>(nullptr,
        TEXT("/Game/_Project/Characters/BBBC_UA/Items/Equipment/Pistol/Pistol_01/DA_ModernWeapons_Pistol_01.DA_ModernWeapons_Pistol_01"));
    if (!TestNotNull(TEXT("正式手枪配置"), SourceDefinition))
    {
        return false;
    }
    auto *Definition = DuplicateObject<UBBBPistolDefinition>(SourceDefinition, GetTransientPackage());
    Definition->FireInterval = 0.1f;
    Definition->AmmoCapacity = 12;
    Definition->bAutomaticFire = false;
    Equipment->SetOwner(Character);
    Equipment->GetEquipmentSkeletalMesh()->SetSkeletalMesh(Definition->EquipmentMesh);
    Character->SetActorTickEnabled(false);
    Character->GetMesh()->SetComponentTickEnabled(false);
    Equipment->SetActorTickEnabled(false);
    Equipment->GetEquipmentSkeletalMesh()->SetComponentTickEnabled(false);
    auto &Data = Equipment->RuntimeData;
    FBBBPistolActionProcessor::Initialize(Data, *Definition);
    const auto Update = [&]()
    {
        FBBBPistolParseUpdateContext Parse{Data, true};
        FBBBPistolParseProcessor::Update(Parse);
        FBBBPistolActionUpdateContext Action{*Equipment, *Character,
            *Equipment->GetEquipmentSkeletalMesh(), *Definition, Data, *World, true};
        FBBBPistolActionProcessor::Update(Action);
    };
    const auto Step = [&](const float Seconds, const int32 PressCount)
    {
        World->Tick(LEVELTICK_All, Seconds);
        FBBBPistolParseProcessor::Submit(Data, FBBBPistolPrimaryLocalControlPacket{{PressCount}});
        Update();
    };
    const auto &State = Data.Action.ReadPistolActionState();
    Step(0.0f, 1);
    TestEqual(TEXT("独立点击立即射击"), State.FireSequence, 1);
    Step(0.04f, 0);
    Step(0.03f, 1);
    TestEqual(TEXT("冷却内点击不提前开火"), State.FireSequence, 1);
    Step(0.031f, 0);
    TestEqual(TEXT("冷却边缘点击在就绪时执行一次"), State.FireSequence, 2);
    Step(0.2f, 0);
    TestEqual(TEXT("半自动持续按住不会连发"), State.FireSequence, 2);
    Step(0.001f, 1);
    TestEqual(TEXT("下一次真实点击立即响应"), State.FireSequence, 3);
    Step(0.02f, 1);
    Update();
    Step(0.1f, 0);
    TestEqual(TEXT("释放取消尚未就绪的点击"), State.FireSequence, 3);
    Step(0.001f, 1);
    Step(0.02f, 1);
    FBBBPistolActionProcessor::Stop(Data);
    Step(0.1f, 0);
    TestEqual(TEXT("生命周期清理不遗留延后射击"), State.FireSequence, 4);

    FBBBPistolParseProcessor::Submit(Data, FBBBPistolPrimaryLocalControlPacket{{2}});
    FBBBPistolParseProcessor::Submit(Data, FBBBPistolPrimaryLocalControlPacket{{3}});
    FBBBPistolParseUpdateContext Parse{Data, true};
    FBBBPistolParseProcessor::Update(Parse);
    TestEqual(TEXT("同帧有数据输入累加点击事实"), Data.Action.ReadPistolActionInputState().PrimaryPressCount, 5);
    FBBBPistolParseProcessor::Update(Parse);
    TestEqual(TEXT("点击事实只消费一帧"), Data.Action.ReadPistolActionInputState().PrimaryPressCount, 0);
    return true;
}

/** 真实相机收到的同帧外部冲击必须叠加 */
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBBBWeaponCameraContributionTest, "BBB.Equipment.Fire.CameraContributions",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FBBBWeaponCameraContributionTest::RunTest(const FString &Parameters)
{
    const auto Initialization = UWorld::InitializationValues().AllowAudioPlayback(false)
        .CreatePhysicsScene(true).CreateNavigation(false).CreateAISystem(false).ShouldSimulatePhysics(false);
    UWorld *World = UWorld::CreateWorld(EWorldType::Game, false, NAME_None, nullptr,
        true, ERHIFeatureLevel::Num, &Initialization);
    if (!TestNotNull(TEXT("镜头验收世界"), World))
    {
        return false;
    }
    GEngine->CreateNewWorldContext(EWorldType::Game).SetCurrentWorld(World);
    ON_SCOPE_EXIT
    {
        GEngine->DestroyWorldContext(World);
        World->DestroyWorld(false);
    };
    auto *Character = World->SpawnActor<ABBBCharacter>();
    auto *Controller = World->SpawnActor<APlayerController>();
    auto *Camera = World->SpawnActor<ABBBPlayerCameraSystem>();
    Camera->Initialize(*Character, *Controller);
    Camera->Submit(FBBBPlayerCameraInput{FVector(1.0, 0.1, 0.0)});
    Camera->Submit(FBBBPlayerCameraInput{FVector(2.0, 0.2, 0.0)});
    Camera->Tick(0.0f);
    TestTrue(TEXT("同帧俯仰冲击相加"), FMath::IsNearlyEqual(Controller->GetControlRotation().Pitch, 3.0, 0.001));
    TestTrue(TEXT("同帧水平冲击相加"), FMath::IsNearlyEqual(Controller->GetControlRotation().Yaw, 0.3, 0.001));
    Camera->Tick(0.0f);
    TestTrue(TEXT("已消费冲击不会重复应用"), FMath::IsNearlyEqual(Controller->GetControlRotation().Pitch, 3.0, 0.001));
    return true;
}
#endif
