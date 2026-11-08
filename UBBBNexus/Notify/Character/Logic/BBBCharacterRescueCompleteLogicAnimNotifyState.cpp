#include "BBBWork/UBBBNexus/Notify/Character/Logic/BBBCharacterRescueCompleteLogicAnimNotifyState.h"
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "BBBWork/UBBBNexus/Character/Input/LocalControl/Life/FBBBCharacterRescueCompleteLocalControlPacket.h"
#include "Animation/ActiveMontageInstanceScope.h"
#include "Animation/AnimNotifyLibrary.h"
#include "Components/SkeletalMeshComponent.h"

void UBBBCharacterRescueCompleteLogicAnimNotifyState::NotifyBegin(USkeletalMeshComponent *Mesh,
    UAnimSequenceBase *Animation, float Duration, const FAnimNotifyEventReference &Reference)
{
    Super::NotifyBegin(Mesh, Animation, Duration, Reference);
    auto *Character = Mesh ? Cast<ABBBCharacter>(Mesh->GetOwner()) : nullptr;
    const auto *Instance = Reference.GetContextData<UE::Anim::FAnimNotifyMontageInstanceContext>();
    if (!Character || Character->IsNetworkMirror() || !Instance)
    {
        return;
    }
    const auto &Rescue = Character->RuntimeData.Life.ReadRescueState();
    if (!Rescue.bReceiving || !Rescue.bAccepted || Rescue.OperationId == 0 || Rescue.DownedRevision == 0)
    {
        return;
    }
    Operations.FindOrAdd(Mesh).Add(Instance->MontageInstanceID, Rescue.OperationId);
    Rounds.FindOrAdd(Mesh).Add(Instance->MontageInstanceID, Rescue.DownedRevision);
}

void UBBBCharacterRescueCompleteLogicAnimNotifyState::NotifyEnd(USkeletalMeshComponent *Mesh,
    UAnimSequenceBase *Animation, const FAnimNotifyEventReference &Reference)
{
    Super::NotifyEnd(Mesh, Animation, Reference);
    auto *Character = Mesh ? Cast<ABBBCharacter>(Mesh->GetOwner()) : nullptr;
    const auto *Instance = Reference.GetContextData<UE::Anim::FAnimNotifyMontageInstanceContext>();
    auto *MeshOperations = Operations.Find(Mesh);
    auto *MeshRounds = Rounds.Find(Mesh);
    if (!Instance || !MeshOperations || !MeshRounds)
    {
        return;
    }
    const uint64 Operation = MeshOperations->FindRef(Instance->MontageInstanceID);
    const uint64 Round = MeshRounds->FindRef(Instance->MontageInstanceID);
    MeshOperations->Remove(Instance->MontageInstanceID);
    MeshRounds->Remove(Instance->MontageInstanceID);
    if (MeshOperations->IsEmpty())
    {
        Operations.Remove(Mesh);
    }
    if (MeshRounds->IsEmpty())
    {
        Rounds.Remove(Mesh);
    }
    if (Character && !Character->IsNetworkMirror() && Operation != 0 && Round != 0
        && UAnimNotifyLibrary::NotifyStateReachedEnd(Reference))
    {
        Character->SubmitInput(FBBBCharacterRescueCompleteLocalControlPacket{Operation, Round});
    }
}
