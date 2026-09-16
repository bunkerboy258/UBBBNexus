#include "BBBWork/UBBBNexus/Character/System/EquipmentSystem/Processors/BBBCharacterEquipmentSelectionProcessor.h"

#include "BBBWork/UBBBNexus/Character/BBBCharacterInstance.h"
#include "BBBWork/UBBBNexus/Character/BBBAnimInstance.h"
#include "BBBWork/UBBBNexus/Character/System/EquipmentSystem/Definition/Commands/BBBCharacterEquipmentCommands.h"
#include "BBBWork/UBBBNexus/Character/System/EquipmentSystem/Definition/Events/BBBCharacterEquipmentEvents.h"
#include "BBBWork/UBBBNexus/Character/System/EquipmentSystem/Definition/States/BBBCharacterEquipmentStates.h"
#include "BBBWork/UBBBNexus/Equipment/BBBEquipmentInstance.h"
#include "BBBWork/UBBBNexus/Equipment/BBBEquipmentAnimInstance.h"
#include "Components/SkeletalMeshComponent.h"

namespace
{
void UnbindWeaponAnimation(
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

bool BindWeaponAnimation(
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
}

void FBBBCharacterEquipmentSelectionProcessor::Update(
    ABBBCharacterInstance &Character,
    USkeletalMeshComponent &CharacterMesh,
    const FName AttachmentSocketName,
    const float WorldTimeSeconds,
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

    if (EquipmentState.ActionState.IsActive()
        || (EquipmentState.ActiveMainHandInstance && EquipmentState.ActiveMainHandInstance->IsReloading()))
    {
        return;
    }

    if (EquipmentState.ActiveMainHandInstance)
    {
        UnbindWeaponAnimation(CharacterMesh, *EquipmentState.ActiveMainHandInstance);
        EquipmentState.ActiveMainHandInstance->Deactivate();
    }

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

    const int32 Sequence = EquipmentState.NextActionSequence++;
    FBBBEquipmentActionResult Result;
    if (!DesiredInstance->BeginEquipAction(Sequence, 0.0f, Result))
    {
        return;
    }

    EquipmentState.ActionState.Begin(
        EBBBCharacterActionType::Equip,
        WorldTimeSeconds,
        Result.DurationSeconds,
        Sequence);

    FBBBEquipmentActionEvent Event;
    Event.ActionType = EBBBCharacterActionType::Equip;
    Event.EquipmentId = DesiredInstance->GetEquipmentId();
    Event.Sequence = Sequence;
    Event.DurationSeconds = Result.DurationSeconds;
    EquipmentEvents.AddAction(MoveTemp(Event));
}
