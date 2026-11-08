#include "BBBWork/UBBBNexus/Notify/Character/Logic/BBBCharacterItemUseLifecycleLogicAnimNotifyState.h"
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "BBBWork/UBBBNexus/Character/Input/LocalControl/Interaction/FBBBCharacterInteractionFinishLocalControlPacket.h"
#include "Components/SkeletalMeshComponent.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Base/BBBConsumable.h"

void UBBBCharacterItemUseLifecycleLogicAnimNotifyState::NotifyBegin(USkeletalMeshComponent *Mesh, UAnimSequenceBase *Animation, float Duration, const FAnimNotifyEventReference &Reference)
{
    Super::NotifyBegin(Mesh, Animation, Duration, Reference);
    auto *Character = Mesh ? Cast<ABBBCharacter>(Mesh->GetOwner()) : nullptr;
    if (!Character || Character->IsNetworkMirror() ||
        (!Character->GetActiveConsumable() &&
         !Character->RuntimeData.Interaction.ReadInteractionUseState().Fact.bQuickMelee) ||
        Character->GetCharacterUseSerial() <= 0)
    {
        return;
    }
    Recipients.Add(Mesh, Character->GetActiveConsumable() ? static_cast<UObject *>(Character->GetActiveConsumable())
                                                          : Character);
    Generations.Add(Mesh, Character->GetCharacterUseSerial());
    Tokens.Add(Mesh, Character->GetCharacterUseSerial());
    Instances.Add(Mesh, Character->RuntimeData.Interaction.ReadInteractionUseState().Instance);
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
