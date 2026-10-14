#include "Misc/AutomationTest.h"
#if WITH_DEV_AUTOMATION_TESTS && WITH_EDITOR
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Catalog/BBBEquipmentCatalog.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Base/BBBEquipment.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Base/Config/BBBEquipmentDefinition.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Catalog/BBBConsumableCatalog.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Instance/Grenade/BBBGrenadeConsumable.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Instance/Grenade/Config/BBBGrenadeDefinition.h"
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
        TestFalse(TEXT("未持有的装备拒绝主行为"), Equipment->SubmitInput(FBBBEquipmentPrimaryLocalControlPacket<>{}));
        Equipment->Destroy();
    }
    TestEqual(TEXT("九类装备均有真实登记资产"), Types.Num(), 9);
    const auto GrenadeClass = ConsumableCatalog->FindItem(TEXT("Grenade_01"));
    if (!TestNotNull(TEXT("实际手雷类"), GrenadeClass.Get()))
    {
        return false;
    }
    auto *Grenade = World->SpawnActor<ABBBGrenadeConsumable>(GrenadeClass);
    Grenade->SetOwner(Character);
    Grenade->DispatchBeginPlay();
    TestTrue(TEXT("手雷经公共和具体初始化成立"), Grenade->IsInitialized());
    TestNotNull(TEXT("手雷具有具体协议组件"), Grenade->GetNetworkComponent());
    Character->RuntimeData.External.NetworkIdentityState.bIsMirror = true;
    FBBBGrenadeNetworkMessage Result;
    Result.Revision = 20;
    Result.UseSerial = 4;
    Result.bUsing = true;
    TestTrue(TEXT("镜像接受最新使用结果"), Grenade->SubmitInput(FBBBGrenadeStateAuthorityFactPacket{{}, {Result}}));
    Grenade->Tick(1.0f / 60.0f);
    TestEqual(TEXT("镜像还原当前使用序号"), Grenade->GetUseSerial(), 4);
    Result.Revision = 19;
    Result.UseSerial = 3;
    TestTrue(TEXT("迟到结果可入队"), Grenade->SubmitInput(FBBBGrenadeStateAuthorityFactPacket{{}, {Result}}));
    Grenade->Tick(1.0f / 60.0f);
    TestEqual(TEXT("迟到结果不得覆盖当前使用序号"), Grenade->GetUseSerial(), 4);
    Result.ReleaseSerial = 5;
    TestFalse(TEXT("格式无效的释放结果在入口拒绝"), Grenade->SubmitInput(FBBBGrenadeStateAuthorityFactPacket{{}, {Result}}));
    return !HasAnyErrors();
}
#endif
