#include "BBBWork/UBBBNexus/Equipment/System/BBBEquipmentSystem.h"

#include "BBBWork/UBBBNexus/Character/ExternalAPI/BBBCharacterExternalAPI.h"
#include "BBBWork/UBBBNexus/Equipment/Base/BBBEquipmentDefinition.h"
#include "BBBWork/UBBBNexus/Equipment/Definition/BBBEquipmentRuntimeData.h"
#include "BBBWork/UBBBNexus/Equipment/Fragments/Equip/BBBEquipDomin.h"
#include "BBBWork/UBBBNexus/Equipment/Fragments/Equip/Definition/BBBEquipRuntimeData.h"
#include "BBBWork/UBBBNexus/Equipment/Fragments/Fire/BBBFireDomin.h"
#include "BBBWork/UBBBNexus/Equipment/Fragments/Fire/Definition/BBBFireRuntimeData.h"
#include "BBBWork/UBBBNexus/Equipment/Fragments/Magazine/BBBMagazineDomin.h"
#include "BBBWork/UBBBNexus/Equipment/Fragments/Magazine/Definition/BBBMagazineRuntimeData.h"
#include "BBBWork/UBBBNexus/Equipment/Presentation/BBBEquipmentPresentationActor.h"

bool UBBBEquipmentSystem::Initialize(
    UBBBEquipmentDefinition &InDefinition,
    UBBBEquipmentRuntimeData &InRuntimeData)
{
    Definition = &InDefinition;
    RuntimeData = &InRuntimeData;

    if (!ensureMsgf(
        RuntimeData->GetEquip()
            && (!Definition->FireDomin.IsValid() || RuntimeData->GetFire())
            && (!Definition->MagazineDomin.IsValid() || RuntimeData->GetMagazine()),
        TEXT("[UBBBE]Equipment fragment runtime data is incomplete")))
    {
        return false;
    }

    return true;
}

//------------------------------------------------------------------------------

void UBBBEquipmentSystem::Equip(
    USkeletalMeshComponent &CharacterMesh,
    FBBBCharacterExternalAPI &CharacterAPI,
    FName AttachmentSocketName)
{
    if (PresentationActor)
    {
        return;
    }

    if (!ensureMsgf(Definition && Definition->EquipDomin.IsValid() && RuntimeData && RuntimeData->GetEquip(), TEXT("[UBBBE]Equipment system cannot equip")))
    {
        return;
    }

    PresentationActor = Definition->EquipDomin.Get().Equip(
        *RuntimeData->GetEquip(),
        CharacterMesh,
        CharacterAPI,
        AttachmentSocketName);
}

//------------------------------------------------------------------------------

void UBBBEquipmentSystem::Update(FBBBCharacterExternalAPI &CharacterAPI)
{
    if (!ensureMsgf(PresentationActor && Definition && RuntimeData, TEXT("[UBBBE]Equipment system is incomplete during update")))
    {
        return;
    }

    if (!ensureMsgf(Definition->EquipDomin.IsValid() && RuntimeData->GetEquip(), TEXT("[UBBBE]Equipment equip domin is unavailable during update")))
    {
        return;
    }

    Definition->EquipDomin.Get().Update(CharacterAPI, *PresentationActor, *RuntimeData->GetEquip());

    if (Definition->MagazineDomin.IsValid()
        && ensureMsgf(RuntimeData->GetMagazine(), TEXT("[UBBBE]Equipment magazine runtime data is unavailable during update")))
    {
        Definition->MagazineDomin.Get().Update(CharacterAPI, *PresentationActor, *RuntimeData->GetMagazine());
    }
}

//------------------------------------------------------------------------------

bool UBBBEquipmentSystem::Fire(FBBBCharacterExternalAPI &CharacterAPI)
{
    if (!ensureMsgf(PresentationActor && Definition && RuntimeData && Definition->FireDomin.IsValid() && RuntimeData->GetFire(), TEXT("[UBBBE]Equipment fire domin is unavailable")))
    {
        return false;
    }

    if (Definition->MagazineDomin.IsValid())
    {
        if (!ensureMsgf(RuntimeData->GetMagazine(), TEXT("[UBBBE]Equipment magazine runtime data is unavailable during fire")))
        {
            return false;
        }

        if (!Definition->MagazineDomin.Get().CanConsumeRound(*RuntimeData->GetMagazine()))
        {
            return false;
        }
    }

    if (!Definition->FireDomin.Get().Fire(CharacterAPI, *PresentationActor, *RuntimeData->GetFire()))
    {
        return false;
    }

    if (Definition->MagazineDomin.IsValid())
    {
        Definition->MagazineDomin.Get().ConsumeRound(*RuntimeData->GetMagazine());
    }

    return true;
}

//------------------------------------------------------------------------------

bool UBBBEquipmentSystem::Reload(FBBBCharacterExternalAPI &CharacterAPI)
{
    if (!ensureMsgf(PresentationActor && Definition && RuntimeData && Definition->MagazineDomin.IsValid() && RuntimeData->GetMagazine(), TEXT("[UBBBE]Equipment magazine domin is unavailable")))
    {
        return false;
    }

    return Definition->MagazineDomin.Get().Reload(
        CharacterAPI,
        *PresentationActor,
        *RuntimeData->GetMagazine());
}

//------------------------------------------------------------------------------

void UBBBEquipmentSystem::PresentFire(FBBBCharacterExternalAPI &CharacterAPI)
{
    if (!ensureMsgf(PresentationActor && Definition && Definition->FireDomin.IsValid(), TEXT("[UBBBE]Equipment fire domin is unavailable during present fire")))
    {
        return;
    }

    Definition->FireDomin.Get().Present(CharacterAPI, *PresentationActor);
}

//------------------------------------------------------------------------------

void UBBBEquipmentSystem::PresentReload(FBBBCharacterExternalAPI &CharacterAPI)
{
    if (!ensureMsgf(PresentationActor && Definition && Definition->MagazineDomin.IsValid(), TEXT("[UBBBE]Equipment magazine domin is unavailable during present reload")))
    {
        return;
    }

    Definition->MagazineDomin.Get().PresentReload(CharacterAPI);
}

//------------------------------------------------------------------------------

void UBBBEquipmentSystem::ReleasePresentation()
{
    if (!PresentationActor)
    {
        return;
    }

    PresentationActor->Destroy();
    PresentationActor = nullptr;
}

//------------------------------------------------------------------------------

ABBBEquipmentPresentationActor *UBBBEquipmentSystem::GetPresentationActor() const
{
    return PresentationActor;
}

//------------------------------------------------------------------------------

bool UBBBEquipmentSystem::IsEquipping() const
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
