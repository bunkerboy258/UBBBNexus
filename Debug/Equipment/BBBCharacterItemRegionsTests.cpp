#if WITH_DEV_AUTOMATION_TESTS && WITH_EDITOR

#include "Misc/AutomationTest.h"
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/ItemSystem/BBBCharacterItemSystem.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/ParseSystem/BBBCharacterParseSystem.h"
#include "BBBWork/UBBBNexus/Item/Catalog/BBBItemCatalog.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "Misc/ScopeExit.h"
#include "UObject/UnrealType.h"

/** 验证分类容量 原子穿戴 满包脱下与过期拖动的真实实例边界 */
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBBBCharacterItemRegionsTest, "BBB.Character.ItemRegions",
                                 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FBBBCharacterItemRegionsTest::RunTest(const FString &Parameters)
{
    const auto Options = UWorld::InitializationValues()
                             .AllowAudioPlayback(false)
                             .CreatePhysicsScene(true)
                             .CreateNavigation(false)
                             .CreateAISystem(false)
                             .ShouldSimulatePhysics(false);
    UWorld *World =
        UWorld::CreateWorld(EWorldType::Game, false, NAME_None, nullptr, true, ERHIFeatureLevel::Num, &Options);
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
    Config->Item.Catalog = NewObject<UBBBItemCatalog>(Config);
    for (const FName Slot : {FName(TEXT("Helmet")), FName(TEXT("Flag"))})
    {
        UBBBItemDefinition *Definition = NewObject<UBBBItemDefinition>(Config);
        Definition->ItemId = Slot;
        Definition->WearSlot = Slot;
        Definition->ItemType = Slot == TEXT("Flag") ? EBBBItemType::Misc : EBBBItemType::Wearable;
        FBBBItemCatalogEntry &Entry = Config->Item.Catalog->Items.AddDefaulted_GetRef();
        Entry.Definition = Definition;
    }
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
    Step();
    const auto &Inventory = Character->RuntimeData.Item.ReadItemInventoryState();
    const auto &Operations = Character->RuntimeData.Item.ReadItemOperationState();
    TestEqual(TEXT("五快捷 二十装备 三十杂物 八穿戴"), Inventory.Slots.Num(), 63);
    for (int32 Index = 0; Index < 26; ++Index)
    {
        Character->SubmitInput(FBBBItemAddLocalControlPacket{{TEXT("Helmet")}});
    }
    Step();
    TestEqual(TEXT("穿戴品只使用二十个普通装备格"), Operations.SucceededCount, 20);
    TestEqual(TEXT("穿戴品满包不能借用快捷或杂物格"), Operations.RejectedCount, 6);
    for (int32 Index = 0; Index < Inventory.QuickAccessSlotCount; ++Index)
    {
        TestNull(TEXT("获得穿戴品不占用快捷栏"), Inventory.Slots[Index].Definition.Get());
    }
    TestNull(TEXT("穿戴品不创建装备演员"), Inventory.Slots[5].EquipmentInstance.Get());
    for (int32 Index = 0; Index < 31; ++Index)
    {
        Character->SubmitInput(FBBBItemAddLocalControlPacket{{TEXT("Flag")}});
    }
    Step();
    TestEqual(TEXT("杂物独立容量"), Operations.SucceededCount, 30);
    TestEqual(TEXT("杂物满包拒绝"), Operations.RejectedCount, 1);
    const FGuid Helmet = Inventory.Slots[5].InstanceId;
    const FGuid Flag = Inventory.Slots[25].InstanceId;
    const auto Move = [&](int32 From, int32 To, FGuid Identity)
    {
        Character->SubmitInput(FBBBItemMoveLocalControlPacket{{From}, {To}, {Identity}});
        Step();
    };
    Move(5, 0, Helmet);
    TestEqual(TEXT("穿戴品移入快捷栏拒绝"), Operations.RejectedCount, 1);
    TestEqual(TEXT("拒绝后穿戴品保持来源身份"), Inventory.Slots[5].InstanceId, Helmet);
    TestNull(TEXT("拒绝后快捷栏仍为空"), Inventory.Slots[0].Definition.Get());
    Move(5, 25, Helmet);
    TestEqual(TEXT("类别互换原子拒绝"), Operations.RejectedCount, 1);
    TestEqual(TEXT("失败保持装备身份"), Inventory.Slots[5].InstanceId, Helmet);
    TestEqual(TEXT("失败保持杂物身份"), Inventory.Slots[25].InstanceId, Flag);
    Move(5, 56, Helmet);
    TestEqual(TEXT("头盔不能放入上衣位置"), Operations.RejectedCount, 1);
    Move(5, 55, Helmet);
    TestEqual(TEXT("穿戴只移动原实例"), Inventory.Slots[55].InstanceId, Helmet);
    TestNull(TEXT("穿戴释放原背包位置"), Inventory.Slots[5].Definition.Get());
    Character->SubmitInput(FBBBItemAddLocalControlPacket{{TEXT("Helmet")}});
    Step();
    Move(55, INDEX_NONE, Helmet);
    TestEqual(TEXT("满包脱下拒绝"), Operations.RejectedCount, 1);
    TestEqual(TEXT("满包脱下不丢失物品"), Inventory.Slots[55].InstanceId, Helmet);
    Move(55, 5, Helmet);
    TestEqual(TEXT("满包合法交换允许"), Inventory.Slots[5].InstanceId, Helmet);
    TestTrue(TEXT("穿戴位置保留被交换头盔"), Inventory.Slots[55].InstanceId.IsValid());
    Move(25, 62, Flag);
    TestEqual(TEXT("国旗占据真实穿戴位置"), Inventory.Slots[62].InstanceId, Flag);
    Move(25, 26, Flag);
    TestEqual(TEXT("过期拖拽拒绝"), Operations.RejectedCount, 1);
    TestEqual(TEXT("过期拖动不影响穿戴物品"), Inventory.Slots[62].InstanceId, Flag);
    TSet<FGuid> Identities;
    for (const auto &Item : Inventory.Slots)
    {
        if (Item.Definition)
        {
            TestFalse(TEXT("真实实例只登记一次"), Identities.Contains(Item.InstanceId));
            Identities.Add(Item.InstanceId);
        }
    }
    TestEqual(TEXT("全部已获得物品仍唯一存在"), Identities.Num(), 51);
    Items.Shutdown();
    return true;
}

#endif
