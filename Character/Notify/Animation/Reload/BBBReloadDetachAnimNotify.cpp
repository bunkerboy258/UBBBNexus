#include "BBBWork/UBBBNexus/Character/Notify/Animation/Reload/BBBReloadDetachAnimNotify.h"

#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "BBBWork/UBBBNexus/Character/Input/Packets/Event/Animation/BBBReloadDetachPacket.h"
#include "Components/SkeletalMeshComponent.h"

void UBBBReloadDetachAnimNotify::Notify(
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

    Character->SubmitInput(FBBBReloadDetachPacket());
}
