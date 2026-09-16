#if WITH_DEV_AUTOMATION_TESTS
#include "Misc/AutomationTest.h"
#include "Animation/AnimMontage.h"
#include "Components/SkeletalMeshComponent.h"
#include "BBBWork/UBBBNexus/Equipment/BBBEquipmentInstance.h"
#include "BBBWork/UBBBNexus/Equipment/Core/Config/BBBEquipmentDefinition.h"
#include "BBBWork/UBBBNexus/Equipment/Pipeline/Arbitration/BBBEquipmentActionArbitrator.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBBBEquipmentReloadRulesTest, "BBB.Equipment.ReloadRules",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FBBBEquipmentReloadRulesTest::RunTest(const FString &Parameters)
{
    UBBBEquipmentDefinition *Definition = NewObject<UBBBEquipmentDefinition>();
    Definition->ReloadConfig.Montage = NewObject<UAnimMontage>();
    Definition->AmmoConfig.AmmoCapacity = 30;
    FBBBEquipmentRuntimeData Runtime;
    Runtime.Ammo.LoadedAmmo = 5;
    FBBBEquipmentCommand Command;
    Command.Sequence = 7;
    auto Allowed = [&](const EBBBEquipmentCommandType Type)
    {
        Command.Type = Type;
        return FBBBEquipmentActionArbitrator::CanExecute(Command, Runtime, *Definition, 100.0f, true);
    };

    TestTrue(TEXT("有弹可开火"), Allowed(EBBBEquipmentCommandType::Fire));
    Runtime.Fire.LastFireTimeSeconds = 99.99f;
    TestFalse(TEXT("开火间隔由装备拒绝"), Allowed(EBBBEquipmentCommandType::Fire));
    Runtime.Fire.LastFireTimeSeconds = 0.0f;
    Runtime.Ammo.LoadedAmmo = 0;
    TestFalse(TEXT("空弹拒绝开火"), Allowed(EBBBEquipmentCommandType::Fire));
    TestTrue(TEXT("空弹允许换弹"), Allowed(EBBBEquipmentCommandType::Reload));
    Runtime.Ammo.LoadedAmmo = 30;
    TestFalse(TEXT("满弹拒绝换弹"), Allowed(EBBBEquipmentCommandType::Reload));
    Runtime.Ammo.LoadedAmmo = 5;
    Definition->ReloadConfig.Montage = nullptr;
    TestFalse(TEXT("缺少蒙太奇不得进入换弹"), Allowed(EBBBEquipmentCommandType::Reload));
    Definition->ReloadConfig.Montage = NewObject<UAnimMontage>();

    Runtime.Reload.bIsReloading = true;
    Runtime.Reload.Sequence = 7;
    TestFalse(TEXT("换弹期间拒绝开火"), Allowed(EBBBEquipmentCommandType::Fire));
    TestFalse(TEXT("换弹期间拒绝重复换弹"), Allowed(EBBBEquipmentCommandType::Reload));
    TestFalse(TEXT("未卸下不得装填"), Allowed(EBBBEquipmentCommandType::LoadMagazine));
    Command.Sequence = 6;
    TestFalse(TEXT("旧序号不得卸下"), Allowed(EBBBEquipmentCommandType::DetachMagazine));
    Command.Sequence = 7;
    TestTrue(TEXT("当前序号允许卸下"), Allowed(EBBBEquipmentCommandType::DetachMagazine));

    FBBBEquipmentReloadSystem Reload;
    Reload.DetachMagazine(Runtime);
    TestEqual(TEXT("卸下清空弹量"), Runtime.Ammo.LoadedAmmo, 0);
    TestFalse(TEXT("重复卸下被拒绝"), Allowed(EBBBEquipmentCommandType::DetachMagazine));
    TestTrue(TEXT("卸下后允许装填"), Allowed(EBBBEquipmentCommandType::LoadMagazine));
    Reload.Cancel(Runtime);
    TestEqual(TEXT("取消不补弹"), Runtime.Ammo.LoadedAmmo, 0);
    TestFalse(TEXT("取消后旧装填无效"), Allowed(EBBBEquipmentCommandType::LoadMagazine));
    TestFalse(TEXT("取消后空弹仍不得开火"), Allowed(EBBBEquipmentCommandType::Fire));

    Runtime.Reload.bIsReloading = true;
    Runtime.Reload.bMagazineDetached = true;
    Reload.LoadMagazine(Runtime, Definition->AmmoConfig);
    TestEqual(TEXT("装填补满配置容量"), Runtime.Ammo.LoadedAmmo, 30);
    TestFalse(TEXT("装填结束换弹"), Runtime.Reload.bIsReloading);
    TestFalse(TEXT("重复装填被拒绝"), Allowed(EBBBEquipmentCommandType::LoadMagazine));
    Reload.Cancel(Runtime);
    TestEqual(TEXT("装填后中断保留弹药"), Runtime.Ammo.LoadedAmmo, 30);
    TestTrue(TEXT("换弹结束允许开火"), Allowed(EBBBEquipmentCommandType::Fire));
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBBBEquipmentTickGroupTest, "BBB.Equipment.TickGroup",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FBBBEquipmentTickGroupTest::RunTest(const FString &Parameters)
{
    const ABBBEquipmentInstance *Equipment = GetDefault<ABBBEquipmentInstance>();
    TestTrue(TEXT("装备启用独立更新"), Equipment->PrimaryActorTick.bCanEverTick);
    TestTrue(TEXT("装备逻辑显式位于后更新组"), Equipment->PrimaryActorTick.TickGroup == TG_PostUpdateWork);
    const USkeletalMeshComponent *Mesh = Equipment->FindComponentByClass<USkeletalMeshComponent>();
    TestNotNull(TEXT("装备网格存在"), Mesh);
    if (Mesh)
    {
        TestTrue(TEXT("装备动画显式位于后更新组"), Mesh->PrimaryComponentTick.TickGroup == TG_PostUpdateWork);
        TestTrue(TEXT("装备动画依赖装备发布快照"), Mesh->PrimaryComponentTick.GetPrerequisites().Num() > 0);
    }
    return true;
}
#endif
