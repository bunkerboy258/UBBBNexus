#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/Notify/BBBRifleLoadMagazineAnimNotify.h"

#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/BBBRifleEquipment.h"
#include "Components/SkeletalMeshComponent.h"

void UBBBRifleLoadMagazineAnimNotify::Notify(
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

    Rifle->SubmitCommand(
        FBBBEquipmentCommand{EBBBEquipmentCommandType::LoadMagazine, INDEX_NONE},
        false);
}
