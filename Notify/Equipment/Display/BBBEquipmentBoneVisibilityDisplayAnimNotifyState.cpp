#include "BBBWork/UBBBNexus/Notify/Equipment/Display/BBBEquipmentBoneVisibilityDisplayAnimNotifyState.h"
#include "BBBWork/UBBBNexus/Equipment/Base/BBBEquipment.h"
#include "Components/SkeletalMeshComponent.h"

void UBBBEquipmentBoneVisibilityDisplayAnimNotifyState::NotifyBegin(
    USkeletalMeshComponent *MeshComp, UAnimSequenceBase *, float, const FAnimNotifyEventReference &)
{
    if (!MeshComp || !Cast<ABBBEquipment>(MeshComp->GetOwner()))
    {
        return;
    }

    if (!ensureMsgf(IsInGameThread() && MeshComp->GetBoneIndex(BoneName) != INDEX_NONE,
        TEXT("装备骨骼显隐通知缺少有效骨骼 %s %s"), *GetPathName(), *BoneName.ToString()))
    {
        return;
    }

    MeshComp->HideBoneByName(BoneName, EPhysBodyOp::PBO_None);
}

void UBBBEquipmentBoneVisibilityDisplayAnimNotifyState::NotifyEnd(
    USkeletalMeshComponent *MeshComp, UAnimSequenceBase *, const FAnimNotifyEventReference &)
{
    if (MeshComp && Cast<ABBBEquipment>(MeshComp->GetOwner()))
    {
        MeshComp->UnHideBoneByName(BoneName);
    }
}
