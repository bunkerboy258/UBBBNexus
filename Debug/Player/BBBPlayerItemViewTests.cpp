#if WITH_DEV_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"
#include "BBBWork/UBBBNexus/Player/BBBPlayerController.h"
#include "BBBWork/UBBBNexus/Player/BBBPlayerItemDisplayData.h"
#include "BBBWork/UBBBNexus/Player/UI/BBBPlayerItemView.h"
#include "BBBWork/UBBBNexus/Player/UI/BBBPlayerItemPortrait.h"
#include "BBBWork/UBBBNexus/Player/UI/SBBBPlayerItemSlot.h"
#include "BBBWork/UBBBNexus/Player/UI/SBBBCombatHud.h"
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Base/BBBEquipment.h"
#include "BBBWork/UBBBNexus/Mass/Core/BBBMassSubsystem.h"
#include "MassCommonFragments.h"
#include "Components/BoxComponent.h"
#include "Input/HittestGrid.h"
#include "Types/PaintArgs.h"
#include "Rendering/DrawElements.h"
#include "Engine/Engine.h"
#include "Engine/LocalPlayer.h"
#include "Engine/World.h"
#include "Engine/SceneCapture2D.h"
#include "EngineUtils.h"
#include "Misc/ScopeExit.h"
#include "Widgets/Input/SButton.h"

/** 通过公开控制器接口验证界面请求 不访问物品系统或写入领域状态 */
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBBBPlayerItemViewTest, "BBB.Player.ItemView.Interaction",
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
        World->GetOutermost()->SetDirtyFlag(false);
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
    if (!TestTrue(TEXT("角色生命周期初始化真实背包"), Controller->GetItemSlotCount() == 110
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
    TFunction<TSharedPtr<SWidget>(const TSharedRef<SWidget> &, FName)> FindTagged;
    FindTagged = [&FindTagged](const TSharedRef<SWidget> &Node, FName Tag) -> TSharedPtr<SWidget>
    {
        if (Node->GetTag() == Tag)
        {
            return Node;
        }
        FChildren *Children = Node->GetChildren();
        for (int32 Index = 0; Index < Children->Num(); ++Index)
        {
            if (TSharedPtr<SWidget> Found = FindTagged(Children->GetChildAt(Index), Tag))
            {
                return Found;
            }
        }
        return nullptr;
    };
    const auto Click = [&](FName Tag)
    {
        const TSharedPtr<SWidget> Button = FindTagged(Widget, Tag);
        if (TestTrue(FString::Printf(TEXT("导航按钮存在 %s"), *Tag.ToString()),
            Button.IsValid() && Button->GetType() == TEXT("SButton")))
        {
            StaticCastSharedPtr<SButton>(Button)->SimulateClick();
        }
    };
    TFunction<int32(const TSharedRef<SWidget> &)> CountSlots;
    CountSlots = [&CountSlots](const TSharedRef<SWidget> &Node)
    {
        int32 Count = Node->GetType() == TEXT("SBBBPlayerItemSlot") ? 1 : 0;
        FChildren *Children = Node->GetChildren();
        for (int32 Index = 0; Index < Children->Num(); ++Index)
        {
            Count += CountSlots(Children->GetChildAt(Index));
        }
        return Count;
    };
    const TSharedPtr<SWidget> PanelSlots = FindTagged(Widget, TEXT("Backpack.PanelSlots"));
    if (!TestTrue(TEXT("右侧物品展示面板存在"), PanelSlots.IsValid()))
    {
        return false;
    }
    int32 ConsumableStart = 0;
    int32 ConsumableCount = 0;
    Controller->GetConsumableQuickRange(ConsumableStart, ConsumableCount);
    const auto PressNumber = [&](FKey Key, int32 ExpectedSlot)
    {
        TestTrue(TEXT("数字键由背包界面处理"),
            Widget->OnPreviewKeyDown(FGeometry(), FKeyEvent(Key, FModifierKeysState(), 0, false, 0, 0)).IsEventHandled());
        Character->Tick(1.0f / 60.0f);
        TestEqual(TEXT("角色消费输入后选中当前右侧快捷位"), Controller->GetSelectedItemSlot(), ExpectedSlot);
    };
    TestEqual(TEXT("默认右侧显示武器快捷位"), CountSlots(PanelSlots.ToSharedRef()), Controller->GetQuickAccessSlotCount());
    Click(TEXT("Backpack.Panel.2"));
    TestEqual(TEXT("消耗品页显示真实快捷位数量"), CountSlots(PanelSlots.ToSharedRef()), ConsumableCount);
    PressNumber(EKeys::Two, ConsumableStart + 1);
    Click(TEXT("Backpack.Storage.1"));
    PressNumber(EKeys::Three, ConsumableStart + 2);
    TestFalse(TEXT("普通消耗品存储位不能直接作为快捷目标"), View->SelectSlot(ConsumableStart + ConsumableCount));
    Click(TEXT("Backpack.Panel.1"));
    TestEqual(TEXT("角色页显示全部身体穿戴位置"), CountSlots(PanelSlots.ToSharedRef()), Character->GetCharacterConfig().Item.WearSlots.Num());
    const int32 BeforeCharacterKey = Controller->GetSelectedItemSlot();
    Widget->OnPreviewKeyDown(FGeometry(), FKeyEvent(EKeys::Four, FModifierKeysState(), 0, false, 0, 0));
    Character->Tick(1.0f / 60.0f);
    TestEqual(TEXT("角色页数字键不误选武器或消耗品"),
        Controller->GetSelectedItemSlot(), BeforeCharacterKey);
    Click(TEXT("Backpack.Panel.0"));
    Click(TEXT("Backpack.Storage.3"));
    PressNumber(EKeys::Two, 1);
    Click(TEXT("Backpack.Storage.0"));
    Controller->SubmitItemSelect(INDEX_NONE);
    Character->Tick(1.0f / 60.0f);
    UBBBPlayerItemPortrait *Portrait = NewObject<UBBBPlayerItemPortrait>(Controller);
    TestTrue(TEXT("建立穿脱回归捕获"), Portrait->Open(Character));
    Controller->SubmitItemAdd(TEXT("Helmet_UASoldHelmet01"));
    Character->Tick(1.0f / 60.0f);
    const FGuid Helmet = Controller->GetItemInstanceId(20);
    TestTrue(TEXT("头盔登记真实身份"), Helmet.IsValid());
    Controller->SubmitItemMove(20, 0, Helmet);
    Character->Tick(1.0f / 60.0f);
    TestEqual(TEXT("头盔与快捷武器交换被拒绝"), Controller->GetItemInstanceId(20), Helmet);
    TestEqual(TEXT("非法交换保留快捷武器"), Controller->GetItemInstanceId(0), First);
    TestEqual(TEXT("头盔双击目标为对应穿戴位置"), Controller->GetItemDisplayData(20).EquipSlot, 100);
    TestTrue(TEXT("双击穿戴请求接受"), View->EquipItem(20, Character, Helmet));
    Character->Tick(1.0f / 60.0f);
    Portrait->Update(1.0f);
    TestTrue(TEXT("穿戴后捕获真实位置"), Controller->GetItemInstanceId(100) == Helmet);
    Controller->SubmitItemAdd(TEXT("Helmet_UASoldHelmet00"));
    Character->Tick(1.0f / 60.0f);
    const FGuid ReplacementHelmet = Controller->GetItemInstanceId(20);
    TestTrue(TEXT("双击替换头盔请求接受"), View->EquipItem(20, Character, ReplacementHelmet));
    Character->Tick(1.0f / 60.0f);
    TestEqual(TEXT("新头盔进入穿戴位"), Controller->GetItemInstanceId(100), ReplacementHelmet);
    TestEqual(TEXT("旧头盔回到替换来源格"), Controller->GetItemInstanceId(20), Helmet);
    Controller->SubmitItemMove(100, 20, ReplacementHelmet);
    Character->Tick(1.0f / 60.0f);
    Portrait->Update(1.0f);
    Controller->SubmitItemMove(100, 21, Helmet);
    Character->Tick(1.0f / 60.0f);
    Portrait->Update(1.0f);
    TestTrue(TEXT("脱下清空网格后捕获安全"), Controller->GetItemInstanceId(21) == Helmet
        && !Controller->GetItemInstanceId(100).IsValid());
    Portrait->Close();
    TestTrue(TEXT("控制器提供物品展示数据"), Controller->GetItemDisplayData(0).bOccupied
        && !Controller->GetItemDisplayData(0).Name.IsEmpty());
    TestFalse(TEXT("普通槽位不能由界面直接选中"), View->SelectSlot(Controller->GetQuickAccessSlotCount()));
    TestTrue(TEXT("界面提交快捷选择"), View->SelectSlot(0));
    TestEqual(TEXT("界面不提前修改选择"), Controller->GetSelectedItemSlot(), INDEX_NONE);
    Character->Tick(1.0f / 60.0f);
    TestEqual(TEXT("界面读取角色处理后的选择"), Controller->GetSelectedItemSlot(), 0);
    TestTrue(TEXT("控制器区分实际装备状态"), Controller->GetItemDisplayData(0).bActive);
    const TSharedRef<SBBBCombatHud> Hud = SNew(SBBBCombatHud).View(View);
    const FGeometry HudGeometry = FGeometry::MakeRoot(FVector2f(1920.0f, 1080.0f), FSlateLayoutTransform());
    FHittestGrid HudHitGrid;
    const FPaintArgs HudArgs(nullptr, HudHitGrid, FVector2D::ZeroVector, 0.0, 0.0f);
    TestFalse(TEXT("持枪未瞄准时准星仍隐藏"), Controller->ShouldShowAimHud());
    FSlateWindowElementList HipFireHud(nullptr);
    Hud->OnPaint(HudArgs, HudGeometry, FSlateRect(0.0f, 0.0f, 1920.0f, 1080.0f),
        HipFireHud, 0, FWidgetStyle(), true);
    const auto &AmmoTexts = HipFireHud.GetUncachedDrawElements().Get<(uint8)EElementType::ET_Text>();
    if (TestEqual(TEXT("未瞄准也绘制剩余弹药与容量"), AmmoTexts.Num(), 2))
    {
        int32 Loaded = 0;
        int32 Capacity = 0;
        bool bContinuous = false;
        const ABBBEquipment *Held = Cast<ABBBEquipment>(Controller->GetActiveItem());
        TestTrue(TEXT("弹药绘制读取真实枪械"), Held && Held->ReadAmmoDisplay(Loaded, Capacity, bContinuous));
        TestEqual(TEXT("右下数字保持真实剩余弹药"), FString(AmmoTexts[0].GetText()), FString::FromInt(Loaded));
        TestEqual(TEXT("右下数字保持真实容量"), FString(AmmoTexts[1].GetText()), FString::Printf(TEXT("/ %d"), Capacity));
        for (const FSlateTextElement &Text : AmmoTexts)
        {
            const FVector2f Position = Text.GetPosition();
            TestTrue(TEXT("弹药数字固定在右下角"), Position.X > 1600.0f && Position.Y > 950.0f);
        }
    }
    ABBBEquipment *HeldEquipment = Cast<ABBBEquipment>(Controller->GetActiveItem());
    FTransform Muzzle;
    if (!TestTrue(TEXT("真实手持枪械提供枪口"), HeldEquipment && HeldEquipment->TryGetMuzzleTransform(Muzzle)))
    {
        return false;
    }
    const FVector RayStart = Muzzle.GetLocation();
    const FVector RayDirection = Muzzle.GetUnitAxis(EAxis::X);
    UBBBMassSubsystem *Mass = World->GetSubsystem<UBBBMassSubsystem>();
    if (!TestNotNull(TEXT("枪口落点使用公开实体碰撞"), Mass))
    {
        return false;
    }
    FMassEntityManager &Manager = World->GetSubsystem<UMassEntitySubsystem>()->GetMutableEntityManager();
    const FMassArchetypeHandle AimArchetype = Manager.CreateArchetype({FTransformFragment::StaticStruct()});
    const FMassEntityHandle NearEntity = Manager.CreateEntity(AimArchetype);
    const FMassEntityHandle FarEntity = Manager.CreateEntity(AimArchetype);
    FBBBMassCollisionBody NearBody;
    NearBody.Entity = NearEntity;
    NearBody.Center = RayStart + RayDirection * 2000.0;
    NearBody.Radius = 25.0f;
    FBBBMassCollisionBody FarBody = NearBody;
    FarBody.Entity = FarEntity;
    FarBody.Center = RayStart + RayDirection * 4000.0;
    Mass->AddCollisionBody(FarBody);
    Mass->AddCollisionBody(NearBody);
    FVector AimImpact;
    TestTrue(TEXT("无表现演员的实体仍能提供枪口落点"), Controller->GetActualAimWorldPosition(AimImpact));
    TestTrue(TEXT("白圈落在最近实体表面"), AimImpact.Equals(RayStart + RayDirection * 1975.0, 0.1));
    AActor *AimWall = World->SpawnActor<AActor>();
    UBoxComponent *AimWallBox = NewObject<UBoxComponent>(AimWall);
    AimWall->SetRootComponent(AimWallBox);
    AimWallBox->SetBoxExtent(FVector(15.0, 200.0, 200.0));
    AimWallBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    AimWallBox->SetCollisionResponseToAllChannels(ECR_Ignore);
    AimWallBox->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
    AimWallBox->RegisterComponent();
    AimWall->SetActorRotation(Muzzle.GetRotation());
    AimWall->SetActorLocation(RayStart + RayDirection * 1000.0);
    Controller->GetActualAimWorldPosition(AimImpact);
    TestTrue(TEXT("墙前遮挡优先于墙后僵尸"), AimImpact.Equals(RayStart + RayDirection * 985.0, 0.1));
    AimWall->SetActorLocation(RayStart + RayDirection * 7000.0);
    Controller->GetActualAimWorldPosition(AimImpact);
    TestTrue(TEXT("僵尸后方墙面不覆盖实体落点"), AimImpact.Equals(RayStart + RayDirection * 1975.0, 0.1));
    Mass->BeginCollisionFrame();
    NearBody.Center = RayStart + RayDirection * 2300.0;
    Mass->AddCollisionBody(NearBody);
    Controller->GetActualAimWorldPosition(AimImpact);
    TestTrue(TEXT("白圈跟随实体新位置"), AimImpact.Equals(RayStart + RayDirection * 2275.0, 0.1));
    Mass->BeginCollisionFrame();
    NearBody.Center += Muzzle.GetUnitAxis(EAxis::Y) * 250.0;
    Mass->AddCollisionBody(NearBody);
    Controller->GetActualAimWorldPosition(AimImpact);
    TestTrue(TEXT("目标移出射线后白圈立即回到场景命中"), AimImpact.Equals(RayStart + RayDirection * 6985.0, 0.1));
    Mass->BeginCollisionFrame();
    AimWallBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    Controller->GetActualAimWorldPosition(AimImpact);
    TestTrue(TEXT("目标与遮挡移除后不残留旧白圈位置"), AimImpact.Equals(RayStart + RayDirection * 100000.0, 0.1));
    Manager.DestroyEntity(NearEntity);
    Manager.DestroyEntity(FarEntity);
    AimWall->Destroy();
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
    FSlateWindowElementList UnarmedHud(nullptr);
    Hud->OnPaint(HudArgs, HudGeometry, FSlateRect(0.0f, 0.0f, 1920.0f, 1080.0f),
        UnarmedHud, 0, FWidgetStyle(), true);
    TestEqual(TEXT("空手时不显示旧枪械弹药"),
        UnarmedHud.GetUncachedDrawElements().Get<(uint8)EElementType::ET_Text>().Num(), 0);
    TestFalse(TEXT("空手时不保留枪口落点"), Controller->GetActualAimWorldPosition(AimImpact));
    TestEqual(TEXT("快捷栏使用前五格"), Controller->GetQuickAccessSlotCount(), 5);
    TestEqual(TEXT("四类背包和身体穿戴共一百一十格"), Controller->GetItemSlotCount(), 110);
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
