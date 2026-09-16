#include "BBBWork/UBBBNexus/Character/System/EquipmentSystem/Processors/BBBCharacterEquipmentSelectionProcessor.h"

#include "BBBWork/UBBBNexus/Character/BBBCharacterInstance.h"
#include "BBBWork/UBBBNexus/Character/BBBAnimInstance.h"
#include "BBBWork/UBBBNexus/Character/System/EquipmentSystem/Definition/Commands/BBBCharacterEquipmentCommands.h"
#include "BBBWork/UBBBNexus/Character/System/EquipmentSystem/Definition/Events/BBBCharacterEquipmentEvents.h"
#include "BBBWork/UBBBNexus/Character/System/EquipmentSystem/Definition/States/BBBCharacterEquipmentStates.h"
#include "BBBWork/UBBBNexus/Equipment/BBBEquipmentInstance.h"
#include "BBBWork/UBBBNexus/Equipment/BBBEquipmentAnimInstance.h"
#include "Components/SkeletalMeshComponent.h"

void FBBBCharacterEquipmentSelectionProcessor::UnbindWeaponAnimation(
    USkeletalMeshComponent &CharacterMesh,
    ABBBEquipmentInstance &Equipment)
{
    USkeletalMeshComponent *WeaponMesh = Equipment.GetEquipmentSkeletalMesh();
    if (WeaponMesh)
    {
        WeaponMesh->PrimaryComponentTick.RemovePrerequisite(
            &CharacterMesh,
            CharacterMesh.PrimaryComponentTick);
    }

    UBBBAnimInstance *CharacterAnim = Cast<UBBBAnimInstance>(CharacterMesh.GetAnimInstance());
    UBBBEquipmentAnimInstance *WeaponAnim = WeaponMesh
        ? Cast<UBBBEquipmentAnimInstance>(WeaponMesh->GetAnimInstance())
        : nullptr;
    if (CharacterAnim && CharacterAnim->TryGetWeaponAnimInstance() == WeaponAnim)
    {
        CharacterAnim->BindWeaponAnimInstance(nullptr);
    }
}

bool FBBBCharacterEquipmentSelectionProcessor::BindWeaponAnimation(
    USkeletalMeshComponent &CharacterMesh,
    ABBBEquipmentInstance &Equipment)
{
    USkeletalMeshComponent *WeaponMesh = Equipment.GetEquipmentSkeletalMesh();
    UBBBEquipmentAnimInstance *WeaponAnim = WeaponMesh
        ? Cast<UBBBEquipmentAnimInstance>(WeaponMesh->GetAnimInstance())
        : nullptr;
    UBBBAnimInstance *CharacterAnim = Cast<UBBBAnimInstance>(CharacterMesh.GetAnimInstance());
    if (!ensureMsgf(
        CharacterAnim && WeaponAnim,
        TEXT("[UBBBC]Character and weapon animation instances must use BBB base classes")))
    {
        return false;
    }

    CharacterAnim->BindWeaponAnimInstance(WeaponAnim);
    WeaponMesh->PrimaryComponentTick.AddPrerequisite(
        &CharacterMesh,
        CharacterMesh.PrimaryComponentTick);
    return true;
}

void FBBBCharacterEquipmentSelectionProcessor::Update(
    ABBBCharacterInstance &Character,
    USkeletalMeshComponent &CharacterMesh,
    const FName AttachmentSocketName,
    FBBBCharacterEquipmentCommands &EquipmentCommands,
    FBBBCharacterEquipmentState &EquipmentState,
    FBBBCharacterEquipmentEvents &EquipmentEvents) const
{
    bool bRestoringEquipment = false;
    UBBBEquipmentDefinition *RestoredDefinition = EquipmentCommands.ConsumeRestoredEquipment();
    if (RestoredDefinition)
    {
        ABBBEquipmentInstance *RestoredInstance = ABBBEquipmentInstance::Create(
            Character,
            *RestoredDefinition,
            true);
        if (!ensureMsgf(RestoredInstance, TEXT("[UBBBC]Restored equipment instance creation failed")))
        {
            return;
        }

        if (!RestoredInstance->BindHolder(CharacterMesh, AttachmentSocketName))
        {
            RestoredInstance->Shutdown();
            return;
        }

        EquipmentState.DesiredMainHandInstance = RestoredInstance;
        bRestoringEquipment = true;
    }

    if (EquipmentState.ActiveMainHandInstance == EquipmentState.DesiredMainHandInstance)
    {
        return;
    }

    if (EquipmentState.ActiveMainHandInstance)
    {
        UnbindWeaponAnimation(CharacterMesh, *EquipmentState.ActiveMainHandInstance);
        EquipmentState.ActiveMainHandInstance->Deactivate();
    }

    EquipmentState.ReloadSequence = INDEX_NONE;
    EquipmentState.ActiveMainHandInstance = EquipmentState.DesiredMainHandInstance;
    ABBBEquipmentInstance *DesiredInstance = EquipmentState.ActiveMainHandInstance;
    if (!DesiredInstance)
    {
        return;
    }

    if (!DesiredInstance->BindHolder(CharacterMesh, AttachmentSocketName)
        || !DesiredInstance->Activate()
        || !BindWeaponAnimation(CharacterMesh, *DesiredInstance))
    {
        DesiredInstance->Deactivate();
        EquipmentState.ActiveMainHandInstance = nullptr;
        return;
    }

    if (bRestoringEquipment)
    {
        return;
    }

    DesiredInstance->GetExternalAPI().SubmitEquip(EquipmentState.NextActionSequence++);
}
