#include "Misc/AutomationTest.h"

#if WITH_DEV_AUTOMATION_TESTS && WITH_EDITOR
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Shotgun/BBBShotgunEquipment.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Shotgun/Config/BBBShotgunDefinition.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Shotgun/Logic/System/ActionSystem/DomainData/Context/BBBShotgunActionUpdateContext.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Shotgun/Logic/System/ActionSystem/Processors/BBBShotgunActionProcessor.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Shotgun/Logic/System/ParseSystem/DomainData/Context/BBBShotgunParseUpdateContext.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Shotgun/Logic/System/ParseSystem/Processors/BBBShotgunParseProcessor.h"
#include "Animation/AnimMontage.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "Misc/ScopeExit.h"
#include "UObject/UnrealType.h"

/** 使用真实输入解析与动作系统验证逐发装填边界 不播放动画或重演网络逻辑 */
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBBBShotgunReloadTest, "BBB.Equipment.Shotgun.PerShellReload",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FBBBShotgunReloadTest::RunTest(const FString &Parameters)
{
    const auto Initialization = UWorld::InitializationValues().AllowAudioPlayback(false)
        .CreatePhysicsScene(true).CreateNavigation(false).CreateAISystem(false).ShouldSimulatePhysics(false);
    UWorld *World = UWorld::CreateWorld(EWorldType::Game, false, NAME_None, nullptr,
        true, ERHIFeatureLevel::Num, &Initialization);
    if (!TestNotNull(TEXT("隔离测试世界"), World))
    {
        return false;
    }
    GEngine->CreateNewWorldContext(EWorldType::Game).SetCurrentWorld(World);
    ON_SCOPE_EXIT
    {
        GEngine->DestroyWorldContext(World);
        World->DestroyWorld(false);
    };

    UClass *EquipmentClass = LoadClass<ABBBShotgunEquipment>(nullptr,
        TEXT("/Game/_Project/Characters/BBBC_UA/Items/Equipment/Shotgun/Shotgun_01/BP_ModernWeapons_Shotgun_01.BP_ModernWeapons_Shotgun_01_C"));
    if (!TestNotNull(TEXT("霰弹枪蓝图"), EquipmentClass))
    {
        return false;
    }
    UClass *CharacterClass = LoadClass<ABBBCharacter>(nullptr,
        TEXT("/Game/_Project/Characters/BBBC_UA/BBBC_UA_0.BBBC_UA_0_C"));
    if (!TestNotNull(TEXT("具有正式配置的角色蓝图"), CharacterClass))
    {
        return false;
    }
    auto *Character = World->SpawnActor<ABBBCharacter>(CharacterClass);
    auto *Controller = World->SpawnActor<APlayerController>();
    Controller->Possess(Character);
    Character->DispatchBeginPlay();
    Character->Tick(0.0f);
    auto *Equipment = World->SpawnActor<ABBBShotgunEquipment>(EquipmentClass);
    Equipment->SetOwner(Character);
    Equipment->DispatchBeginPlay();
    auto *Definition = DuplicateObject<UBBBShotgunDefinition>(Cast<UBBBShotgunDefinition>(Equipment->GetDefinition()), GetTransientPackage());
    auto *DefinitionProperty = FindFProperty<FObjectPropertyBase>(ABBBEquipment::StaticClass(), TEXT("Definition"));
    if (!TestNotNull(TEXT("独立测试配置"), Definition) || !TestNotNull(TEXT("配置字段"), DefinitionProperty))
    {
        return false;
    }
    DefinitionProperty->SetObjectPropertyValue_InContainer(Equipment, Definition);
    Definition->bReloadOneRoundAtATime = true;
    Definition->CharacterReloadStartMontage = NewObject<UAnimMontage>();
    Definition->CharacterReloadEndMontage = NewObject<UAnimMontage>();
    Definition->EquipmentReloadStartMontage = NewObject<UAnimMontage>();
    Definition->EquipmentReloadEndMontage = NewObject<UAnimMontage>();
    Definition->AmmoCapacity = 1;
    auto &Data = Equipment->RuntimeData;
    FBBBShotgunActionProcessor::Initialize(Data, *Definition);
    const auto Update = [&]()
    {
        FBBBShotgunParseUpdateContext ParseContext{Data, true};
        FBBBShotgunParseProcessor::Update(ParseContext);
        FBBBShotgunActionUpdateContext ActionContext{*Equipment, *Character,
            *Equipment->GetEquipmentSkeletalMesh(), *Definition, Data, *World, true};
        FBBBShotgunActionProcessor::Update(ActionContext);
    };
    const auto &State = Data.Action.ReadShotgunActionState();

    FBBBShotgunParseProcessor::Submit(Data, FBBBShotgunPrimaryLocalControlPacket{});
    Update();
    TestEqual(TEXT("第一枪消耗唯一弹药"), State.LoadedAmmo, 0);
    Update();
    FBBBShotgunParseProcessor::Submit(Data, FBBBShotgunSecondaryLocalControlPacket{});
    Update();
    TestTrue(TEXT("空枪开始装填"), State.bIsReloading);
    FBBBShotgunParseProcessor::Submit(Data, FBBBShotgunPrimaryLocalControlPacket{});
    Update();
    TestTrue(TEXT("空枪记住射击请求"), State.bFireAfterReload);
    TestFalse(TEXT("没有装入前不能提前收手"), State.bReloadFinishing);
    const int32 BeforeInsert = State.FireSequence;
    FBBBShotgunParseProcessor::Submit(Data, FBBBShotgunLoadAmmoLocalControlPacket{});
    Update();
    TestEqual(TEXT("插入只增加一发"), State.LoadedAmmo, 1);
    TestFalse(TEXT("装满结束装填"), State.bIsReloading);
    TestTrue(TEXT("插入后进入收手"), State.bReloadFinishing);
    TestEqual(TEXT("收手前不能射击"), State.FireSequence, BeforeInsert);
    FBBBShotgunParseProcessor::Submit(Data, FBBBShotgunInterruptReloadLocalControlPacket{});
    FBBBShotgunParseProcessor::Submit(Data, FBBBShotgunLoadAmmoLocalControlPacket{});
    Update();
    TestTrue(TEXT("被覆盖动作的取消通知不抹掉排队射击"), State.bFireAfterReload);
    TestTrue(TEXT("旧通知不提前结束收手"), State.bReloadFinishing);
    TestEqual(TEXT("收手阶段忽略旧装弹通知"), State.LoadedAmmo, 1);
    FBBBShotgunParseProcessor::Submit(Data, FBBBShotgunReloadCycleEndLocalControlPacket{});
    Update();
    TestTrue(TEXT("旧装填周期自然结束不能解除收手限制"), State.bReloadFinishing);
    FBBBShotgunParseProcessor::Submit(Data, FBBBShotgunReloadCycleEndLocalControlPacket{true});
    Update();
    TestFalse(TEXT("收手自然结束解除阶段限制"), State.bReloadFinishing);

    FBBBShotgunActionProcessor::Stop(Data);
    TestFalse(TEXT("失活清除待发射请求"), State.bFireAfterReload);
    TestFalse(TEXT("失活清除收手阶段"), State.bReloadFinishing);
    TestEqual(TEXT("失活不丢弃已装弹药"), State.LoadedAmmo, 1);
    return true;
}
#endif
