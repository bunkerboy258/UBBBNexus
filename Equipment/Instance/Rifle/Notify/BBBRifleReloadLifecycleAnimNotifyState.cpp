#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/Notify/BBBRifleReloadLifecycleAnimNotifyState.h"

#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/BBBRifleEquipment.h"
#include "Components/SkeletalMeshComponent.h"

void UBBBRifleReloadLifecycleAnimNotifyState::NotifyEnd(
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
        FBBBEquipmentCommand{EBBBEquipmentCommandType::InterruptReload, INDEX_NONE},
        false);
}
