#include "BBBWork/UBBBNexus/Notify/Character/Logic/BBBCharacterGetUpCompleteLogicAnimNotifyState.h"
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "BBBWork/UBBBNexus/Character/Input/LocalControl/PhysicalPresentation/FBBBCharacterGetUpCompleteLocalControlPacket.h"
#include "Animation/ActiveMontageInstanceScope.h"
#include "Animation/AnimMontage.h"
#include "Animation/AnimNotifyLibrary.h"
#include "Components/SkeletalMeshComponent.h"

void UBBBCharacterGetUpCompleteLogicAnimNotifyState::NotifyBegin(USkeletalMeshComponent *Mesh,
    UAnimSequenceBase *Animation, float Duration, const FAnimNotifyEventReference &Reference)
{
    Super::NotifyBegin(Mesh, Animation, Duration, Reference);
    auto *Character = Mesh ? Cast<ABBBCharacter>(Mesh->GetOwner()) : nullptr;
    const auto *Instance = Reference.GetContextData<UE::Anim::FAnimNotifyMontageInstanceContext>();
    if (!Character || Character->IsNetworkMirror() || !Instance)
    {
        return;
    }
    const auto &Result = Character->RuntimeData.PhysicalPresentation.ReadPhysicalPresentationState().RagdollSnapshot;
    if (Result.Phase != EBBBCharacterPhysicalPhase::Recovering || Result.Episode == 0 || Animation != Result.GetUpMontage)
    {
        return;
    }
    for (auto It = Episodes.CreateIterator(); It; ++It)
    {
        if (!It.Key().IsValid())
        {
            It.RemoveCurrent();
        }
    }
    Episodes.FindOrAdd(Mesh).Add(Instance->MontageInstanceID, Result.Episode);
}

void UBBBCharacterGetUpCompleteLogicAnimNotifyState::NotifyEnd(USkeletalMeshComponent *Mesh,
    UAnimSequenceBase *Animation, const FAnimNotifyEventReference &Reference)
{
    Super::NotifyEnd(Mesh, Animation, Reference);
    auto *Character = Mesh ? Cast<ABBBCharacter>(Mesh->GetOwner()) : nullptr;
    const auto *Instance = Reference.GetContextData<UE::Anim::FAnimNotifyMontageInstanceContext>();
    auto *Bound = Episodes.Find(Mesh);
    if (!Instance || !Bound)
    {
        return;
    }
    const uint64 Episode = Bound->FindRef(Instance->MontageInstanceID);
    Bound->Remove(Instance->MontageInstanceID);
    if (Bound->IsEmpty())
    {
        Episodes.Remove(Mesh);
    }
    if (Character && !Character->IsNetworkMirror() && Episode != 0
        && UAnimNotifyLibrary::NotifyStateReachedEnd(Reference))
    {
        Character->SubmitInput(FBBBCharacterGetUpCompleteLocalControlPacket{{Episode}});
    }
}
