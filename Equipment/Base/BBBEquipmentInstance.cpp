#include "BBBWork/UBBBNexus/Equipment/Base/BBBEquipmentInstance.h"

#include "BBBWork/UBBBNexus/Character/ExternalAPI/BBBCharacterExternalAPI.h"
#include "BBBWork/UBBBNexus/Equipment/Base/BBBEquipmentDefinition.h"
#include "BBBWork/UBBBNexus/Equipment/Definition/BBBEquipmentRuntimeData.h"
#include "BBBWork/UBBBNexus/Equipment/Fragments/Equip/BBBEquipFragment.h"
#include "BBBWork/UBBBNexus/Equipment/Fragments/Equip/Definition/BBBEquipRuntimeData.h"
#include "BBBWork/UBBBNexus/Equipment/Fragments/Fire/BBBSingleProjectileFireFragment.h"
#include "BBBWork/UBBBNexus/Equipment/Fragments/Fire/Definition/BBBFireRuntimeData.h"
#include "BBBWork/UBBBNexus/Equipment/Fragments/Magazine/BBBMagazineFragment.h"
#include "BBBWork/UBBBNexus/Equipment/Fragments/Magazine/Definition/BBBMagazineRuntimeData.h"
#include "BBBWork/UBBBNexus/Equipment/Presentation/BBBEquipmentPresentationActor.h"

UBBBEquipmentInstance *UBBBEquipmentInstance::Create(
    UObject &Outer,
    UBBBEquipmentDefinition &InDefinition)
{
    UBBBEquipmentInstance *Instance = NewObject<UBBBEquipmentInstance>(&Outer);
    if (!ensureMsgf(Instance, TEXT("[UBBBE]Equipment instance creation failed")))
    {
        return nullptr;
    }

    // 分配唯一标识并绑定静态配置
    Instance->InstanceId = FGuid::NewGuid();
    Instance->Definition = &InDefinition;
    Instance->RuntimeData = NewObject<UBBBEquipmentRuntimeData>(Instance);

    if (!ensureMsgf(Instance->RuntimeData, TEXT("[UBBBE]Equipment runtime data creation failed")))
    {
        return nullptr;
    }

    Instance->RuntimeData->Initialize(InDefinition);

    // 校验所有已配置碎片均已生成对应运行数据
    if (!ensureMsgf(
        Instance->RuntimeData->GetEquip()
            && (!InDefinition.FireFragment.bEnabled || Instance->RuntimeData->GetFire())
            && (!InDefinition.MagazineFragment.bEnabled || Instance->RuntimeData->GetMagazine()),
        TEXT("[UBBBE]Equipment fragment runtime data is incomplete")))
    {
        return nullptr;
    }

    return Instance;
}

//------------------------------------------------------------------------------

void UBBBEquipmentInstance::Equip(
    USkeletalMeshComponent &CharacterMesh,
    FBBBCharacterExternalAPI &CharacterAPI,
    FName AttachmentSocketName)
{
    // 已有表现实体说明装备完成，忽略重复装备
    if (PresentationActor)
    {
        return;
    }

    if (!ensureMsgf(Definition && RuntimeData && RuntimeData->GetEquip(), TEXT("[UBBBE]Equipment instance cannot equip")))
    {
        return;
    }

    PresentationActor = Definition->EquipFragment.Equip(
        *RuntimeData->GetEquip(),
        CharacterMesh,
        CharacterAPI,
        AttachmentSocketName);
}

//------------------------------------------------------------------------------

void UBBBEquipmentInstance::Update(FBBBCharacterExternalAPI &CharacterAPI)
{
    if (!ensureMsgf(PresentationActor && Definition && RuntimeData, TEXT("[UBBBE]Equipment instance is incomplete during update")))
    {
        return;
    }

    // 装备碎片为强制配置，缺失即视为异常
    if (!ensureMsgf(RuntimeData->GetEquip(), TEXT("[UBBBE]Equipment equip fragment is unavailable during update")))
    {
        return;
    }

    // 推进装备过渡状态
    Definition->EquipFragment.Update(CharacterAPI, *PresentationActor, *RuntimeData->GetEquip());

    // 推进弹匣状态，弹匣碎片可选但配置了就必须有对应运行数据
    if (Definition->MagazineFragment.bEnabled
        && ensureMsgf(RuntimeData->GetMagazine(), TEXT("[UBBBE]Equipment magazine runtime data is unavailable during update")))
    {
        Definition->MagazineFragment.Update(CharacterAPI, *PresentationActor, *RuntimeData->GetMagazine());
    }
}

//------------------------------------------------------------------------------

bool UBBBEquipmentInstance::Fire(FBBBCharacterExternalAPI &CharacterAPI)
{
    if (!ensureMsgf(PresentationActor && Definition && RuntimeData && Definition->FireFragment.bEnabled && RuntimeData->GetFire(), TEXT("[UBBBE]Equipment fire fragment is unavailable")))
    {
        return false;
    }

    // 弹匣碎片可选但配置了就必须有对应运行数据，弹药不足时拒绝开火
    if (Definition->MagazineFragment.bEnabled)
    {
        if (!ensureMsgf(RuntimeData->GetMagazine(), TEXT("[UBBBE]Equipment magazine runtime data is unavailable during fire")))
        {
            return false;
        }

        if (!Definition->MagazineFragment.CanConsumeRound(*RuntimeData->GetMagazine()))
        {
            return false;
        }
    }

    if (!Definition->FireFragment.Fire(CharacterAPI, *PresentationActor, *RuntimeData->GetFire()))
    {
        return false;
    }

    // 开火成功后再消耗弹药，保证失败不扣弹，运行数据已在上方校验
    if (Definition->MagazineFragment.bEnabled)
    {
        Definition->MagazineFragment.ConsumeRound(*RuntimeData->GetMagazine());
    }

    return true;
}

//------------------------------------------------------------------------------

bool UBBBEquipmentInstance::Reload(FBBBCharacterExternalAPI &CharacterAPI)
{
    if (!ensureMsgf(PresentationActor && Definition && RuntimeData && Definition->MagazineFragment.bEnabled && RuntimeData->GetMagazine(), TEXT("[UBBBE]Equipment magazine fragment is unavailable")))
    {
        return false;
    }

    return Definition->MagazineFragment.Reload(
        CharacterAPI,
        *PresentationActor,
        *RuntimeData->GetMagazine());
}

//------------------------------------------------------------------------------

void UBBBEquipmentInstance::PresentFire(FBBBCharacterExternalAPI &CharacterAPI)
{
    if (!ensureMsgf(PresentationActor && Definition && Definition->FireFragment.bEnabled, TEXT("[UBBBE]Equipment fire fragment is unavailable during present fire")))
    {
        return;
    }

    Definition->FireFragment.Present(CharacterAPI, *PresentationActor);
}

//------------------------------------------------------------------------------

void UBBBEquipmentInstance::PresentReload(FBBBCharacterExternalAPI &CharacterAPI)
{
    if (!ensureMsgf(PresentationActor && Definition && Definition->MagazineFragment.bEnabled, TEXT("[UBBBE]Equipment magazine fragment is unavailable during present reload")))
    {
        return;
    }

    Definition->MagazineFragment.PresentReload(CharacterAPI);
}

//------------------------------------------------------------------------------

void UBBBEquipmentInstance::ReleasePresentation()
{
    // 未生成表现实体时无需处理
    if (!PresentationActor)
    {
        return;
    }

    PresentationActor->Destroy();
    PresentationActor = nullptr;
}

//------------------------------------------------------------------------------

const FGuid &UBBBEquipmentInstance::GetInstanceId() const
{
    return InstanceId;
}

//------------------------------------------------------------------------------

UBBBEquipmentDefinition *UBBBEquipmentInstance::GetDefinition() const
{
    return Definition;
}

//------------------------------------------------------------------------------

ABBBEquipmentPresentationActor *UBBBEquipmentInstance::GetPresentationActor() const
{
    return PresentationActor;
}

//------------------------------------------------------------------------------

bool UBBBEquipmentInstance::IsEquipping() const
{
    if (!ensureMsgf(RuntimeData, TEXT("[UBBBE]Equipment runtime data is unavailable")))
    {
        return false;
    }

    const UBBBEquipRuntimeData *EquipRuntimeData = RuntimeData->GetEquip();
    if (!ensureMsgf(EquipRuntimeData, TEXT("[UBBBE]Equipment equip runtime data is unavailable")))
    {
        return false;
    }

    return EquipRuntimeData->IsEquipping();
}

//------------------------------------------------------------------------------

bool UBBBEquipmentInstance::IsValid() const
{
    return InstanceId.IsValid()
        && Definition
        && RuntimeData
        && RuntimeData->GetEquip();
}
