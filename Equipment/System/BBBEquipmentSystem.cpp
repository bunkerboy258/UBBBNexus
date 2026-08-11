#include "BBBWork/UBBBNexus/Equipment/System/BBBEquipmentSystem.h"

#include "BBBWork/UBBBNexus/Character/ExternalAPI/BBBCharacterExternalAPI.h"
#include "BBBWork/UBBBNexus/Equipment/Base/BBBEquipmentDefinition.h"
#include "BBBWork/UBBBNexus/Equipment/Base/BBBEquipmentInstance.h"
#include "BBBWork/UBBBNexus/Equipment/RunTime/BBBEquipmentRuntimeData.h"
#include "BBBWork/UBBBNexus/Equipment/Fragments/Equip/BBBEquipDomin.h"
#include "BBBWork/UBBBNexus/Equipment/Fragments/Equip/Definition/BBBEquipRuntimeData.h"
#include "BBBWork/UBBBNexus/Equipment/Fragments/Fire/BBBFireDomin.h"
#include "BBBWork/UBBBNexus/Equipment/Fragments/Fire/Definition/BBBFireRuntimeData.h"
#include "BBBWork/UBBBNexus/Equipment/Fragments/Magazine/BBBMagazineDomin.h"
#include "BBBWork/UBBBNexus/Equipment/Fragments/Magazine/Definition/BBBMagazineRuntimeData.h"
#include "BBBWork/UBBBNexus/Equipment/Presentation/BBBEquipmentPresentationActor.h"
#include "Components/StaticMeshComponent.h"

bool UBBBEquipmentSystem::Initialize(
    UBBBEquipmentInstance &InInstance,
    UBBBEquipmentDefinition &InDefinition,
    UBBBEquipmentRuntimeData &InRuntimeData)
{
    Instance = &InInstance;
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
    if (!ensureMsgf(Instance, TEXT("[UBBBE]Equipment system has no owning instance")))
    {
        return;
    }

    if (Instance->PresentationActor)
    {
        return;
    }

    if (!ensureMsgf(Definition && Definition->EquipDomin.IsValid() && RuntimeData && RuntimeData->GetEquip(), TEXT("[UBBBE]Equipment system cannot equip")))
    {
        return;
    }

    Instance->PresentationActor = Definition->EquipDomin.Get().Equip(
        *RuntimeData->GetEquip(),
        CharacterMesh,
        CharacterAPI,
        AttachmentSocketName);
}

//------------------------------------------------------------------------------

void UBBBEquipmentSystem::Update(FBBBCharacterExternalAPI &CharacterAPI)
{
    if (!ensureMsgf(Instance && Instance->PresentationActor && Definition && RuntimeData, TEXT("[UBBBE]Equipment system is incomplete during update")))
    {
        return;
    }

    if (!ensureMsgf(Definition->EquipDomin.IsValid() && RuntimeData->GetEquip(), TEXT("[UBBBE]Equipment equip domin is unavailable during update")))
    {
        return;
    }

    Definition->EquipDomin.Get().Update(CharacterAPI, *Instance->PresentationActor, *RuntimeData->GetEquip());

    if (Definition->FireDomin.IsValid()
        && ensureMsgf(RuntimeData->GetFire(), TEXT("[UBBBE]Equipment fire runtime data is unavailable during update")))
    {
        Definition->FireDomin.Get().Update(CharacterAPI, *Instance->PresentationActor, *RuntimeData->GetFire());
    }

    if (Definition->MagazineDomin.IsValid()
        && ensureMsgf(RuntimeData->GetMagazine(), TEXT("[UBBBE]Equipment magazine runtime data is unavailable during update")))
    {
        Definition->MagazineDomin.Get().Update(CharacterAPI, *Instance->PresentationActor, *RuntimeData->GetMagazine());
    }
}

//------------------------------------------------------------------------------

bool UBBBEquipmentSystem::Fire(FBBBCharacterExternalAPI &CharacterAPI)
{
    if (!ensureMsgf(Instance && Instance->PresentationActor && Definition && RuntimeData && Definition->FireDomin.IsValid() && RuntimeData->GetFire(), TEXT("[UBBBE]Equipment fire domin is unavailable")))
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

    if (!Definition->FireDomin.Get().Fire(CharacterAPI, *Instance->PresentationActor, *RuntimeData->GetFire()))
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
    if (!ensureMsgf(Instance && Instance->PresentationActor && Definition && RuntimeData && Definition->MagazineDomin.IsValid() && RuntimeData->GetMagazine(), TEXT("[UBBBE]Equipment magazine domin is unavailable")))
    {
        return false;
    }

    return Definition->MagazineDomin.Get().Reload(
        CharacterAPI,
        *Instance->PresentationActor,
        *RuntimeData->GetMagazine());
}

//------------------------------------------------------------------------------

void UBBBEquipmentSystem::PresentFire(FBBBCharacterExternalAPI &CharacterAPI)
{
    if (!ensureMsgf(Instance && Instance->PresentationActor && Definition && RuntimeData && Definition->FireDomin.IsValid() && RuntimeData->GetFire(), TEXT("[UBBBE]Equipment fire domin is unavailable during present fire")))
    {
        return;
    }

    Definition->FireDomin.Get().Present(CharacterAPI, *Instance->PresentationActor, *RuntimeData->GetFire());
}

//------------------------------------------------------------------------------

void UBBBEquipmentSystem::PresentReload(FBBBCharacterExternalAPI &CharacterAPI)
{
    if (!ensureMsgf(Instance && Instance->PresentationActor && Definition && RuntimeData && Definition->MagazineDomin.IsValid() && RuntimeData->GetMagazine(), TEXT("[UBBBE]Equipment magazine domin is unavailable during present reload")))
    {
        return;
    }

    Definition->MagazineDomin.Get().PresentReload(
        CharacterAPI,
        *Instance->PresentationActor,
        *RuntimeData->GetMagazine());
}

//------------------------------------------------------------------------------

void UBBBEquipmentSystem::ReleasePresentation()
{
    if (!Instance || !Instance->PresentationActor)
    {
        return;
    }

    Instance->PresentationActor->Destroy();
    Instance->PresentationActor = nullptr;
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

//------------------------------------------------------------------------------

bool UBBBEquipmentSystem::TryGetAimSourceWorldTransform(FTransform &OutTransform) const
{
    OutTransform = FTransform::Identity;

    if (!Definition || !Definition->EquipDomin.IsValid() || !Instance || !Instance->PresentationActor)
    {
        return false;
    }

    const UStaticMeshComponent *EquipmentMesh = Instance->PresentationActor->GetEquipmentMesh();
    if (!EquipmentMesh)
    {
        return false;
    }

    const FName SocketName = Definition->EquipDomin.Get().GetAimSourceSocketName();
    if (!EquipmentMesh->DoesSocketExist(SocketName))
    {
        return false;
    }

    OutTransform = EquipmentMesh->GetSocketTransform(SocketName, RTS_World);
    return true;
}

//------------------------------------------------------------------------------

bool UBBBEquipmentSystem::TryGetLeftHandGripWorldTransform(FTransform &OutTransform) const
{
    OutTransform = FTransform::Identity;

    if (!Definition || !Definition->EquipDomin.IsValid() || !Instance || !Instance->PresentationActor)
    {
        return false;
    }

    const UStaticMeshComponent *EquipmentMesh = Instance->PresentationActor->GetEquipmentMesh();
    if (!EquipmentMesh)
    {
        return false;
    }

    const FBBBEquipDomin &EquipDomin = Definition->EquipDomin.Get();
    const FName SocketName = EquipDomin.GetLeftHandGripSocketName();
    if (!EquipmentMesh->DoesSocketExist(SocketName))
    {
        return false;
    }

    OutTransform = EquipDomin.GetLeftHandGripSocketLocalOffset()
        * EquipmentMesh->GetSocketTransform(SocketName, RTS_World);
    return true;
}

//------------------------------------------------------------------------------

bool UBBBEquipmentSystem::IsLeftHandIKEnabled() const
{
    if (!Definition || !Definition->EquipDomin.IsValid())
    {
        return false;
    }

    return Definition->EquipDomin.Get().IsLeftHandIKEnabled();
}
