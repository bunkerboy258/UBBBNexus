#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/BBBRifleEquipment.h"

#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "BBBWork/UBBBNexus/Equipment/BBBEquipmentAnimInstance.h"
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

void ABBBRifleEquipment::SubmitCommand(const FBBBEquipmentCommand &Command)
{
    switch (Command.Type)
    {
        case EBBBEquipmentCommandType::Equip:
            Runtime.SubmitInput(FBBBRifleEquipInput{Command.Sequence});
            return;

        case EBBBEquipmentCommandType::Fire:
            Runtime.SubmitInput(FBBBRifleFireInput{Command.Sequence});
            return;

        case EBBBEquipmentCommandType::Reload:
            Runtime.SubmitInput(FBBBRifleReloadInput{Command.Sequence});
            return;

        case EBBBEquipmentCommandType::DetachMagazine:
            Runtime.SubmitInput(FBBBRifleDetachMagazineInput{Command.Sequence});
            return;

        case EBBBEquipmentCommandType::LoadMagazine:
            Runtime.SubmitInput(FBBBRifleLoadMagazineInput{Command.Sequence});
            return;

        case EBBBEquipmentCommandType::CancelReload:
            Runtime.SubmitInput(FBBBRifleCancelReloadInput{Command.Sequence});
            return;

        case EBBBEquipmentCommandType::Fact:
            Runtime.SubmitInput(FBBBRifleFactInput{Command.Fact});
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
        DeltaSeconds,
        IsMirror()};
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
