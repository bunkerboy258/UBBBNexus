#include "BBBWork/UBBBNexus/Notify/Character/Display/BBBCharacterOldMagazineDisplayAnimNotifyState.h"

#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "Animation/AnimTypes.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMeshActor.h"
#include "Engine/World.h"

namespace
{
    const FName OldMagazineTag(TEXT("BBB_CharacterOldHandMagazine"));

    UStaticMeshComponent *FindOldMagazine(USkeletalMeshComponent &HandMesh)
    {
        TArray<USceneComponent *> Children;
        HandMesh.GetChildrenComponents(false, Children);

        for (USceneComponent *Child : Children)
        {
            UStaticMeshComponent *Magazine = Cast<UStaticMeshComponent>(Child);
            if (Magazine && Magazine->ComponentHasTag(OldMagazineTag))
            {
                return Magazine;
            }
        }

        return nullptr;
    }
}

void UBBBCharacterOldMagazineDisplayAnimNotifyState::NotifyBegin(
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
        && FMath::IsFinite(DroppedLifeSeconds) && DroppedLifeSeconds > 0.0f
        && !FindOldMagazine(*MeshComp),
        TEXT("旧弹匣角色表现缺少网格 骨骼 位置或存在重复组件 %s"), *Character->GetName()))
    {
        return;
    }

    // 旧弹匣不是从武器对象转移而来 角色动画仅生成独立的手持视觉对象
    UStaticMeshComponent *Magazine = NewObject<UStaticMeshComponent>(Character);
    if (!ensureMsgf(Magazine, TEXT("旧弹匣角色表现创建组件失败 %s"), *Character->GetName()))
    {
        return;
    }

    Magazine->ComponentTags.Add(OldMagazineTag);
    Magazine->SetStaticMesh(MagazineMesh);
    Magazine->SetMobility(EComponentMobility::Movable);
    Magazine->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    Magazine->RegisterComponent();
    Magazine->AttachToComponent(MeshComp, FAttachmentTransformRules::SnapToTargetNotIncludingScale, HandBoneName);
    Magazine->SetRelativeTransform(HandMagazineTransform);

    UE_LOG(LogTemp, Log, TEXT("[BBBRifleMagazine] Old magazine spawned on character hand Character=%s"), *Character->GetName());
}

void UBBBCharacterOldMagazineDisplayAnimNotifyState::NotifyEnd(
    USkeletalMeshComponent *MeshComp,
    UAnimSequenceBase *Animation,
    const FAnimNotifyEventReference &EventReference)
{
    if (!MeshComp)
    {
        return;
    }

    UStaticMeshComponent *Magazine = FindOldMagazine(*MeshComp);
    ABBBCharacter *Character = Cast<ABBBCharacter>(MeshComp->GetOwner());
    if (!Magazine || !Character)
    {
        return;
    }

    // 蒙太奇被打断时仍会结束通知区间 只有经过甩出帧才产生世界掉落物
    UAnimMontage *Montage = Cast<UAnimMontage>(Animation);
    UAnimInstance *AnimInstance = MeshComp->GetAnimInstance();
    const FAnimNotifyEvent *Event = EventReference.GetNotify();
    const bool bReachedRelease = Montage && AnimInstance && Event
        && AnimInstance->Montage_GetPosition(Montage) + 0.002f >= Event->GetEndTriggerTime();

    if (!bReachedRelease)
    {
        Magazine->DestroyComponent();
        UE_LOG(LogTemp, Log, TEXT("[BBBRifleMagazine] Old magazine display cancelled Character=%s"), *Character->GetName());
        return;
    }

    UWorld *World = Character->GetWorld();
    if (!ensureMsgf(World && MagazineMesh, TEXT("旧弹匣角色表现无法创建掉落物 %s"), *Character->GetName()))
    {
        Magazine->DestroyComponent();
        return;
    }

    const FTransform DropWorld = Magazine->GetComponentTransform();
    AStaticMeshActor *Dropped = World->SpawnActor<AStaticMeshActor>(DropWorld.GetLocation(), DropWorld.Rotator());
    if (ensureMsgf(Dropped, TEXT("旧弹匣角色表现生成掉落对象失败 %s"), *Character->GetName()))
    {
        UStaticMeshComponent *DroppedMesh = Dropped->GetStaticMeshComponent();
        DroppedMesh->SetMobility(EComponentMobility::Movable);
        DroppedMesh->SetStaticMesh(MagazineMesh);
        DroppedMesh->SetWorldTransform(DropWorld);
        DroppedMesh->SetCollisionProfileName(TEXT("PhysicsActor"));
        DroppedMesh->SetSimulatePhysics(true);
        DroppedMesh->SetPhysicsLinearVelocity(
            (MeshComp->GetBoneLinearVelocity(HandBoneName) + Character->GetVelocity()).GetClampedToMaxSize(1500.0f));
        Dropped->SetLifeSpan(DroppedLifeSeconds);
    }

    Magazine->DestroyComponent();
    UE_LOG(LogTemp, Log, TEXT("[BBBRifleMagazine] Old magazine dropped Character=%s"), *Character->GetName());
}
