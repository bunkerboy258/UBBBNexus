#include "BBBWork/UBBBNexus/Notify/Equipment/Rifle/BBBRifleOldMagazineAnimNotifyState.h"

#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/BBBRifleEquipment.h"
#include "Animation/AnimMontage.h"
#include "Animation/AnimTypes.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMeshActor.h"
#include "Engine/World.h"

namespace
{
    const FName OldMagazineTag(TEXT("BBB_RifleOldHandMagazine"));

    UStaticMeshComponent *FindOldMagazine(ABBBRifleEquipment &Rifle)
    {
        return Cast<UStaticMeshComponent>(Rifle.FindComponentByTag(
            UStaticMeshComponent::StaticClass(), OldMagazineTag));
    }
}

void UBBBRifleOldMagazineAnimNotifyState::NotifyBegin(
    USkeletalMeshComponent *MeshComp,
    UAnimSequenceBase *,
    float,
    const FAnimNotifyEventReference &)
{
    ABBBRifleEquipment *Rifle = MeshComp ? Cast<ABBBRifleEquipment>(MeshComp->GetOwner()) : nullptr;
    ABBBCharacter *Character = Rifle ? Cast<ABBBCharacter>(Rifle->GetOwner()) : nullptr;
    USkeletalMeshComponent *HandMesh = Character ? Character->GetMesh() : nullptr;
    if (!Rifle || !Rifle->IsEquipped())
    {
        return;
    }

    if (!ensureMsgf(IsInGameThread() && MagazineMesh && HandMesh
        && MeshComp->GetBoneIndex(WeaponMagazineBoneName) != INDEX_NONE
        && HandMesh->GetBoneIndex(HandBoneName) != INDEX_NONE
        && !HandMagazineTransform.ContainsNaN()
        && !FindOldMagazine(*Rifle),
        TEXT("旧弹匣通知缺少有效网格 骨骼 位置或存在重复组件 %s"), *Rifle->GetName()))
    {
        return;
    }

    // 通知实例拥有旧弹匣的表现配置 组件仅以装备为外部对象以便中断时查找和销毁
    UStaticMeshComponent *Magazine = NewObject<UStaticMeshComponent>(Rifle);
    if (!ensureMsgf(Magazine, TEXT("旧弹匣通知创建手持组件失败 %s"), *Rifle->GetName()))
    {
        return;
    }

    Magazine->ComponentTags.Add(OldMagazineTag);
    Magazine->SetStaticMesh(MagazineMesh);
    Magazine->SetMobility(EComponentMobility::Movable);
    Magazine->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    Magazine->RegisterComponent();
    Magazine->AttachToComponent(HandMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale, HandBoneName);
    Magazine->SetRelativeTransform(HandMagazineTransform);
    MeshComp->HideBoneByName(WeaponMagazineBoneName, EPhysBodyOp::PBO_None);

    UE_LOG(LogTemp, Log, TEXT("[BBBRifleMagazine] Old magazine taken Equipment=%s"), *Rifle->GetName());
}

void UBBBRifleOldMagazineAnimNotifyState::NotifyTick(
    USkeletalMeshComponent *MeshComp,
    UAnimSequenceBase *Animation,
    float,
    const FAnimNotifyEventReference &EventReference)
{
    ABBBRifleEquipment *Rifle = MeshComp ? Cast<ABBBRifleEquipment>(MeshComp->GetOwner()) : nullptr;
    UStaticMeshComponent *HandMagazine = Rifle ? FindOldMagazine(*Rifle) : nullptr;
    if (!HandMagazine)
    {
        return;
    }

    UAnimMontage *Montage = Cast<UAnimMontage>(Animation);
    UAnimInstance *AnimInstance = MeshComp->GetAnimInstance();
    const FAnimNotifyEvent *Event = EventReference.GetNotify();
    if (!ensureMsgf(Montage && AnimInstance && Event && FMath::IsFinite(ReleaseDelaySeconds),
        TEXT("旧弹匣通知缺少甩出动画时间 %s"), *Rifle->GetName()))
    {
        return;
    }

    if (AnimInstance->Montage_GetPosition(Montage) < Event->GetTriggerTime() + ReleaseDelaySeconds)
    {
        return;
    }

    ABBBCharacter *Character = Cast<ABBBCharacter>(Rifle->GetOwner());
    USkeletalMeshComponent *HandMesh = Character ? Character->GetMesh() : nullptr;
    UWorld *World = Rifle->GetWorld();
    if (!ensureMsgf(HandMesh && World, TEXT("旧弹匣通知甩出时角色或世界无效 %s"), *Rifle->GetName()))
    {
        HandMagazine->DestroyComponent();
        return;
    }

    // 掉落网格继承实例配置的手持位置 并读取动画骨骼的实际甩手速度
    const FTransform DropWorld = HandMagazine->GetComponentTransform();
    AStaticMeshActor *Dropped = World->SpawnActor<AStaticMeshActor>(DropWorld.GetLocation(), DropWorld.Rotator());
    if (ensureMsgf(Dropped, TEXT("旧弹匣通知创建掉落对象失败 %s"), *Rifle->GetName()))
    {
        UStaticMeshComponent *DroppedMesh = Dropped->GetStaticMeshComponent();
        DroppedMesh->SetMobility(EComponentMobility::Movable);
        DroppedMesh->SetStaticMesh(MagazineMesh);
        DroppedMesh->SetWorldTransform(DropWorld);
        DroppedMesh->SetCollisionProfileName(TEXT("PhysicsActor"));
        DroppedMesh->SetSimulatePhysics(true);
        DroppedMesh->SetPhysicsLinearVelocity(
            (HandMesh->GetBoneLinearVelocity(HandBoneName) + Character->GetVelocity()).GetClampedToMaxSize(1500.0f));
        Dropped->SetLifeSpan(20.0f);
    }

    HandMagazine->DestroyComponent();
    UE_LOG(LogTemp, Log, TEXT("[BBBRifleMagazine] Old magazine dropped Equipment=%s"), *Rifle->GetName());
}

void UBBBRifleOldMagazineAnimNotifyState::NotifyEnd(
    USkeletalMeshComponent *MeshComp,
    UAnimSequenceBase *,
    const FAnimNotifyEventReference &)
{
    ABBBRifleEquipment *Rifle = MeshComp ? Cast<ABBBRifleEquipment>(MeshComp->GetOwner()) : nullptr;
    if (!Rifle)
    {
        return;
    }

    // 正常结束和中断都恢复枪上弹匣 留在手中的旧弹匣只在中断路径需要清理
    if (UStaticMeshComponent *HandMagazine = FindOldMagazine(*Rifle))
    {
        HandMagazine->DestroyComponent();
    }

    MeshComp->UnHideBoneByName(WeaponMagazineBoneName);
}
