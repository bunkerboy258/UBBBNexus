#include "BBBWork/UBBBNexus/Character/Notify/Animation/Reload/BBBReloadLoadAnimNotify.h"

#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "BBBWork/UBBBNexus/Character/Input/Packets/Event/Animation/BBBReloadLoadPacket.h"
#include "Components/SkeletalMeshComponent.h"

void UBBBReloadLoadAnimNotify::Notify(
    USkeletalMeshComponent *MeshComp,
    UAnimSequenceBase *,
    const FAnimNotifyEventReference &)
{
    if (!MeshComp)
    {
        return;
    }

    ABBBCharacter *Character = Cast<ABBBCharacter>(MeshComp->GetOwner());
    if (!Character)
    {
        return;
    }

    Character->SubmitInput(FBBBReloadLoadPacket());
}
