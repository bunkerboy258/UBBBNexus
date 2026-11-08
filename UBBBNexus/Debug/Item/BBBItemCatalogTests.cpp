#include "Misc/AutomationTest.h"
#if WITH_DEV_AUTOMATION_TESTS && WITH_EDITOR
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Catalog/BBBEquipmentCatalog.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Base/BBBEquipment.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Base/Config/BBBEquipmentDefinition.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Catalog/BBBConsumableCatalog.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Throwable/Grenade/BBBGrenadeConsumable.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Base/BBBConsumable.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Throwable/Grenade/Config/BBBGrenadeDefinition.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "Misc/ScopeExit.h"

/** 真实型号登记 初始化 失活和镜像结果顺序验收 */
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBBBItemCatalogTest, "BBB.Item.CatalogLifecycle",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FBBBItemCatalogTest::RunTest(const FString &)
{
    const auto Options = UWorld::InitializationValues().AllowAudioPlayback(false).CreatePhysicsScene(true)
        .CreateNavigation(false).CreateAISystem(false).ShouldSimulatePhysics(false);
    UWorld *World = UWorld::CreateWorld(EWorldType::Game, false, NAME_None, nullptr,
        true, ERHIFeatureLevel::Num, &Options);
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
    auto *Character = World->SpawnActor<ABBBCharacter>();
    auto *EquipmentCatalog = LoadObject<UBBBEquipmentCatalog>(nullptr,
        TEXT("/Game/_Project/Characters/BBBC_UA/Items/Equipment/DA_EquipmentCatalog.DA_EquipmentCatalog"));
    auto *ConsumableCatalog = LoadObject<UBBBConsumableCatalog>(nullptr,
        TEXT("/Game/_Project/Characters/BBBC_UA/Items/Consumable/DA_ConsumableCatalog.DA_ConsumableCatalog"));
    if (!TestNotNull(TEXT("装备类目录"), EquipmentCatalog)
        || !TestNotNull(TEXT("消耗品类目录"), ConsumableCatalog))
    {
        return false;
    }
    TestTrue(TEXT("装备目录完成回填"), !EquipmentCatalog->Items.IsEmpty());
    TSet<EBBBEquipmentType> Types;
    for (const auto &Class : EquipmentCatalog->Items)
    {
        const auto *CDO = Class ? Class.GetDefaultObject() : nullptr;
        const auto *Definition = CDO ? CDO->GetDefinition() : nullptr;
        if (!TestNotNull(TEXT("登记类的默认定义"), Definition))
        {
            return false;
        }
        TestTrue(TEXT("型号查找唯一返回登记类"), EquipmentCatalog->FindItem(Definition->ItemId) == Class);
        Types.Add(Definition->EquipmentType);
        auto *Equipment = World->SpawnActor<ABBBEquipment>(Class);
        Equipment->SetOwner(Character);
        Equipment->DispatchBeginPlay();
        TestTrue(*FString::Printf(TEXT("%s 初始化成立"), *Definition->ItemId.ToString()), Equipment->IsInitialized());
        TestTrue(TEXT("闲置装备允许正常切换"), Equipment->CanSwitch());
        Equipment->SubmitInput(FBBBEquipmentPrimaryLocalControlPacket<>{});
        Equipment->Destroy();
    }
    TestEqual(TEXT("九类装备均有真实登记资产"), Types.Num(), 9);
    auto *Definition = ConsumableCatalog->FindItem(TEXT("Grenade_01"));
    TestNotNull(TEXT("正式手雷定义"), Definition);
    if (Definition)
    {
        TestTrue(TEXT("轻量使用类型有效"),
                 Definition->RuntimeClass && Definition->RuntimeClass->IsChildOf(UBBBConsumable::StaticClass()));
        TestFalse(TEXT("消耗品类型不属于 Actor"),
                  Definition->RuntimeClass && Definition->RuntimeClass->IsChildOf(AActor::StaticClass()));
    }
    return !HasAnyErrors();
}
#endif
