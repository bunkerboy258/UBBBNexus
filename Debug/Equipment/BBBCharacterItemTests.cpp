#include "Misc/AutomationTest.h"

#if WITH_DEV_AUTOMATION_TESTS && WITH_EDITOR

#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/ItemSystem/BBBCharacterItemSystem.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/ParseSystem/BBBCharacterParseSystem.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/BBBRifleEquipment.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/Config/BBBRifleDefinition.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/Logic/System/ActionSystem/Processors/BBBRifleActionProcessor.h"
#include "BBBWork/UBBBNexus/Equipment/Catalog/BBBEquipmentCatalog.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "Misc/ScopeExit.h"
#include "UObject/UnrealType.h"

/** 隔离世界验证统一背包与上层选择的独立责任 */
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBBBCharacterItemSystemTest, "BBB.Character.ItemSystem",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FBBBCharacterItemSystemTest::RunTest(const FString &Parameters)
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

    ABBBCharacter *Character = World->SpawnActor<ABBBCharacter>();
    ABBBRifleEquipment *First = World->SpawnActor<ABBBRifleEquipment>();
    ABBBRifleEquipment *Second = World->SpawnActor<ABBBRifleEquipment>();
    if (!TestNotNull(TEXT("测试角色"), Character) || !TestNotNull(TEXT("第一把步枪"), First)
        || !TestNotNull(TEXT("第二把步枪"), Second))
    {
        return false;
    }

    UBBBCharacterConfig *Config = NewObject<UBBBCharacterConfig>(Character);
    Config->Item.InventorySlotCount = 3;
    Config->Item.QuickAccessSlotCount = 2;
    Config->Equipment.EquipmentCatalog = NewObject<UBBBEquipmentCatalog>(Config);
    FObjectProperty *Property = FindFProperty<FObjectProperty>(ABBBCharacter::StaticClass(), TEXT("CharacterConfigAsset"));
    Property->SetObjectPropertyValue_InContainer(Character, Config);
    Character->RuntimeData.External.NetworkIdentityState.bIsMirror = false;
    FBBBCharacterItemSystem System;
    System.Initialize(*Character, Character->RuntimeData, Config->Item);
    FBBBCharacterParseSystem Parse;
    Parse.Initialize(Character->RuntimeData);
    System.Update();

    auto &Items = Character->RuntimeData.Item;
    TestEqual(TEXT("背包容量包含快捷区域"), Items.ReadItemInventoryState().BackpackSlots.Num(), 3);
    TestEqual(TEXT("前序快捷区域数量"), Items.ReadItemBarState().QuickAccessSlotCount, 2);
    TestEqual(TEXT("初始化保持空手"), Items.ReadItemBarState().SelectedSlot, INDEX_NONE);
    Items.ItemInventoryState.BackpackSlots[0].ItemActor = First;
    Items.ItemInventoryState.BackpackSlots[1].ItemActor = Second;

    UBBBRifleDefinition *FirstDefinition = NewObject<UBBBRifleDefinition>();
    FirstDefinition->AmmoCapacity = 10;
    UBBBRifleDefinition *SecondDefinition = NewObject<UBBBRifleDefinition>();
    SecondDefinition->AmmoCapacity = 30;
    FBBBRifleActionProcessor::Initialize(First->RuntimeData, *FirstDefinition);
    FBBBRifleActionProcessor::Initialize(Second->RuntimeData, *SecondDefinition);

    Character->SubmitInput(FBBBItemSelectLocalControlPacket{{0}});
    Parse.Update();
    System.Update();
    TestTrue(TEXT("选中槽位生成第一把枪目标"), Items.ReadItemBarState().DesiredMainHandItem == First);
    const int32 SelectionRevision = Items.ReadItemBarState().Revision;
    Character->SubmitInput(FBBBItemSelectLocalControlPacket{{0}});
    Parse.Update();
    System.Update();
    TestEqual(TEXT("重复选择不收枪也不生成新选择版本"), Items.ReadItemBarState().Revision, SelectionRevision);

    Character->SubmitInput(FBBBItemMoveLocalControlPacket{{0}, {1}});
    Parse.Update();
    System.Update();
    TestTrue(TEXT("交换选中槽位后目标切换为第二把枪"), Items.ReadItemBarState().DesiredMainHandItem == Second);
    TestTrue(TEXT("第一把枪保留在同一份背包"), Items.ReadItemInventoryState().BackpackSlots[1].ItemActor == First);
    TestEqual(TEXT("第一把枪保留自身弹量"), First->GetLoadedAmmo(), 10);
    TestEqual(TEXT("第二把枪保留自身弹量"), Second->GetLoadedAmmo(), 30);

    Character->SubmitInput(FBBBItemMoveLocalControlPacket{{0}, {2}});
    Parse.Update();
    System.Update();
    TestEqual(TEXT("选中槽位变空取消选择"), Items.ReadItemBarState().SelectedSlot, INDEX_NONE);
    TestNull(TEXT("选中槽位变空生成空手目标"), Items.ReadItemBarState().DesiredMainHandItem.Get());
    Character->SubmitInput(FBBBItemMoveLocalControlPacket{{2}, {0}});
    Parse.Update();
    System.Update();
    TestEqual(TEXT("物品回到原格不恢复选择"), Items.ReadItemBarState().SelectedSlot, INDEX_NONE);

    Character->SubmitInput(FBBBItemSelectLocalControlPacket{{2}});
    Parse.Update();
    System.Update();
    TestEqual(TEXT("后序槽位不能直接装备"), Items.ReadItemBarState().SelectedSlot, INDEX_NONE);
    TestEqual(TEXT("越界快捷选择返回操作失败"), Items.ReadItemOperationState().RejectedCount, 1);

    const int32 BeforeMoves = Items.ReadItemOperationState().Revision;
    Character->SubmitInput(FBBBItemMoveLocalControlPacket{{0}, {2}});
    Character->SubmitInput(FBBBItemMoveLocalControlPacket{{1}, {0}});
    Parse.Update();
    System.Update();
    TestEqual(TEXT("同帧两次移动都完成"), Items.ReadItemOperationState().Revision, BeforeMoves + 2);
    TestTrue(TEXT("同帧操作保留提交顺序"), Items.ReadItemInventoryState().BackpackSlots[0].ItemActor == First);
    TestTrue(TEXT("另一件物品仍在背包后序格"), Items.ReadItemInventoryState().BackpackSlots[2].ItemActor == Second);
    System.Update();
    TestEqual(TEXT("已消费输入不会下一帧重放"), Items.ReadItemOperationState().Revision, BeforeMoves + 2);

    AActor *Third = World->SpawnActor<AActor>();
    Items.ItemInventoryState.BackpackSlots[1].ItemActor = Third;
    const int32 BeforeAdds = Items.ReadItemOperationState().Revision;
    Character->SubmitInput(FBBBItemAddLocalControlPacket{{TEXT("MissingEquipment")}});
    Character->SubmitInput(FBBBItemAddLocalControlPacket{{TEXT("MissingEquipment")}});
    Parse.Update();
    System.Update();
    TestEqual(TEXT("同帧入包请求全部消费"), Items.ReadItemOperationState().Revision, BeforeAdds + 2);
    TestEqual(TEXT("满包拒绝新增"), Items.ReadItemOperationState().RejectedCount, 2);
    TestTrue(TEXT("满包拒绝不替换现有物品"), Items.ReadItemInventoryState().BackpackSlots[0].ItemActor == First);

    Character->SubmitInput(FBBBItemMoveLocalControlPacket{{99}, {0}});
    Parse.Update();
    System.Update();
    TestEqual(TEXT("非法移动失败"), Items.ReadItemOperationState().RejectedCount, 1);
    TestTrue(TEXT("非法移动不破坏已有物品"), Items.ReadItemInventoryState().BackpackSlots[0].ItemActor == First);

    Character->SubmitInput(FBBBItemSelectLocalControlPacket{{0}});
    Parse.Update();
    System.Update();
    First->Destroy();
    System.Update();
    TestNull(TEXT("失效物品引用被清除"), Items.ReadItemInventoryState().BackpackSlots[0].ItemActor.Get());
    TestEqual(TEXT("物品失效取消选择"), Items.ReadItemBarState().SelectedSlot, INDEX_NONE);

    Third->Destroy();
    System.Update();
    UClass *EquipmentClass = LoadClass<ABBBEquipment>(nullptr,
        TEXT("/Game/_Project/Characters/BBBC_UA/Equipment/Rifle/Rifle_01/BP_ModernWeapons_Rifle_01.BP_ModernWeapons_Rifle_01_C"));
    if (!TestNotNull(TEXT("现有步枪资产类"), EquipmentClass))
    {
        System.Shutdown();
        return false;
    }
    Config->Equipment.EquipmentCatalog->EquipmentClasses.Add(EquipmentClass);
    const FName EquipmentId = EquipmentClass->GetDefaultObject<ABBBEquipment>()->GetEquipmentId();
    World->InitializeActorsForPlay(FURL());
    World->SetBegunPlay(true);
    Character->SubmitInput(FBBBItemAddLocalControlPacket{{EquipmentId}});
    Character->SubmitInput(FBBBItemAddLocalControlPacket{{EquipmentId}});
    Parse.Update();
    System.Update();
    ABBBEquipment *AddedFirst = Cast<ABBBEquipment>(Items.ReadItemInventoryState().BackpackSlots[0].ItemActor.Get());
    ABBBEquipment *AddedSecond = Cast<ABBBEquipment>(Items.ReadItemInventoryState().BackpackSlots[1].ItemActor.Get());
    TestEqual(TEXT("两次获得真实装备均成功"), Items.ReadItemOperationState().SucceededCount, 2);
    TestTrue(TEXT("同型号物品创建独立实例并完成初始化"), IsValid(AddedFirst) && IsValid(AddedSecond)
        && AddedFirst != AddedSecond && AddedFirst->IsInitialized() && AddedSecond->IsInitialized());
    TestEqual(TEXT("获得装备不自动选择快捷槽位"), Items.ReadItemBarState().SelectedSlot, INDEX_NONE);
    TestNull(TEXT("获得装备保持空手目标"), Items.ReadItemBarState().DesiredMainHandItem.Get());
    if (IsValid(AddedFirst))
    {
        TestTrue(TEXT("入包物品保持隐藏"), AddedFirst->IsHidden());
        TestFalse(TEXT("入包物品不启用玩法更新"), AddedFirst->IsActorTickEnabled());
    }
    Character->SubmitInput(FBBBItemAddLocalControlPacket{{EquipmentId}});
    Parse.Update();
    System.Update();
    TestEqual(TEXT("有效装备请求也受满包限制"), Items.ReadItemOperationState().RejectedCount, 1);

    Character->RuntimeData.External.NetworkIdentityState.bIsMirror = true;
    Character->SubmitInput(FBBBItemAddLocalControlPacket{{TEXT("MissingEquipment")}});
    Character->SubmitInput(FBBBItemMoveLocalControlPacket{{2}, {0}});
    Character->SubmitInput(FBBBItemSelectLocalControlPacket{{0}});
    Parse.Update();
    TestTrue(TEXT("镜像拒绝真实背包获得输入"), Items.ReadItemOperationState().PendingEquipmentIds.IsEmpty());
    TestTrue(TEXT("镜像拒绝真实背包移动输入"), Items.ReadItemOperationState().PendingMoveSources.IsEmpty());
    TestTrue(TEXT("镜像拒绝快捷选择输入"), Items.ReadItemOperationState().PendingSelectedSlots.IsEmpty());

    System.Shutdown();
    TestTrue(TEXT("角色关闭清空背包"), Items.ReadItemInventoryState().BackpackSlots.IsEmpty());
    TestFalse(TEXT("角色关闭销毁背包中保留的装备"), IsValid(Second));
    TestFalse(TEXT("角色关闭销毁实际入包创建的第一件装备"), IsValid(AddedFirst));
    TestFalse(TEXT("角色关闭销毁实际入包创建的第二件装备"), IsValid(AddedSecond));
    return true;
}

#endif
