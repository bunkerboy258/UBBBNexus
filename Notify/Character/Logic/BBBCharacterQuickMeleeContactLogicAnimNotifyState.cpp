#include "BBBWork/UBBBNexus/Notify/Character/Logic/BBBCharacterQuickMeleeContactLogicAnimNotifyState.h"
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "BBBWork/UBBBNexus/Character/Input/LocalControl/Interaction/FBBBCharacterMeleeBeginLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/LocalControl/Interaction/FBBBCharacterMeleeEndLocalControlPacket.h"
#include "Components/SkeletalMeshComponent.h"
void UBBBCharacterQuickMeleeContactLogicAnimNotifyState::NotifyBegin(USkeletalMeshComponent *Mesh,
                                                                     UAnimSequenceBase *Animation, float Duration,
                                                                     const FAnimNotifyEventReference &Reference)
{
    Super::NotifyBegin(Mesh, Animation, Duration, Reference);
    auto *Character = Mesh ? Cast<ABBBCharacter>(Mesh->GetOwner()) : nullptr;
    if (Character && !Character->IsNetworkMirror() &&
        Character->RuntimeData.Interaction.ReadInteractionUseState().Fact.bQuickMelee)
    {
        const int32 Token = Character->GetCharacterUseSerial();
        Tokens.Add(Mesh, Token);
        Character->SubmitInput(FBBBCharacterMeleeBeginLocalControlPacket{{Token}});
    }
}
void UBBBCharacterQuickMeleeContactLogicAnimNotifyState::NotifyEnd(USkeletalMeshComponent *Mesh,
                                                                   UAnimSequenceBase *Animation,
                                                                   const FAnimNotifyEventReference &Reference)
{
    Super::NotifyEnd(Mesh, Animation, Reference);
    auto *Character = Mesh ? Cast<ABBBCharacter>(Mesh->GetOwner()) : nullptr;
    if (Character && !Character->IsNetworkMirror() && Tokens.Contains(Mesh))
    {
        Character->SubmitInput(FBBBCharacterMeleeEndLocalControlPacket{{Tokens.FindRef(Mesh)}});
    }
    Tokens.Remove(Mesh);
}
