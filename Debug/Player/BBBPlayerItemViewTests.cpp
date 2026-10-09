#if WITH_DEV_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"
#include "BBBWork/UBBBNexus/Player/BBBPlayerController.h"
#include "BBBWork/UBBBNexus/Player/BBBPlayerItemDisplayData.h"
#include "BBBWork/UBBBNexus/Player/UI/BBBPlayerItemView.h"
#include "BBBWork/UBBBNexus/Player/UI/BBBPlayerItemPortrait.h"
#include "BBBWork/UBBBNexus/Player/UI/SBBBPlayerItemSlot.h"
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "Engine/Engine.h"
#include "Engine/LocalPlayer.h"
#include "Engine/World.h"
#include "Engine/SceneCapture2D.h"
#include "EngineUtils.h"
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
    const int32 ContextsBeforeBag = GEngine->GetWorldContexts().Num();
    const auto CaptureCount = [World]()
    {
        int32 Count = 0;
        for (TActorIterator<ASceneCapture2D> Actor(World); Actor; ++Actor)
        {
            ++Count;
        }
        return Count;
    };
    TestEqual(TEXT("打开前没有人物捕获演员"), CaptureCount(), 0);
    View->SetBackpackOpen(true);
    TestEqual(TEXT("人物捕获不建立额外世界上下文"), GEngine->GetWorldContexts().Num(), ContextsBeforeBag);
    TestEqual(TEXT("捕获演员只生成在玩家世界"), CaptureCount(), 1);
    UBBBPlayerItemPortrait *Portrait = NewObject<UBBBPlayerItemPortrait>(Controller);
    TestTrue(TEXT("建立穿脱回归捕获"), Portrait->Open(Character));
    Controller->SubmitItemAdd(TEXT("Helmet_UASoldHelmet01"));
    Character->Tick(1.0f / 60.0f);
    const FGuid Helmet = Controller->GetItemInstanceId(5);
    TestTrue(TEXT("头盔登记真实身份"), Helmet.IsValid());
    Controller->SubmitItemMove(5, 0, Helmet);
    Character->Tick(1.0f / 60.0f);
    TestEqual(TEXT("头盔与快捷武器交换被拒绝"), Controller->GetItemInstanceId(5), Helmet);
    TestEqual(TEXT("非法交换保留快捷武器"), Controller->GetItemInstanceId(0), First);
    TestEqual(TEXT("头盔双击目标为对应穿戴位置"), Controller->GetItemDisplayData(5).EquipSlot, 55);
    TestTrue(TEXT("双击穿戴请求接受"), View->EquipItem(5, Character, Helmet));
    Character->Tick(1.0f / 60.0f);
    Portrait->Update(1.0f);
    TestTrue(TEXT("穿戴后捕获真实位置"), Controller->GetItemInstanceId(55) == Helmet);
    Controller->SubmitItemAdd(TEXT("Helmet_UASoldHelmet00"));
    Character->Tick(1.0f / 60.0f);
    const FGuid ReplacementHelmet = Controller->GetItemInstanceId(5);
    TestTrue(TEXT("双击替换头盔请求接受"), View->EquipItem(5, Character, ReplacementHelmet));
    Character->Tick(1.0f / 60.0f);
    TestEqual(TEXT("新头盔进入穿戴位"), Controller->GetItemInstanceId(55), ReplacementHelmet);
    TestEqual(TEXT("旧头盔回到替换来源格"), Controller->GetItemInstanceId(5), Helmet);
    Controller->SubmitItemMove(55, 5, ReplacementHelmet);
    Character->Tick(1.0f / 60.0f);
    Portrait->Update(1.0f);
    Controller->SubmitItemMove(55, 6, Helmet);
    Character->Tick(1.0f / 60.0f);
    Portrait->Update(1.0f);
    TestTrue(TEXT("脱下清空网格后捕获安全"), Controller->GetItemInstanceId(6) == Helmet
        && !Controller->GetItemInstanceId(55).IsValid());
    Portrait->Close();
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
    TestTrue(TEXT("武器移到普通格请求接受"), View->MoveItem(1, 7, Character, First));
    Character->Tick(1.0f / 60.0f);
    TestFalse(TEXT("双击拒绝过期物品身份"), View->EquipItem(7, Character, Second));
    const TSharedRef<SBBBPlayerItemSlot> WeaponSlot = SNew(SBBBPlayerItemSlot).View(View).Slot(7);
    const FPointerEvent DoubleClick(0, FVector2D::ZeroVector, FVector2D::ZeroVector,
        TSet<FKey>{EKeys::LeftMouseButton}, EKeys::LeftMouseButton, 0.0f, FModifierKeysState());
    TestTrue(TEXT("鼠标双击武器事件接受"), WeaponSlot->OnMouseButtonDoubleClick(FGeometry(), DoubleClick).IsEventHandled());
    TestEqual(TEXT("双击仍等待角色消费输入"), Controller->GetItemInstanceId(7), First);
    Character->Tick(1.0f / 60.0f);
    TestEqual(TEXT("双击武器进入快捷一号位"), Controller->GetItemInstanceId(0), First);
    TestEqual(TEXT("原一号位武器回到双击来源格"), Controller->GetItemInstanceId(7), Second);
    TestEqual(TEXT("双击武器选择快捷一号位"), Controller->GetSelectedItemSlot(), 0);
    TestTrue(TEXT("双击后新武器实际手持"), Controller->GetItemDisplayData(0).bActive);
    View->SetBackpackOpen(false);
    TestEqual(TEXT("关闭后清除玩家世界内的捕获演员"), CaptureCount(), 0);
    TestEqual(TEXT("关闭后世界上下文保持原值"), GEngine->GetWorldContexts().Num(), ContextsBeforeBag);
    TestFalse(TEXT("界面关闭后拒绝拖动"), View->MoveItem(1, 0, Character, First));
    TestFalse(TEXT("界面关闭后拒绝双击装备"), View->EquipItem(7, Character, Second));
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
