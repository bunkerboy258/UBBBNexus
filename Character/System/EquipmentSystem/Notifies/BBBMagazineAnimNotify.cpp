#include "BBBWork/UBBBNexus/Character/System/EquipmentSystem/Notifies/BBBMagazineAnimNotify.h"

#include "BBBWork/UBBBNexus/Character/System/AnimationSystem/BBBAnimInstance.h"
#include "Components/SkeletalMeshComponent.h"

void UBBBMagazineAnimNotify::Notify(
    USkeletalMeshComponent *MeshComp,
    UAnimSequenceBase *Animation,
    const FAnimNotifyEventReference &EventReference)
{
    Super::Notify(MeshComp, Animation, EventReference);

    UBBBAnimInstance *AnimInstance = MeshComp
        ? Cast<UBBBAnimInstance>(MeshComp->GetAnimInstance())
        : nullptr;

    if (!AnimInstance)
    {
        return;
    }

    if (Action == EBBBMagazineAnimNotifyAction::Remove)
    {
        AnimInstance->SubmitRemoveMagazine();
        return;
    }

    if (Action == EBBBMagazineAnimNotifyAction::Spawn)
    {
        AnimInstance->SubmitSpawnMagazine();
    }
}

//------------------------------------------------------------------------------

FString UBBBMagazineAnimNotify::GetNotifyName_Implementation() const
{
    if (Action == EBBBMagazineAnimNotifyAction::Remove)
    {
        return TEXT("BBB Remove Magazine");
    }

    return TEXT("BBB Spawn Magazine");
}
