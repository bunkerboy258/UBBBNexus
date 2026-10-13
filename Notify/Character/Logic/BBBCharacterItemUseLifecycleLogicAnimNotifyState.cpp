#include "BBBWork/UBBBNexus/Notify/Character/Logic/BBBCharacterItemUseLifecycleLogicAnimNotifyState.h"
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "BBBWork/UBBBNexus/Character/Input/LocalControl/Interaction/FBBBCharacterInteractionFinishLocalControlPacket.h"
#include "Components/SkeletalMeshComponent.h"

void UBBBCharacterItemUseLifecycleLogicAnimNotifyState::NotifyBegin(USkeletalMeshComponent *Mesh, UAnimSequenceBase *Animation, float Duration, const FAnimNotifyEventReference &Reference)
{
    Super::NotifyBegin(Mesh, Animation, Duration, Reference);
    auto *Character = Mesh ? Cast<ABBBCharacter>(Mesh->GetOwner()) : nullptr;
    if (!Character || Character->IsNetworkMirror() || !Character->GetHeldItem()
        || Character->GetHeldUseSerial() <= 0)
    {
        return;
    }
    Recipients.Add(Mesh, Character->GetHeldItem());
    Generations.Add(Mesh, Character->GetItemGeneration());
    Tokens.Add(Mesh, Character->GetHeldUseSerial());
    Instances.Add(Mesh, Character->GetHeldItemIdentity());
}

void UBBBCharacterItemUseLifecycleLogicAnimNotifyState::NotifyEnd(USkeletalMeshComponent *Mesh, UAnimSequenceBase *Animation, const FAnimNotifyEventReference &Reference)
{
    Super::NotifyEnd(Mesh, Animation, Reference);
    auto *Character = Mesh ? Cast<ABBBCharacter>(Mesh->GetOwner()) : nullptr;
    if (Character && !Character->IsNetworkMirror() && Recipients.Contains(Mesh))
    {
        Character->SubmitInput(FBBBCharacterInteractionFinishLocalControlPacket{
            {Recipients.FindRef(Mesh)}, {Generations.FindRef(Mesh)}, {Tokens.FindRef(Mesh)}, {Instances.FindRef(Mesh)}});
    }
    Recipients.Remove(Mesh);
    Generations.Remove(Mesh);
    Tokens.Remove(Mesh);
    Instances.Remove(Mesh);
}
