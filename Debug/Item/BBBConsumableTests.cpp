#include "Misc/AutomationTest.h"
#if WITH_DEV_AUTOMATION_TESTS && WITH_EDITOR
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "BBBWork/UBBBNexus/Character/Input/LocalControl/Interaction/FBBBCharacterInteractionReleaseLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/LocalControl/Interaction/FBBBCharacterInteractionFinishLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Base/BBBConsumable.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Rifle/BBBRifleEquipment.h"
#include "BBBWork/UBBBNexus/Character/Input/LocalControl/Interaction/FBBBCharacterInteractionSecondaryLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Base/Input/LocalControl/Fire/FBBBEquipmentBlockFireLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Base/Input/LocalControl/Fire/FBBBEquipmentAllowFireLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Player/BBBPlayerController.h"
#include "BBBWork/UBBBNexus/Player/BBBPlayerItemDisplayData.h"
#include "Engine/Engine.h"
#include "Engine/LocalPlayer.h"
#include "Engine/World.h"
#include "GameFramework/PlayerState.h"
#include "MassEntitySubsystem.h"
#include "Misc/ScopeExit.h"

/** 真实角色和物品实例验证释放幂等 零数量生命周期及自动返回 */
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBBBConsumableTest, "BBB.Character.Consumable",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FBBBConsumableTest::RunTest(const FString &)
{
    const auto Options = UWorld::InitializationValues().AllowAudioPlayback(false).CreatePhysicsScene(true)
        .CreateNavigation(false).CreateAISystem(false).ShouldSimulatePhysics(false);
    UWorld *World = UWorld::CreateWorld(EWorldType::Game, false, NAME_None, nullptr, true, ERHIFeatureLevel::Num, &Options);
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
    if (!TestNotNull(TEXT("正式角色类"), CharacterClass))
    {
        return false;
    }
    auto *Character = World->SpawnActor<ABBBCharacter>(CharacterClass);
    auto *Controller = World->SpawnActor<ABBBPlayerController>();
    auto *Player = NewObject<ULocalPlayer>(GEngine);
    Player->PlayerController = Controller;
    Controller->Player = Player;
    Controller->SetPlayerState(World->SpawnActor<APlayerState>());
    Controller->PlayerState->SetPlayerId(17);
    Controller->Possess(Character);
    World->InitializeActorsForPlay(FURL());
    World->SetBegunPlay(true);
    Character->DispatchBeginPlay();
    auto &Manager = World->GetSubsystem<UMassEntitySubsystem>()->GetMutableEntityManager();
    const auto Step = [Character]()
    {
        Character->Tick(1.0f / 60.0f);
        const auto &Inventory = Character->RuntimeData.Item.ReadCharacterItemInventoryState();
        for (const auto &Item : Inventory.Slots)
        {
            if (IsValid(Item.HeldInstance) && Item.HeldInstance->IsActorTickEnabled())
            {
                Item.HeldInstance->Tick(1.0f / 60.0f);
            }
        }
    };
    Controller->SubmitItemAdd(TEXT("ModernWeapons_Rifle_01"));
    Controller->SubmitItemAdd(TEXT("Grenade_01"), 4);
    Step();
    int32 Start = 0;
    int32 Count = 0;
    Controller->GetConsumableQuickRange(Start, Count);
    TestEqual(TEXT("独立消耗品快捷栏"), Count, 5);
    TestEqual(TEXT("每格四枚"), Controller->GetItemDisplayData(Start).Quantity, 4);
    AActor *Stored = Character->RuntimeData.Item.ReadCharacterItemInventoryState().Slots[Start].HeldInstance;
    if (!TestNotNull(TEXT("获得堆叠时已建立实例"), Stored))
    {
        return false;
    }
    TestTrue(TEXT("存放时隐藏且关闭更新"), Stored->IsHidden() && !Stored->IsActorTickEnabled());
    Controller->SubmitItemSelect(0);
    Step();
    AActor *Original = Character->GetHeldItem();
    if (!TestNotNull(TEXT("原装备已持有"), Original))
    {
        return false;
    }

    Controller->SubmitItemAdd(TEXT("ModernWeapons_Rifle_01"));
    Step();
    auto *Rifle = Cast<ABBBRifleEquipment>(Original);
    if (!TestNotNull(TEXT("步枪切换权限验证实例"), Rifle))
    {
        return false;
    }
    Controller->SubmitPrimaryUse();
    Step();
    Step();
    Character->SubmitInput(FBBBCharacterInteractionSecondaryLocalControlPacket{});
    Step();
    Step();
    TestTrue(TEXT("步枪实际进入换弹"), Rifle->RuntimeData.Action.ReadRifleActionState().bIsReloading);
    const int32 AmmoBeforeSwitch = Rifle->GetLoadedAmmo();
    Rifle->SubmitInput(FBBBEquipmentBlockFireLocalControlPacket{});
    Step();
    Step();
    TestTrue(TEXT("换弹及禁火不能禁止普通切枪"), Rifle->CanSwitch());
    TestFalse(TEXT("换弹期间仍拒绝 G 快速使用"), Rifle->CanQuickUse());
    Controller->SubmitQuickUse();
    Step();
    TestTrue(TEXT("G 冲突请求保留原步枪"), Character->GetHeldItem() == Original);
    Controller->SubmitItemSelect(INDEX_NONE);
    Step();
    TestNull(TEXT("换弹期间可以实际解除装备"), Character->GetHeldItem());
    TestFalse(TEXT("卸下完成后终止旧换弹"), Rifle->RuntimeData.Action.ReadRifleActionState().bIsReloading);
    TestEqual(TEXT("卸下不能重置已有弹药"), Rifle->GetLoadedAmmo(), AmmoBeforeSwitch);
    Controller->SubmitItemSelect(0);
    Step();
    Step();
    Rifle->SubmitInput(FBBBEquipmentAllowFireLocalControlPacket{});
    Character->SubmitInput(FBBBCharacterInteractionSecondaryLocalControlPacket{});
    Step();
    Step();
    TestTrue(TEXT("切枪验证重新进入换弹"), Rifle->RuntimeData.Action.ReadRifleActionState().bIsReloading);
    Controller->SubmitItemSelect(1);
    Step();
    Step();
    TestTrue(TEXT("换弹期间可以实际切到另一支枪"), Character->GetHeldItem() && Character->GetHeldItem() != Original);
    Controller->SubmitItemSelect(INDEX_NONE);
    Step();
    TestNull(TEXT("换弹后的持有关系可以实际解除"), Character->GetHeldItem());
    Controller->SubmitItemSelect(0);
    Step();
    Step();
    TestTrue(TEXT("重新持有原步枪实例"), Character->GetHeldItem() == Original);
    TestFalse(TEXT("重新持有不能继续旧换弹"), Rifle->RuntimeData.Action.ReadRifleActionState().bIsReloading);
    Rifle->SubmitInput(FBBBEquipmentAllowFireLocalControlPacket{});
    Step();
    Step();
    const int32 Baseline = FMath::Max(0, Manager.DebugGetEntityCount());
    for (int32 Use = 0; Use < 4; ++Use)
    {
        Controller->SubmitQuickUse();
        for (int32 Frame = 0; Frame < 5; ++Frame)
        {
            Step();
        }
        auto *Consumable = Cast<ABBBConsumable>(Character->GetHeldItem());
        if (!TestTrue(TEXT("快速使用只切出原堆叠并开始动作"), Consumable == Stored && Consumable->IsUsing()))
        {
            return false;
        }
        const uint64 Generation = Character->GetItemGeneration();
        const int32 Serial = Consumable->GetUseSerial();
        const FGuid Identity = Character->GetHeldItemIdentity();
        Character->SubmitInput(FBBBCharacterInteractionReleaseLocalControlPacket{{Consumable}, {Generation}, {Serial + 1}});
        Step();
        TestEqual(TEXT("过期通知不得消耗"), Character->GetHeldItemQuantity(), 4 - Use);
        Character->SubmitInput(FBBBCharacterInteractionReleaseLocalControlPacket{{Consumable}, {Generation}, {Serial}});
        Step();
        Step();
        TestEqual(TEXT("成功释放后仅扣一枚"), Character->GetHeldItemQuantity(), 3 - Use);
        TestEqual(TEXT("释放后独立实体只出生一次"), Manager.DebugGetEntityCount(), Baseline + Use + 1);
        Character->SubmitInput(FBBBCharacterInteractionReleaseLocalControlPacket{{Consumable}, {Generation}, {Serial}});
        Controller->SubmitItemSelect(0);
        Step();
        TestTrue(TEXT("收手前拒绝主动切换并保留零数量实例"), Character->GetHeldItem() == Consumable);
        TestEqual(TEXT("重复释放不重复扣减"), Character->GetHeldItemQuantity(), 3 - Use);
        Character->SubmitInput(FBBBCharacterInteractionFinishLocalControlPacket{{Consumable}, {Generation}, {Serial}, {Identity}});
        Step();
        Step();
        TestTrue(TEXT("完成后恢复原装备实例"), Character->GetHeldItem() == Original);
        TestEqual(TEXT("结束不回收已投出的独立实体"), Manager.DebugGetEntityCount(), Baseline + Use + 1);
    }
    TestFalse(TEXT("最后一枚收手后销毁持有实例"), IsValid(Stored));
    TestFalse(TEXT("耗尽后原位置为空"), Controller->GetItemDisplayData(Start).bOccupied);
    TestEqual(TEXT("耗尽后保留消耗品选择"), Controller->GetSelectedConsumableSlot(), 0);
    Controller->SubmitQuickUse();
    Step();
    TestTrue(TEXT("空位置快速使用不改变原装备"), Character->GetHeldItem() == Original);
    return !HasAnyErrors();
}
#endif
