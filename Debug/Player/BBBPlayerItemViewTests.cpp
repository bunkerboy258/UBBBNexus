#if WITH_DEV_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"
#include "BBBWork/UBBBNexus/Player/BBBPlayerController.h"
#include "BBBWork/UBBBNexus/Player/BBBPlayerItemDisplayData.h"
#include "BBBWork/UBBBNexus/Player/UI/BBBPlayerItemView.h"
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "Engine/Engine.h"
#include "Engine/LocalPlayer.h"
#include "Engine/World.h"
#include "Misc/ScopeExit.h"

/** 通过公开控制器接口验证界面请求 不访问物品系统或写入领域状态 */
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBBBPlayerItemViewTest, "BBB.Player.ItemView",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FBBBPlayerItemViewTest::RunTest(const FString &Parameters)
{
    const auto Initialization = UWorld::InitializationValues()
        .AllowAudioPlayback(false).CreatePhysicsScene(true).CreateNavigation(false)
        .CreateAISystem(false).ShouldSimulatePhysics(false);
    UWorld *World = UWorld::CreateWorld(EWorldType::Game, false, NAME_None, nullptr,
        true, ERHIFeatureLevel::Num, &Initialization);
    if (!TestNotNull(TEXT("隔离世界"), World))
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
    if (!TestNotNull(TEXT("项目玩家角色类"), CharacterClass))
    {
        return false;
    }
    ABBBCharacter *Character = World->SpawnActor<ABBBCharacter>(CharacterClass);
    ABBBPlayerController *Controller = World->SpawnActor<ABBBPlayerController>();
    if (!TestNotNull(TEXT("玩家角色"), Character) || !TestNotNull(TEXT("玩家控制器"), Controller))
    {
        return false;
    }
    ULocalPlayer *LocalPlayer = NewObject<ULocalPlayer>(GEngine);
    LocalPlayer->PlayerController = Controller;
    Controller->Player = LocalPlayer;
    Controller->Possess(Character);
    World->InitializeActorsForPlay(FURL());
    World->SetBegunPlay(true);
    Character->DispatchBeginPlay();

    Controller->SubmitItemAdd(TEXT("ModernWeapons_Rifle_01"));
    Controller->SubmitItemAdd(TEXT("ModernWeapons_Rifle_02"));
    Character->Tick(1.0f / 60.0f);
    if (!TestTrue(TEXT("角色生命周期初始化真实背包"), Controller->GetItemSlotCount() == 63
        && Controller->GetItemDisplayData(0).bOccupied && Controller->GetItemDisplayData(1).bOccupied))
    {
        return false;
    }
    const FGuid First = Controller->GetItemDisplayData(0).InstanceId;
    const FGuid Second = Controller->GetItemDisplayData(1).InstanceId;
    TestTrue(TEXT("公开身份查询与展示数据一致"), Controller->GetItemInstanceId(0) == First);
    TestFalse(TEXT("空格身份无效"), Controller->GetItemInstanceId(2).IsValid());
    TestFalse(TEXT("越界身份无效"), Controller->GetItemInstanceId(-1).IsValid());

    UBBBPlayerItemView *View = NewObject<UBBBPlayerItemView>(Controller);
    View->SetOwningPlayer(Controller);
    TestTrue(TEXT("初始化原生物品界面"), View->Initialize());
    const TSharedRef<SWidget> Widget = View->TakeWidget();
    View->SetBackpackOpen(true);
    TestTrue(TEXT("控制器提供物品展示数据"), Controller->GetItemDisplayData(0).bOccupied
        && !Controller->GetItemDisplayData(0).Name.IsEmpty());
    TestFalse(TEXT("普通槽位不能由界面直接选中"), View->SelectSlot(Controller->GetQuickAccessSlotCount()));
    TestTrue(TEXT("界面提交快捷选择"), View->SelectSlot(0));
    TestEqual(TEXT("界面不提前修改选择"), Controller->GetSelectedItemSlot(), INDEX_NONE);
    Character->Tick(1.0f / 60.0f);
    TestEqual(TEXT("界面读取角色处理后的选择"), Controller->GetSelectedItemSlot(), 0);
    TestTrue(TEXT("控制器区分实际装备状态"), Controller->GetItemDisplayData(0).bActive);
    TestTrue(TEXT("界面提交拖动交换"), View->MoveItem(0, 1, Character, First));
    TestTrue(TEXT("界面不提前改变槽位内容"), Controller->GetItemDisplayData(0).InstanceId == First);
    Character->Tick(1.0f / 60.0f);
    TestTrue(TEXT("角色完成交换"), Controller->GetItemDisplayData(0).InstanceId == Second
        && Controller->GetItemDisplayData(1).InstanceId == First);
    TestTrue(TEXT("手持装备跟随选择格内容"), Controller->GetItemDisplayData(0).bActive);

    TestFalse(TEXT("拒绝物品已换位的旧拖动"), View->MoveItem(0, 1, Character, First));
    TestFalse(TEXT("拒绝角色失效的旧拖动"), View->MoveItem(1, 0, nullptr, First));
    View->SetBackpackOpen(false);
    TestFalse(TEXT("界面关闭后拒绝拖动"), View->MoveItem(1, 0, Character, First));
    TestTrue(TEXT("界面提交收起"), View->SelectSlot(INDEX_NONE));
    Character->Tick(1.0f / 60.0f);
    TestEqual(TEXT("收起后未选择槽位"), Controller->GetSelectedItemSlot(), INDEX_NONE);
    TestNull(TEXT("收起后实际空手"), Controller->GetActiveItem());
    TestEqual(TEXT("快捷栏使用前五格"), Controller->GetQuickAccessSlotCount(), 5);
    TestEqual(TEXT("背包与穿戴共六十三格"), Controller->GetItemSlotCount(), 63);
    TestEqual(TEXT("满生命显示完整白线"), Controller->GetHudHealthFraction(), 1.0f);
    TestFalse(TEXT("空手不显示瞄准界面"), Controller->ShouldShowAimHud());
    const float MaximumHealth = Character->GetCharacterConfig().MaximumHealth;
    Character->SubmitInput(FBBBCharacterDamageLocalControlPacket{
        {MaximumHealth * 0.25f}, {NAME_None}, {Character->GetActorLocation()}, {FVector::ZeroVector}, {nullptr}});
    Character->Tick(1.0f / 60.0f);
    TestEqual(TEXT("白线读取真实生命损失"), Controller->GetHudHealthFraction(), 0.75f);
    Controller->UnPossess();
    TestFalse(TEXT("失去角色后身份无效"), Controller->GetItemInstanceId(0).IsValid());
    TestEqual(TEXT("失去角色后生命接口归零"), Controller->GetHudHealthFraction(), 0.0f);
    return true;
}

#endif
