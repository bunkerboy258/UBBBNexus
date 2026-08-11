#include "BBBWork/UBBBNexus/Equipment/Animation/Notifies/BBBMagazineAnimNotify.h"

#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "Components/SkeletalMeshComponent.h"

void UBBBMagazineAnimNotify::Notify(
    USkeletalMeshComponent *MeshComp,
    UAnimSequenceBase *Animation,
    const FAnimNotifyEventReference &EventReference)
{
    Super::Notify(MeshComp, Animation, EventReference);

    ABBBCharacter *Character = MeshComp
        ? Cast<ABBBCharacter>(MeshComp->GetOwner())
        : nullptr;

    if (!Character)
    {
        return;
    }

    if (Action == EBBBMagazineAnimNotifyAction::Remove)
    {
        Character->EquipmentSystem.RemoveMagazinePresentation();
        return;
    }

    if (Action == EBBBMagazineAnimNotifyAction::Spawn)
    {
        Character->EquipmentSystem.SpawnMagazinePresentation();
    }
}

//------------------------------------------------------------------------------

FString UBBBMagazineAnimNotify::GetNotifyName_Implementation() const
{
    if (Action == EBBBMagazineAnimNotifyAction::Remove)
    {
        return TEXT("BBB Remove Magazine");
    }

    return TEXT("BBB Spawn Magazine");
}
