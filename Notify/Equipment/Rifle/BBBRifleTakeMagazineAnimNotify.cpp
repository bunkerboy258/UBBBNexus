#include "BBBWork/UBBBNexus/Notify/Equipment/Rifle/BBBRifleTakeMagazineAnimNotify.h"

#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/BBBRifleEquipment.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/Input/LocalControl/Reload/FBBBRifleTakeMagazineLocalControlPacket.h"
#include "Components/SkeletalMeshComponent.h"

void UBBBRifleTakeMagazineAnimNotify::Notify(
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

    ensureMsgf(Rifle->SubmitInput(FBBBRifleTakeMagazineLocalControlPacket{}),
        TEXT("步枪拿匣通知未被当前装备接受"));
}
