#include "Misc/AutomationTest.h"
#if WITH_DEV_AUTOMATION_TESTS && WITH_EDITOR
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/ItemSystem/BBBCharacterItemSystem.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/ParseSystem/BBBCharacterParseSystem.h"
#include "BBBWork/UBBBNexus/Item/Wearable/Catalog/BBBWearableCatalog.h"
#include "BBBWork/UBBBNexus/Item/Misc/Catalog/BBBMiscCatalog.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "Misc/ScopeExit.h"
#include "UObject/UnrealType.h"

#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Rifle/BBBRifleEquipment.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Rifle/Config/BBBRifleDefinition.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Rifle/Logic/System/ActionSystem/Processors/BBBRifleActionProcessor.h"

/** 验证单一物品身份 独立武器状态与整次数量获取 */
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBBBCharacterItemSystemTest, "BBB.Character.ItemSystem",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FBBBCharacterItemSystemTest::RunTest(const FString &Parameters)
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
    };
    ABBBCharacter *Character = World->SpawnActor<ABBBCharacter>();
    UBBBCharacterConfig *Config = NewObject<UBBBCharacterConfig>(Character);
    FindFProperty<FObjectProperty>(ABBBCharacter::StaticClass(), TEXT("CharacterConfigAsset"))
        ->SetObjectPropertyValue_InContainer(Character, Config);
    Character->RuntimeData.External.NetworkIdentityState.bIsMirror = false;
    FBBBCharacterItemSystem Items;
    Items.Initialize(*Character, Character->RuntimeData, Config->Item);
    FBBBCharacterParseSystem Parse;
    Parse.Initialize(Character->RuntimeData);
    const auto Step = [&]()
    {
        Parse.Update();
        Items.Update();
    };

    Config->Item.MiscSlotCount = 2;
    Config->Item.MiscCatalog = NewObject<UBBBMiscCatalog>(Config);
    auto *Definition = NewObject<UBBBItemDefinition>(Config);
    Definition->ItemId = TEXT("TestStack");
    Definition->ItemType = EBBBItemType::Misc;
    Definition->MaxStack = 4;
    Config->Item.MiscCatalog->Items.Add(Definition);
    Step();
    auto &Domain = Character->RuntimeData.Item;
    const auto &Inventory = Domain.ReadCharacterItemInventoryState();
    const auto &Operations = Domain.ReadCharacterItemOperationState();
    const int32 MiscStart = Inventory.EquipmentSlotCount + Inventory.WearableSlotCount + Inventory.ConsumableSlotCount;
    Character->SubmitInput(FBBBCharacterItemAddLocalControlPacket{{TEXT("TestStack")}, {7}});
    Step();
    TestEqual(TEXT("整次获取创建四和三两个堆叠"), Inventory.Slots[MiscStart].Quantity, 4);
    TestEqual(TEXT("第二个堆叠数量"), Inventory.Slots[MiscStart + 1].Quantity, 3);
    const FGuid FirstStack = Inventory.Slots[MiscStart].InstanceId;
    const FGuid SecondStack = Inventory.Slots[MiscStart + 1].InstanceId;
    Character->SubmitInput(FBBBCharacterItemAddLocalControlPacket{{TEXT("TestStack")}, {2}});
    Step();
    TestEqual(TEXT("空间不足整次拒绝"), Operations.RejectedCount, 1);
    TestEqual(TEXT("拒绝不先填一份"), Inventory.Slots[MiscStart + 1].Quantity, 3);
    TestEqual(TEXT("拒绝不更换实例"), Inventory.Slots[MiscStart].InstanceId, FirstStack);
    Character->SubmitInput(FBBBCharacterItemMoveLocalControlPacket{{MiscStart}, {MiscStart + 1}, {FirstStack}});
    Step();
    TestEqual(TEXT("同型号只补满目标"), Inventory.Slots[MiscStart + 1].Quantity, 4);
    TestEqual(TEXT("合并余量留在源"), Inventory.Slots[MiscStart].Quantity, 3);
    TestEqual(TEXT("源余量保留身份"), Inventory.Slots[MiscStart].InstanceId, FirstStack);
    TestEqual(TEXT("目标保留身份"), Inventory.Slots[MiscStart + 1].InstanceId, SecondStack);
    Character->SubmitInput(FBBBCharacterItemMoveLocalControlPacket{{MiscStart}, {0}, {FirstStack}});
    Step();
    TestEqual(TEXT("杂物不能成为主手装备"), Operations.RejectedCount, 1);

    auto *First = World->SpawnActor<ABBBRifleEquipment>();
    auto *Second = World->SpawnActor<ABBBRifleEquipment>();
    auto *FirstDefinition = NewObject<UBBBRifleDefinition>(Config);
    auto *SecondDefinition = NewObject<UBBBRifleDefinition>(Config);
    FirstDefinition->ItemId = TEXT("FirstRifle");
    SecondDefinition->ItemId = TEXT("SecondRifle");
    FirstDefinition->AmmoCapacity = 10;
    SecondDefinition->AmmoCapacity = 30;
    FBBBRifleActionProcessor::Initialize(First->RuntimeData, *FirstDefinition);
    FBBBRifleActionProcessor::Initialize(Second->RuntimeData, *SecondDefinition);
    Domain.ItemInventoryState.Slots[0] = {FGuid::NewGuid(), FirstDefinition, First, 1};
    Domain.ItemInventoryState.Slots[1] = {FGuid::NewGuid(), SecondDefinition, Second, 1};
    Character->SubmitInput(FBBBCharacterItemSelectLocalControlPacket{{0}});
    Step();
    TestTrue(TEXT("主手目标只引用原实例"), Domain.ReadCharacterItemBarState().DesiredMainHandItem == First);
    Character->SubmitInput(FBBBCharacterItemMoveLocalControlPacket{{0}, {1}, {Inventory.Slots[0].InstanceId}});
    Step();
    TestTrue(TEXT("交换后目标跟随原格子"), Domain.ReadCharacterItemBarState().DesiredMainHandItem == Second);
    TestEqual(TEXT("第一把武器保存弹量"), First->GetLoadedAmmo(), 10);
    TestEqual(TEXT("第二把武器保存弹量"), Second->GetLoadedAmmo(), 30);
    Character->SubmitInput(FBBBCharacterItemMoveLocalControlPacket{{0}, {5}, {Inventory.Slots[0].InstanceId}});
    Step();
    TestEqual(TEXT("空装备格取消原选择"), Domain.ReadCharacterItemBarState().SelectedSlot, INDEX_NONE);
    TestNull(TEXT("原格变空保持空手"), Domain.ReadCharacterItemBarState().DesiredMainHandItem.Get());
    const int32 Revision = Operations.Revision;
    Step();
    TestEqual(TEXT("已消费操作不重放"), Operations.Revision, Revision);
    Character->RuntimeData.External.NetworkIdentityState.bIsMirror = true;
    Character->SubmitInput(FBBBCharacterItemAddLocalControlPacket{{TEXT("TestStack")}, {1}});
    Parse.Update();
    TestTrue(TEXT("镜像不维护真实数量"), Operations.PendingItemIds.IsEmpty());
    Items.Shutdown();
    TestTrue(TEXT("关闭清理唯一物品数组"), Inventory.Slots.IsEmpty());
    TestFalse(TEXT("关闭销毁第一把真实装备"), IsValid(First));
    TestFalse(TEXT("关闭销毁第二把真实装备"), IsValid(Second));
    return true;
}
#endif
