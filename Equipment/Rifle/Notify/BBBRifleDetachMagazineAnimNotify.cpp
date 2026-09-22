#include "BBBWork/UBBBNexus/Equipment/Rifle/Notify/BBBRifleDetachMagazineAnimNotify.h"

#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "BBBWork/UBBBNexus/Equipment/Rifle/BBBRifleEquipment.h"
#include "Components/SkeletalMeshComponent.h"

void UBBBRifleDetachMagazineAnimNotify::Notify(
    USkeletalMeshComponent *MeshComp,
    UAnimSequenceBase *,
    const FAnimNotifyEventReference &)
{
    ABBBCharacter *Character = MeshComp ? Cast<ABBBCharacter>(MeshComp->GetOwner()) : nullptr;
    ABBBRifleEquipment *Rifle = Character ? Cast<ABBBRifleEquipment>(Character->GetActiveEquipment()) : nullptr;
    if (!Rifle || Rifle->IsMirror())
    {
        return;
    }

    Rifle->SubmitDetachMagazineInput(INDEX_NONE, false);
}
