#include "BBBWork/UBBBNexus/Character/Notify/Animation/Reload/BBBReloadLifecycleAnimNotifyState.h"

#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "BBBWork/UBBBNexus/Character/Input/Packets/Event/Animation/BBBReloadInterruptPacket.h"
#include "Components/SkeletalMeshComponent.h"

void UBBBReloadLifecycleAnimNotifyState::NotifyEnd(
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

    Character->SubmitInput(FBBBReloadInterruptPacket());
}
