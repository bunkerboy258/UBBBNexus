#include "BBBWork/UBBBNexus/Notify/Equipment/Display/BBBRifleMagazineVisibilityDisplayAnimNotifyState.h"

#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/BBBRifleEquipment.h"
#include "Components/SkeletalMeshComponent.h"

void UBBBRifleMagazineVisibilityDisplayAnimNotifyState::NotifyBegin(
    USkeletalMeshComponent *MeshComp,
    UAnimSequenceBase *,
    float,
    const FAnimNotifyEventReference &)
{
    ABBBRifleEquipment *Rifle = MeshComp ? Cast<ABBBRifleEquipment>(MeshComp->GetOwner()) : nullptr;
    if (!Rifle)
    {
        return;
    }

    if (!ensureMsgf(IsInGameThread() && MeshComp->GetBoneIndex(WeaponMagazineBoneName) != INDEX_NONE,
        TEXT("武器弹匣显隐通知缺少骨骼 %s"), *Rifle->GetName()))
    {
        return;
    }

    MeshComp->HideBoneByName(WeaponMagazineBoneName, EPhysBodyOp::PBO_None);
    UE_LOG(LogTemp, Log, TEXT("[BBBRifleMagazine] Weapon magazine hidden Equipment=%s"), *Rifle->GetName());
}

void UBBBRifleMagazineVisibilityDisplayAnimNotifyState::NotifyEnd(
    USkeletalMeshComponent *MeshComp,
    UAnimSequenceBase *,
    const FAnimNotifyEventReference &)
{
    if (!MeshComp)
    {
        return;
    }

    MeshComp->UnHideBoneByName(WeaponMagazineBoneName);
    UE_LOG(LogTemp, Log, TEXT("[BBBRifleMagazine] Weapon magazine restored Mesh=%s"), *MeshComp->GetName());
}
