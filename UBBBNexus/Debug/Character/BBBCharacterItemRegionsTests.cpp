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

/** 验证四类容量和原子穿戴边界 */
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBBBCharacterItemRegionsTest, "BBB.Character.ItemRegions",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FBBBCharacterItemRegionsTest::RunTest(const FString &Parameters)
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

    Config->Item.WearableCatalog = NewObject<UBBBWearableCatalog>(Config);
    for (FName Slot : {FName(TEXT("Helmet")), FName(TEXT("Flag"))})
    {
        auto *Definition = NewObject<UBBBItemDefinition>(Config);
        Definition->ItemId = Slot;
        Definition->ItemType = EBBBItemType::Wearable;
        Definition->WearSlot = Slot;
        Config->Item.WearableCatalog->Items.AddDefaulted_GetRef().Definition = Definition;
    }
    Step();
    const auto &Inventory = Character->RuntimeData.Item.ReadCharacterItemInventoryState();
    const auto &Operations = Character->RuntimeData.Item.ReadCharacterItemOperationState();
    TestEqual(TEXT("四类背包总容量"), Inventory.BackpackSlotCount, 100);
    TestEqual(TEXT("穿戴位置独立于背包"), Inventory.Slots.Num(), 110);
    TestEqual(TEXT("装备容量包含快捷位置"), Inventory.EquipmentSlotCount, 20);
    TestEqual(TEXT("消耗品容量包含快捷位置"), Inventory.ConsumableSlotCount, 30);
    Character->SubmitInput(FBBBCharacterItemAddLocalControlPacket{{TEXT("Helmet")}, {20}});
    Step();
    const int32 WearStart = Inventory.EquipmentSlotCount;
    const int32 HelmetSlot = Inventory.BackpackSlotCount;
    const FGuid Helmet = Inventory.Slots[WearStart].InstanceId;
    TestEqual(TEXT("穿戴物具有独立存储区域"), Inventory.Slots[WearStart].Definition->ItemType, EBBBItemType::Wearable);
    TestNull(TEXT("穿戴物不占装备快捷位置"), Inventory.Slots[0].Definition.Get());
    Character->SubmitInput(FBBBCharacterItemMoveLocalControlPacket{{WearStart}, {0}, {Helmet}});
    Step();
    TestEqual(TEXT("穿戴物不能进入装备位置"), Operations.RejectedCount, 1);
    Character->SubmitInput(FBBBCharacterItemMoveLocalControlPacket{{WearStart}, {HelmetSlot + 1}, {Helmet}});
    Step();
    TestEqual(TEXT("穿戴位置类型不匹配时原子拒绝"), Operations.RejectedCount, 1);
    Character->SubmitInput(FBBBCharacterItemMoveLocalControlPacket{{WearStart}, {HelmetSlot}, {Helmet}});
    Step();
    TestEqual(TEXT("穿戴移动同一个身份"), Inventory.Slots[HelmetSlot].InstanceId, Helmet);
    TestNull(TEXT("穿戴释放存储格"), Inventory.Slots[WearStart].Definition.Get());
    Character->SubmitInput(FBBBCharacterItemAddLocalControlPacket{{TEXT("Helmet")}, {1}});
    Step();
    Character->SubmitInput(FBBBCharacterItemMoveLocalControlPacket{{HelmetSlot}, {INDEX_NONE}, {Helmet}});
    Step();
    TestEqual(TEXT("满包脱下拒绝"), Operations.RejectedCount, 1);
    TestEqual(TEXT("拒绝不丢失穿戴实例"), Inventory.Slots[HelmetSlot].InstanceId, Helmet);
    Character->SubmitInput(FBBBCharacterItemMoveLocalControlPacket{{HelmetSlot}, {WearStart}, {Helmet}});
    Step();
    TestEqual(TEXT("满包合法交换仍成立"), Inventory.Slots[WearStart].InstanceId, Helmet);
    Character->SubmitInput(FBBBCharacterItemMoveLocalControlPacket{{HelmetSlot}, {WearStart}, {Helmet}});
    Step();
    TestEqual(TEXT("过期拖动拒绝"), Operations.RejectedCount, 1);
    Character->SubmitInput(FBBBCharacterItemSelectLocalControlPacket{{40}});
    Step();
    TestEqual(TEXT("空消耗品格保留选择"), Character->RuntimeData.Item.ReadCharacterItemBarState().SelectedConsumableSlot, 0);
    TestNull(TEXT("空消耗品格不生成持有实例"), Character->RuntimeData.Item.ReadCharacterItemBarState().DesiredMainHandItem.Get());
    TSet<FGuid> Identities;
    for (const auto &Item : Inventory.Slots)
    {
        if (Item.Definition)
        {
            TestFalse(TEXT("实例身份只有一个位置"), Identities.Contains(Item.InstanceId));
            Identities.Add(Item.InstanceId);
        }
    }
    TestEqual(TEXT("穿戴和交换不增加物品"), Identities.Num(), 21);
    Items.Shutdown();
    return true;
}
#endif
