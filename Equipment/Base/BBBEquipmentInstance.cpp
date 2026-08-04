#include "BBBWork/UBBBNexus/Equipment/Base/BBBEquipmentInstance.h"

#include "BBBWork/UBBBNexus/Character/ExternalAPI/BBBCharacterExternalAPI.h"
#include "BBBWork/UBBBNexus/Equipment/Base/BBBEquipmentDefinition.h"
#include "BBBWork/UBBBNexus/Equipment/Definition/BBBEquipmentRuntimeData.h"
#include "BBBWork/UBBBNexus/Equipment/Domains/Equip/BBBEquipDomain.h"
#include "BBBWork/UBBBNexus/Equipment/Domains/Equip/Definition/BBBEquipRuntimeData.h"
#include "BBBWork/UBBBNexus/Equipment/Domains/Fire/Base/BBBFireDomain.h"
#include "BBBWork/UBBBNexus/Equipment/Domains/Fire/Definition/BBBFireRuntimeData.h"
#include "BBBWork/UBBBNexus/Equipment/Domains/Magazine/BBBMagazineDomain.h"
#include "BBBWork/UBBBNexus/Equipment/Domains/Magazine/Definition/BBBMagazineRuntimeData.h"
#include "BBBWork/UBBBNexus/Equipment/Presentation/BBBEquipmentPresentationActor.h"

UBBBEquipmentInstance *UBBBEquipmentInstance::Create(
    UObject &Outer,
    UBBBEquipmentDefinition &InDefinition)
{
    if (!ensureMsgf(InDefinition.EquipDomain, TEXT("[UBBBE]Equipment definition has no equip domain")))
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
            && (!InDefinition.FireDomain || Instance->RuntimeData->GetFire())
            && (!InDefinition.MagazineDomain || Instance->RuntimeData->GetMagazine()),
        TEXT("[UBBBE]Equipment domain runtime data is incomplete")))
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

    if (!ensureMsgf(Definition && RuntimeData && Definition->EquipDomain && RuntimeData->GetEquip(), TEXT("[UBBBE]Equipment instance cannot equip")))
    {
        return;
    }

    PresentationActor = Definition->EquipDomain->Equip(
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

    if (Definition->EquipDomain && RuntimeData->GetEquip())
    {
        Definition->EquipDomain->Update(*PresentationActor, *RuntimeData->GetEquip());
    }

    if (Definition->MagazineDomain && RuntimeData->GetMagazine())
    {
        Definition->MagazineDomain->Update(CharacterAPI, *PresentationActor, *RuntimeData->GetMagazine());
    }
}

bool UBBBEquipmentInstance::Fire(FBBBCharacterExternalAPI &CharacterAPI)
{
    if (!ensureMsgf(PresentationActor && Definition && RuntimeData && Definition->FireDomain && RuntimeData->GetFire(), TEXT("[UBBBE]Equipment fire domain is unavailable")))
    {
        return false;
    }

    if (Definition->MagazineDomain
        && RuntimeData->GetMagazine()
        && !Definition->MagazineDomain->CanConsumeRound(*RuntimeData->GetMagazine()))
    {
        return false;
    }

    if (!Definition->FireDomain->Fire(CharacterAPI, *PresentationActor, *RuntimeData->GetFire()))
    {
        return false;
    }

    if (Definition->MagazineDomain && RuntimeData->GetMagazine())
    {
        Definition->MagazineDomain->ConsumeRound(*RuntimeData->GetMagazine());
    }

    return true;
}

bool UBBBEquipmentInstance::Reload(FBBBCharacterExternalAPI &CharacterAPI)
{
    if (!ensureMsgf(PresentationActor && Definition && RuntimeData && Definition->MagazineDomain && RuntimeData->GetMagazine(), TEXT("[UBBBE]Equipment magazine domain is unavailable")))
    {
        return false;
    }

    return Definition->MagazineDomain->Reload(
        CharacterAPI,
        *PresentationActor,
        *RuntimeData->GetMagazine());
}

void UBBBEquipmentInstance::PresentFire(FBBBCharacterExternalAPI &CharacterAPI)
{
    if (PresentationActor && Definition && Definition->FireDomain)
    {
        Definition->FireDomain->Present(CharacterAPI, *PresentationActor);
    }
}

void UBBBEquipmentInstance::PresentReload(FBBBCharacterExternalAPI &CharacterAPI)
{
    if (PresentationActor && Definition && Definition->MagazineDomain)
    {
        Definition->MagazineDomain->PresentReload(CharacterAPI);
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
    const UBBBEquipRuntimeData *EquipRuntimeData = RuntimeData
        ? RuntimeData->GetEquip()
        : nullptr;

    return EquipRuntimeData && EquipRuntimeData->IsEquipping();
}

bool UBBBEquipmentInstance::IsValid() const
{
    return InstanceId.IsValid()
        && Definition
        && Definition->EquipDomain
        && RuntimeData
        && RuntimeData->GetEquip();
}
