#include "BBBWork/UBBBNexus/Notify/Character/Logic/BBBCharacterItemReleaseUseLogicAnimNotifyState.h"
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "BBBWork/UBBBNexus/Character/Input/LocalControl/Interaction/FBBBCharacterInteractionReleaseLocalControlPacket.h"
#include "Components/SkeletalMeshComponent.h"

void UBBBCharacterItemReleaseUseLogicAnimNotifyState::NotifyBegin(USkeletalMeshComponent *Mesh, UAnimSequenceBase *Animation, float Duration, const FAnimNotifyEventReference &Reference)
{
    Super::NotifyBegin(Mesh, Animation, Duration, Reference);
    auto *Character = Mesh ? Cast<ABBBCharacter>(Mesh->GetOwner()) : nullptr;
    if (Character && !Character->IsNetworkMirror() && Character->GetHeldItem()
        && Character->GetHeldUseSerial() > 0)
    {
        Character->SubmitInput(FBBBCharacterInteractionReleaseLocalControlPacket{
            {Character->GetHeldItem()}, {Character->GetItemGeneration()}, {Character->GetHeldUseSerial()}});
    }
}
