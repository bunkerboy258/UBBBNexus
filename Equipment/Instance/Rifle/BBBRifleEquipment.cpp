#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/BBBRifleEquipment.h"

#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "BBBWork/UBBBNexus/Equipment/Base/BBBEquipmentAnimInstance.h"
#include "BBBWork/UBBBNexus/Equipment/Base/Definition/BBBEquipmentAnimationFacts.h"
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

    Runtime.Initialize(InDefinition);
    return true;
}

void ABBBRifleEquipment::SubmitCommand(const FBBBEquipmentCommand &Command, const bool bInIsMirror)
{
    switch (Command.Type)
    {
        case EBBBEquipmentCommandType::Equip:
            Runtime.SubmitInput(FBBBEquipmentEquipInput{Command.Sequence}, bInIsMirror);
            return;

        case EBBBEquipmentCommandType::Primary:
            Runtime.SubmitInput(FBBBEquipmentPrimaryInput{Command.Sequence}, bInIsMirror);
            return;

        case EBBBEquipmentCommandType::Secondary:
            Runtime.SubmitInput(FBBBEquipmentSecondaryInput{Command.bActive, Command.Sequence}, bInIsMirror);
            return;

        case EBBBEquipmentCommandType::Reload:
            Runtime.SubmitInput(FBBBEquipmentReloadInput{Command.Sequence}, bInIsMirror);
            return;

        case EBBBEquipmentCommandType::DetachMagazine:
            Runtime.SubmitInput(FBBBEquipmentDetachMagazineInput{Command.Sequence}, bInIsMirror);
            return;

        case EBBBEquipmentCommandType::LoadMagazine:
            Runtime.SubmitInput(FBBBEquipmentLoadMagazineInput{Command.Sequence}, bInIsMirror);
            return;

        case EBBBEquipmentCommandType::InterruptReload:
            Runtime.SubmitInput(FBBBEquipmentInterruptReloadInput{Command.Sequence}, bInIsMirror);
            return;

        case EBBBEquipmentCommandType::Fact:
            Runtime.SubmitInput(FBBBEquipmentFactInput{Command.Fact}, bInIsMirror);
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
    Runtime.Update(Context);

    UBBBEquipmentAnimInstance *AnimationInstance = Cast<UBBBEquipmentAnimInstance>(WeaponMesh->GetAnimInstance());
    if (!AnimationInstance)
    {
        return;
    }

    const FBBBRifleState &State = Runtime.GetState();
    FBBBEquipmentAnimationFacts Facts;
    Facts.bIsReloading = State.bIsReloading;
    Facts.FireSequence = State.FireSequence;
    Facts.LastFireTimeSeconds = State.LastFireTimeSeconds;
    Facts.LoadedAmmo = State.LoadedAmmo;
    Facts.AmmoCapacity = State.AmmoCapacity;
    Facts.CurrentWorldTimeSeconds = GetWorld() ? GetWorld()->GetTimeSeconds() : 0.0f;
    AnimationInstance->PublishAnimationFacts(Facts);
}
