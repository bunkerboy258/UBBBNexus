#include "Misc/AutomationTest.h"
#if WITH_DEV_AUTOMATION_TESTS && WITH_EDITOR
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "BBBWork/UBBBNexus/Character/Input/LocalControl/Interaction/FBBBCharacterInteractionReleaseLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/LocalControl/Interaction/FBBBCharacterInteractionFinishLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Base/BBBConsumable.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Throwable/Grenade/BBBGrenadeConsumable.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Throwable/Grenade/Config/BBBGrenadeDefinition.h"
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
#include "EngineUtils.h"
#include "Kismet/GameplayStatics.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Projectile/BBBThrowable.h"
#include "Misc/ScopeExit.h"
#include "Components/BoxComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "BBBWork/UBBBNexus/Character/Input/LocalControl/Interaction/FBBBCharacterQuickMeleeLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/LocalControl/Interaction/FBBBCharacterMeleeBeginLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/LocalControl/Interaction/FBBBCharacterMeleeEndLocalControlPacket.h"

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
    Controller->SubmitItemAdd(TEXT("Grenade_Stun"));
    Step();
    int32 Start = 0;
    int32 Count = 0;
    Controller->GetConsumableQuickRange(Start, Count);
    TestEqual(TEXT("三个消耗品快捷位置"), Count, 3);
    TestEqual(TEXT("投掷物进入第一个位置"), Controller->GetItemDisplayData(Start).Quantity, 4);
    TestNull(TEXT("库存消耗品没有手持 Actor"),
             Character->RuntimeData.Item.ReadCharacterItemInventoryState().Slots[Start].HeldInstance.Get());
    TestNull(TEXT("未使用时没有消耗品运行时"), Character->RuntimeData.Item.ReadCharacterItemUseState().Runtime.Get());
    TestEqual(TEXT("进场自动选择一号位"), Controller->GetSelectedItemSlot(), 0);
    Controller->SubmitItemMove(Start + 1, Start + 2, Controller->GetItemInstanceId(Start + 1));
    Step();
    TestEqual(TEXT("后续投掷物位于第三格"), Controller->GetItemDisplayData(Start + 2).Quantity, 1);
    AActor *Weapon = Character->GetHeldItem();
    if (!TestNotNull(TEXT("装备主手有效"), Weapon))
    {
        return false;
    }
    Controller->SubmitItemSelect(Start);
    Step();
    TestEqual(TEXT("选择手雷保持装备主手"), Character->GetHeldItem(), Weapon);
    const auto CountThrowables = [World]()
    {
        int32 Result = 0;
        for (TActorIterator<ABBBThrowable> It(World); It; ++It)
        {
            ++Result;
        }
        return Result;
    };
    const auto Release = [Character]()
    {
        const auto &Use = Character->RuntimeData.Interaction.ReadInteractionUseState();
        Character->SubmitInput(FBBBCharacterInteractionReleaseLocalControlPacket{
            {Use.Runtime.Get()}, {static_cast<uint64>(Use.UseSerial)}, {Use.UseSerial}});
    };
    const auto Finish = [Character]()
    {
        const auto &Use = Character->RuntimeData.Interaction.ReadInteractionUseState();
        Character->SubmitInput(FBBBCharacterInteractionFinishLocalControlPacket{
            {Use.Runtime.Get()}, {static_cast<uint64>(Use.UseSerial)}, {Use.UseSerial}, {Use.Instance}});
    };
    const int32 Baseline = CountThrowables();
    for (int32 Index = 0; Index < 4; ++Index)
    {
        Controller->SubmitQuickUse();
        Step();
        const auto &Use = Character->RuntimeData.Interaction.ReadInteractionUseState();
        if (!TestNotNull(TEXT("使用期间创建轻量对象"), Character->GetActiveConsumable()))
        {
            return false;
        }
        TestTrue(TEXT("使用占用已经成立"), Use.bBusy);
        Controller->SubmitItemSelect(INDEX_NONE);
        Controller->SubmitItemMove(Start, Start + 2, Controller->GetItemInstanceId(Start));
        Step();
        TestEqual(TEXT("使用期间拒绝装备切换"), Character->GetHeldItem(), Weapon);
        TestEqual(TEXT("使用期间拒绝库存移动"), Controller->GetItemDisplayData(Start + 2).Quantity, 1);
        Release();
        Release();
        Step();
        Step();
        TestEqual(TEXT("重复释放只生成一次世界实体"), CountThrowables(), Baseline + Index + 1);
        TestEqual(TEXT("每次只扣除一枚"), Controller->GetItemDisplayData(Start).Quantity, 3 - Index);
        if (Index == 3)
        {
            TestTrue(TEXT("零数量仍保留至使用结束"),
                     Character->RuntimeData.Item.ReadCharacterItemInventoryState().Slots[Start].InstanceId.IsValid());
            TestEqual(TEXT("动作结束前不提前切换"),
                      Character->RuntimeData.Item.ReadCharacterItemBarState().SelectedThrowableSlot, 0);
        }
        Finish();
        Step();
        Step();
        TestFalse(TEXT("完成后解除使用占用"), Character->RuntimeData.Interaction.ReadInteractionUseState().bBusy);
        TestNull(TEXT("完成后回收当次对象"), Character->RuntimeData.Item.ReadCharacterItemUseState().Runtime.Get());
        TestEqual(TEXT("耗尽不清空有效装备"), Character->GetHeldItem(), Weapon);
        TestEqual(TEXT("结束不销毁已投出的世界实体"), CountThrowables(), Baseline + Index + 1);
        TestEqual(TEXT("最后一枚完成后跳过空格"),
                  Controller->GetSelectedThrowableSlot(), Index == 3 ? 2 : 0);
        TestEqual(TEXT("自动选择后不自动使用下一格"), Controller->GetItemDisplayData(Start + 2).Quantity, 1);
    }
    auto *Target = World->SpawnActor<AActor>();
    auto *Collision = NewObject<UBoxComponent>(Target);
    Collision->SetBoxExtent(FVector(8.0f));
    Collision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    Collision->SetCollisionResponseToAllChannels(ECR_Block);
    Target->SetRootComponent(Collision);
    Target->SetCanBeDamaged(true);
    Target->SetActorLocation(Character->GetMesh()->GetSocketLocation(TEXT("hand_r")) +
                             Character->GetActorForwardVector() * 25.0f);
    Collision->RegisterComponent();
    Character->SubmitInput(FBBBCharacterQuickMeleeLocalControlPacket{});
    Step();
    const auto &Melee = Character->RuntimeData.Interaction.ReadInteractionUseState();
    TestTrue(TEXT("快速近战建立独立使用占用"), Melee.bBusy && Melee.Fact.bQuickMelee);
    TestFalse(TEXT("快速近战不沿用上次消耗品身份"), Melee.Instance.IsValid());
    TestTrue(TEXT("未收到开窗通知不能命中"), Melee.MeleeActors.IsEmpty());
    Character->SubmitInput(FBBBCharacterMeleeBeginLocalControlPacket{{Melee.UseSerial}});
    Character->SubmitInput(FBBBCharacterMeleeBeginLocalControlPacket{{Melee.UseSerial}});
    Step();
    Step();
    TestEqual(TEXT("同帧重复开窗和连续帧只记录一个靶标"), Melee.MeleeActors.Num(), 1);
    Character->SubmitInput(FBBBCharacterMeleeEndLocalControlPacket{{Melee.UseSerial + 1}});
    Step();
    TestTrue(TEXT("旧窗口结束不能关闭当前近战"), Melee.bMeleeContact);
    Character->SubmitInput(FBBBCharacterMeleeEndLocalControlPacket{{Melee.UseSerial}});
    Step();
    TestFalse(TEXT("当前通知结束关闭命中窗口"), Melee.bMeleeContact);
    Character->SubmitInput(FBBBCharacterInteractionFinishLocalControlPacket{
        {Character}, {static_cast<uint64>(Melee.UseSerial)}, {Melee.UseSerial}, {Melee.Instance}});
    Step();
    TestFalse(TEXT("近战结束解除使用占用"), Melee.bBusy);
    TestEqual(TEXT("快速近战保持装备主手"), Character->GetHeldItem(), Weapon);
    return !HasAnyErrors();
}
#endif
