#include "BBBWork/UBBBNexus/Notify/Equipment/Rifle/BBBRifleNewMagazineAnimNotifyState.h"

#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/BBBRifleEquipment.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/Input/LocalControl/Reload/FBBBRifleLoadMagazineLocalControlPacket.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "Animation/AnimTypes.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"

namespace
{
    const FName NewMagazineTag(TEXT("BBB_RifleNewHandMagazine"));

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

void UBBBRifleNewMagazineAnimNotifyState::NotifyBegin(
    USkeletalMeshComponent *MeshComp,
    UAnimSequenceBase *,
    float,
    const FAnimNotifyEventReference &)
{
    ABBBCharacter *Character = MeshComp ? Cast<ABBBCharacter>(MeshComp->GetOwner()) : nullptr;
    ABBBRifleEquipment *Rifle = Character ? Cast<ABBBRifleEquipment>(Character->GetActiveEquipment()) : nullptr;
    if (!Rifle || !Rifle->IsEquipped())
    {
        return;
    }

    if (!ensureMsgf(IsInGameThread() && MagazineMesh
        && MeshComp->GetBoneIndex(HandBoneName) != INDEX_NONE
        && !HandMagazineTransform.ContainsNaN()
        && !FindNewMagazine(*MeshComp),
        TEXT("新弹匣通知缺少有效网格 骨骼 位置或存在重复组件 %s"), *Rifle->GetName()))
    {
        return;
    }

    // 临时组件挂在角色动画网格上 结束通知即使装备已经切换也能独立清理
    UStaticMeshComponent *Magazine = NewObject<UStaticMeshComponent>(Rifle);
    if (!ensureMsgf(Magazine, TEXT("新弹匣通知创建手持组件失败 %s"), *Rifle->GetName()))
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

    UE_LOG(LogTemp, Log, TEXT("[BBBRifleMagazine] New magazine held Equipment=%s"), *Rifle->GetName());
}

void UBBBRifleNewMagazineAnimNotifyState::NotifyEnd(
    USkeletalMeshComponent *MeshComp,
    UAnimSequenceBase *Animation,
    const FAnimNotifyEventReference &EventReference)
{
    if (!MeshComp)
    {
        return;
    }

    UStaticMeshComponent *Magazine = FindNewMagazine(*MeshComp);
    ABBBRifleEquipment *Rifle = Magazine ? Cast<ABBBRifleEquipment>(Magazine->GetOwner()) : nullptr;
    if (!Magazine || !Rifle)
    {
        return;
    }

    // 只有动画真正经过区间终点才提交装填 中断只撤销手中的临时表现
    UAnimMontage *Montage = Cast<UAnimMontage>(Animation);
    UAnimInstance *AnimInstance = MeshComp->GetAnimInstance();
    const FAnimNotifyEvent *Event = EventReference.GetNotify();
    const bool bReachedInsertion = Montage && AnimInstance && Event
        && AnimInstance->Montage_GetPosition(Montage) + 0.002f >= Event->GetEndTriggerTime();

    Magazine->DestroyComponent();
    if (!bReachedInsertion)
    {
        UE_LOG(LogTemp, Log, TEXT("[BBBRifleMagazine] New magazine cancelled Equipment=%s"), *Rifle->GetName());
        return;
    }

    if (!Rifle->IsMirror() && Rifle->IsEquipped())
    {
        ensureMsgf(Rifle->SubmitInput(FBBBRifleLoadMagazineLocalControlPacket{}),
            TEXT("新弹匣通知提交装填输入失败 %s"), *Rifle->GetName());
    }

    UE_LOG(LogTemp, Log, TEXT("[BBBRifleMagazine] New magazine inserted Equipment=%s"), *Rifle->GetName());
}
