#include "BBBWork/UBBBNexus/Notify/Character/Logic/BBBCharacterRecoveryEffectLogicAnimNotifyState.h"
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Base/BBBConsumable.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Base/Config/BBBConsumableDefinition.h"
#include "BBBWork/UBBBNexus/Character/Input/LocalControl/Interaction/FBBBCharacterInteractionReleaseLocalControlPacket.h"
#include "Components/SkeletalMeshComponent.h"
void UBBBCharacterRecoveryEffectLogicAnimNotifyState::NotifyBegin(USkeletalMeshComponent *Mesh,
                                                                  UAnimSequenceBase *Animation, float Duration,
                                                                  const FAnimNotifyEventReference &Reference)
{
    Super::NotifyBegin(Mesh, Animation, Duration, Reference);
    auto *Character = Mesh ? Cast<ABBBCharacter>(Mesh->GetOwner()) : nullptr;
    const auto *Definition =
        Character ? Character->RuntimeData.Interaction.ReadInteractionUseState().Definition.Get() : nullptr;
    if (Character && !Character->IsNetworkMirror() && Character->GetActiveConsumable() && Definition &&
        !Definition->bThrowable)
    {
        const int32 Serial = Character->GetCharacterUseSerial();
        Character->SubmitInput(FBBBCharacterInteractionReleaseLocalControlPacket{
            {Character->GetActiveConsumable()}, {static_cast<uint64>(Serial)}, {Serial}});
    }
}
