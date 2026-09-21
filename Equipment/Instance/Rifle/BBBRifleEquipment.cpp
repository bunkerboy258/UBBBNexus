#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/BBBRifleEquipment.h"

#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "BBBWork/UBBBNexus/Equipment/Base/BBBEquipmentAnimInstance.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/Context/BBBRiflePresentationContext.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/Definition/BBBRifleDefinition.h"
#include "Components/SkeletalMeshComponent.h"

bool ABBBRifleEquipment::InitializeRifle(
    UBBBRifleDefinition &InDefinition,
    const FGuid &InInstanceId,
    const bool bInIsMirror)
{
    if (!InitializeEquipment(InDefinition, InInstanceId, bInIsMirror))
    {
        return false;
    }

    TBBBEquipmentRuntime<FBBBRifleSignature>::Initialize(RuntimeData, InDefinition);
    return true;
}

void ABBBRifleEquipment::SubmitCommand(const FBBBEquipmentCommand &Command, const bool bInIsMirror)
{
    switch (Command.Type)
    {
        case EBBBEquipmentCommandType::Equip:
            TBBBEquipmentRuntime<FBBBRifleSignature>::SubmitInput(
                RuntimeData,
                FBBBEquipmentEquipInput{Command.Sequence},
                bInIsMirror);
            return;

        case EBBBEquipmentCommandType::Primary:
            TBBBEquipmentRuntime<FBBBRifleSignature>::SubmitInput(
                RuntimeData,
                FBBBEquipmentPrimaryInput{Command.Sequence},
                bInIsMirror);
            return;

        case EBBBEquipmentCommandType::Secondary:
            TBBBEquipmentRuntime<FBBBRifleSignature>::SubmitInput(
                RuntimeData,
                FBBBEquipmentSecondaryInput{Command.bActive, Command.Sequence},
                bInIsMirror);
            return;

        case EBBBEquipmentCommandType::Reload:
            TBBBEquipmentRuntime<FBBBRifleSignature>::SubmitInput(
                RuntimeData,
                FBBBEquipmentReloadInput{Command.Sequence},
                bInIsMirror);
            return;

        case EBBBEquipmentCommandType::DetachMagazine:
            TBBBEquipmentRuntime<FBBBRifleSignature>::SubmitInput(
                RuntimeData,
                FBBBEquipmentDetachMagazineInput{Command.Sequence},
                bInIsMirror);
            return;

        case EBBBEquipmentCommandType::LoadMagazine:
            TBBBEquipmentRuntime<FBBBRifleSignature>::SubmitInput(
                RuntimeData,
                FBBBEquipmentLoadMagazineInput{Command.Sequence},
                bInIsMirror);
            return;

        case EBBBEquipmentCommandType::InterruptReload:
            TBBBEquipmentRuntime<FBBBRifleSignature>::SubmitInput(
                RuntimeData,
                FBBBEquipmentInterruptReloadInput{Command.Sequence},
                bInIsMirror);
            return;

        case EBBBEquipmentCommandType::Fact:
            TBBBEquipmentRuntime<FBBBRifleSignature>::SubmitInput(
                RuntimeData,
                FBBBEquipmentFactInput{Command.Fact},
                bInIsMirror);
            return;
    }
}

void ABBBRifleEquipment::UpdateEquipment(const float DeltaSeconds)
{
    ABBBCharacter *Character = Cast<ABBBCharacter>(GetOwner());
    USkeletalMeshComponent *WeaponMesh = GetEquipmentSkeletalMesh();
    if (!Character || !WeaponMesh)
    {
        return;
    }

    FBBBEquipmentRuntimeContext Context{
        *this,
        *Character,
        *WeaponMesh,
        DeltaSeconds};
    TBBBEquipmentRuntime<FBBBRifleSignature>::Update(RuntimeData, Context);

    UBBBEquipmentAnimInstance *AnimationInstance = Cast<UBBBEquipmentAnimInstance>(WeaponMesh->GetAnimInstance());
    if (!AnimationInstance)
    {
        return;
    }

    const FBBBRifleState &State = RuntimeData.State;
    FBBBRiflePresentationContext PresentationContext;
    PresentationContext.Facts.bIsReloading = State.bIsReloading;
    PresentationContext.Facts.FireSequence = State.FireSequence;
    PresentationContext.Facts.LastFireTimeSeconds = State.LastFireTimeSeconds;
    PresentationContext.Facts.LoadedAmmo = State.LoadedAmmo;
    PresentationContext.Facts.AmmoCapacity = State.AmmoCapacity;
    PresentationContext.Facts.CurrentWorldTimeSeconds = GetWorld() ? GetWorld()->GetTimeSeconds() : 0.0f;
    AnimationInstance->PublishAnimationFacts(PresentationContext.Facts);
}
