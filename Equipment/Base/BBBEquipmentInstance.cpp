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
    if (!ensureMsgf(InDefinition.EquipFragment, TEXT("[UBBBE]Equipment definition has no equip fragment")))
    {
        return nullptr;
    }

    UBBBEquipmentInstance *Instance = NewObject<UBBBEquipmentInstance>(&Outer);
    if (!ensureMsgf(Instance, TEXT("[UBBBE]Equipment instance creation failed")))
    {
        return nullptr;
    }

    Instance->InstanceId = FGuid::NewGuid();
    Instance->Definition = &InDefinition;
    Instance->RuntimeData = NewObject<UBBBEquipmentRuntimeData>(Instance);

    if (!ensureMsgf(Instance->RuntimeData, TEXT("[UBBBE]Equipment runtime data creation failed")))
    {
        return nullptr;
    }

    Instance->RuntimeData->Initialize(InDefinition);
    if (!ensureMsgf(
        Instance->RuntimeData->GetEquip()
            && (!InDefinition.FireFragment || Instance->RuntimeData->GetFire())
            && (!InDefinition.MagazineFragment || Instance->RuntimeData->GetMagazine()),
        TEXT("[UBBBE]Equipment fragment runtime data is incomplete")))
    {
        return nullptr;
    }

    return Instance;
}

void UBBBEquipmentInstance::Equip(
    USkeletalMeshComponent &CharacterMesh,
    FBBBCharacterExternalAPI &CharacterAPI,
    FName AttachmentSocketName)
{
    if (PresentationActor)
    {
        return;
    }

    if (!ensureMsgf(Definition && RuntimeData && Definition->EquipFragment && RuntimeData->GetEquip(), TEXT("[UBBBE]Equipment instance cannot equip")))
    {
        return;
    }

    PresentationActor = Definition->EquipFragment->Equip(
        *RuntimeData->GetEquip(),
        CharacterMesh,
        CharacterAPI,
        AttachmentSocketName);
}

void UBBBEquipmentInstance::Update(FBBBCharacterExternalAPI &CharacterAPI)
{
    if (!PresentationActor || !Definition || !RuntimeData)
    {
        return;
    }

    if (Definition->EquipFragment && RuntimeData->GetEquip())
    {
        Definition->EquipFragment->Update(*PresentationActor, *RuntimeData->GetEquip());
    }

    if (Definition->MagazineFragment && RuntimeData->GetMagazine())
    {
        Definition->MagazineFragment->Update(CharacterAPI, *PresentationActor, *RuntimeData->GetMagazine());
    }
}

bool UBBBEquipmentInstance::Fire(FBBBCharacterExternalAPI &CharacterAPI)
{
    if (!ensureMsgf(PresentationActor && Definition && RuntimeData && Definition->FireFragment && RuntimeData->GetFire(), TEXT("[UBBBE]Equipment fire fragment is unavailable")))
    {
        return false;
    }

    if (Definition->MagazineFragment
        && RuntimeData->GetMagazine()
        && !Definition->MagazineFragment->CanConsumeRound(*RuntimeData->GetMagazine()))
    {
        return false;
    }

    if (!Definition->FireFragment->Fire(CharacterAPI, *PresentationActor, *RuntimeData->GetFire()))
    {
        return false;
    }

    if (Definition->MagazineFragment && RuntimeData->GetMagazine())
    {
        Definition->MagazineFragment->ConsumeRound(*RuntimeData->GetMagazine());
    }

    return true;
}

bool UBBBEquipmentInstance::Reload(FBBBCharacterExternalAPI &CharacterAPI)
{
    if (!ensureMsgf(PresentationActor && Definition && RuntimeData && Definition->MagazineFragment && RuntimeData->GetMagazine(), TEXT("[UBBBE]Equipment magazine fragment is unavailable")))
    {
        return false;
    }

    return Definition->MagazineFragment->Reload(
        CharacterAPI,
        *PresentationActor,
        *RuntimeData->GetMagazine());
}

void UBBBEquipmentInstance::PresentFire(FBBBCharacterExternalAPI &CharacterAPI)
{
    if (PresentationActor && Definition && Definition->FireFragment)
    {
        Definition->FireFragment->Present(CharacterAPI, *PresentationActor);
    }
}

void UBBBEquipmentInstance::PresentReload(FBBBCharacterExternalAPI &CharacterAPI)
{
    if (PresentationActor && Definition && Definition->MagazineFragment)
    {
        Definition->MagazineFragment->PresentReload(CharacterAPI);
    }
}

void UBBBEquipmentInstance::ReleasePresentation()
{
    if (!PresentationActor)
    {
        return;
    }

    PresentationActor->Destroy();
    PresentationActor = nullptr;
}

const FGuid &UBBBEquipmentInstance::GetInstanceId() const
{
    return InstanceId;
}

UBBBEquipmentDefinition *UBBBEquipmentInstance::GetDefinition() const
{
    return Definition;
}

ABBBEquipmentPresentationActor *UBBBEquipmentInstance::GetPresentationActor() const
{
    return PresentationActor;
}

bool UBBBEquipmentInstance::IsEquipping() const
{
    if (!RuntimeData)
    {
        return false;
    }

    const UBBBEquipRuntimeData *EquipRuntimeData = RuntimeData->GetEquip();
    return EquipRuntimeData && EquipRuntimeData->IsEquipping();
}

bool UBBBEquipmentInstance::IsValid() const
{
    return InstanceId.IsValid()
        && Definition
        && Definition->EquipFragment
        && RuntimeData
        && RuntimeData->GetEquip();
}
