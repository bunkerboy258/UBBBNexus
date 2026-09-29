#include "BBBWork/UBBBNexus/Notify/Character/Display/BBBCharacterNewMagazineDisplayAnimNotifyState.h"

#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"

namespace
{
    const FName NewMagazineTag(TEXT("BBB_CharacterNewHandMagazine"));

    UStaticMeshComponent *FindNewMagazine(USkeletalMeshComponent &HandMesh)
    {
        TArray<USceneComponent *> Children;
        HandMesh.GetChildrenComponents(false, Children);

        for (USceneComponent *Child : Children)
        {
            UStaticMeshComponent *Magazine = Cast<UStaticMeshComponent>(Child);
            if (Magazine && Magazine->ComponentHasTag(NewMagazineTag))
            {
                return Magazine;
            }
        }

        return nullptr;
    }
}

void UBBBCharacterNewMagazineDisplayAnimNotifyState::NotifyBegin(
    USkeletalMeshComponent *MeshComp,
    UAnimSequenceBase *,
    float,
    const FAnimNotifyEventReference &)
{
    ABBBCharacter *Character = MeshComp ? Cast<ABBBCharacter>(MeshComp->GetOwner()) : nullptr;
    if (!Character)
    {
        return;
    }

    if (!ensureMsgf(IsInGameThread() && MagazineMesh
        && MeshComp->GetBoneIndex(HandBoneName) != INDEX_NONE
        && !HandMagazineTransform.ContainsNaN()
        && !FindNewMagazine(*MeshComp),
        TEXT("新弹匣角色表现缺少网格 骨骼 位置或存在重复组件 %s"), *Character->GetName()))
    {
        return;
    }

    // 新弹匣仅是角色动画的临时视觉对象 不读取装备也不改变装填状态
    UStaticMeshComponent *Magazine = NewObject<UStaticMeshComponent>(Character);
    if (!ensureMsgf(Magazine, TEXT("新弹匣角色表现创建组件失败 %s"), *Character->GetName()))
    {
        return;
    }

    Magazine->ComponentTags.Add(NewMagazineTag);
    Magazine->SetStaticMesh(MagazineMesh);
    Magazine->SetMobility(EComponentMobility::Movable);
    Magazine->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    Magazine->RegisterComponent();
    Magazine->AttachToComponent(MeshComp, FAttachmentTransformRules::SnapToTargetNotIncludingScale, HandBoneName);
    Magazine->SetRelativeTransform(HandMagazineTransform);

    UE_LOG(LogTemp, Log, TEXT("[BBBRifleMagazine] New magazine spawned on character hand Character=%s"), *Character->GetName());
}

void UBBBCharacterNewMagazineDisplayAnimNotifyState::NotifyEnd(
    USkeletalMeshComponent *MeshComp,
    UAnimSequenceBase *,
    const FAnimNotifyEventReference &)
{
    if (!MeshComp)
    {
        return;
    }

    UStaticMeshComponent *Magazine = FindNewMagazine(*MeshComp);
    if (!Magazine)
    {
        return;
    }

    Magazine->DestroyComponent();
    UE_LOG(LogTemp, Log, TEXT("[BBBRifleMagazine] New magazine display ended Mesh=%s"), *MeshComp->GetName());
}
