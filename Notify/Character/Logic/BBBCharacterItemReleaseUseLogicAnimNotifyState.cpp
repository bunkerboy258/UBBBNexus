#include "BBBWork/UBBBNexus/Notify/Character/Logic/BBBCharacterItemReleaseUseLogicAnimNotifyState.h"
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "BBBWork/UBBBNexus/Character/Input/LocalControl/Interaction/FBBBCharacterInteractionReleaseLocalControlPacket.h"
#include "Components/SkeletalMeshComponent.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Base/BBBConsumable.h"

void UBBBCharacterItemReleaseUseLogicAnimNotifyState::NotifyBegin(USkeletalMeshComponent *Mesh, UAnimSequenceBase *Animation, float Duration, const FAnimNotifyEventReference &Reference)
{
    Super::NotifyBegin(Mesh, Animation, Duration, Reference);
    auto *Character = Mesh ? Cast<ABBBCharacter>(Mesh->GetOwner()) : nullptr;
    if (Character && !Character->IsNetworkMirror() && Character->GetActiveConsumable() &&
        Character->GetCharacterUseSerial() > 0)
    {
        Character->SubmitInput(
            FBBBCharacterInteractionReleaseLocalControlPacket{{Character->GetActiveConsumable()},
                                                              {static_cast<uint64>(Character->GetCharacterUseSerial())},
                                                              {Character->GetCharacterUseSerial()}});
    }
}
