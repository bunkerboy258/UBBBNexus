#include "BBBWork/UBBBNexus/Notify/Character/Display/BBBCharacterHeldObjectDisplayAnimNotifyState.h"

#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/Actor.h"

void UBBBCharacterHeldObjectDisplayAnimNotifyState::NotifyBegin(
    USkeletalMeshComponent *MeshComp,
    UAnimSequenceBase *,
    float,
    const FAnimNotifyEventReference &)
{
    if (!MeshComp || !MeshComp->GetOwner() || !ObjectMesh
        || MeshComp->GetBoneIndex(HandBoneName) == INDEX_NONE || HandObjectTransform.ContainsNaN())
    {
        return;
    }

    const FName DisplayTag(*GetPathName());
    TArray<USceneComponent *> Children;
    MeshComp->GetChildrenComponents(false, Children);

    for (USceneComponent *Child : Children)
    {
        if (Child && Child->ComponentHasTag(DisplayTag))
        {
            return;
        }
    }

    UStaticMeshComponent *Display = NewObject<UStaticMeshComponent>(MeshComp->GetOwner(), NAME_None, RF_Transient);
    Display->ComponentTags.Add(DisplayTag);
    Display->SetStaticMesh(ObjectMesh);
    Display->SetMobility(EComponentMobility::Movable);
    Display->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    Display->RegisterComponent();

    if (!Display->IsRegistered()
        || !Display->AttachToComponent(MeshComp, FAttachmentTransformRules::SnapToTargetNotIncludingScale, HandBoneName))
    {
        Display->DestroyComponent();
        return;
    }

    Display->SetRelativeTransform(HandObjectTransform);
}

void UBBBCharacterHeldObjectDisplayAnimNotifyState::NotifyEnd(
    USkeletalMeshComponent *MeshComp,
    UAnimSequenceBase *,
    const FAnimNotifyEventReference &)
{
    if (!MeshComp)
    {
        return;
    }

    const FName DisplayTag(*GetPathName());
    TArray<USceneComponent *> Children;
    MeshComp->GetChildrenComponents(false, Children);

    for (USceneComponent *Child : Children)
    {
        if (Child && Child->ComponentHasTag(DisplayTag))
        {
            Child->DestroyComponent();
        }
    }
}
