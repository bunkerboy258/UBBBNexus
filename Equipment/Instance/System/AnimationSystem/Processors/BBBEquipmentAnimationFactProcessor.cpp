#include "BBBWork/UBBBNexus/Equipment/Instance/System/AnimationSystem/Processors/BBBEquipmentAnimationFactProcessor.h"

#include "BBBWork/UBBBNexus/Equipment/BBBEquipmentAnimInstance.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/System/AnimationSystem/Definition/BBBEquipmentAnimationRuntimeData.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/System/FireSystem/Definition/BBBEquipmentFireRuntimeData.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/System/EquipSystem/Fragment/BBBEquipmentEquipFragment.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/System/FireSystem/Fragment/BBBEquipmentFireFragment.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"

void FBBBEquipmentAnimationFactProcessor::Update(
    USkeletalMeshComponent &CharacterMesh, USkeletalMeshComponent &WeaponMesh,
    FBBBEquipmentAnimationRuntimeData &Data, const FBBBEquipmentFireRuntimeData &Fire,
    const FBBBEquipmentEquipFragment &EquipFragment, const FBBBEquipmentFireFragment &FireFragment) const
{
    // 取得装备动画实例和世界时间来源
    UBBBEquipmentAnimInstance *AnimInstance = Cast<UBBBEquipmentAnimInstance>(WeaponMesh.GetAnimInstance());
    UWorld *World = WeaponMesh.GetWorld();
    if (!ensureMsgf(AnimInstance && World, TEXT("[UBBBE]Equipment animation snapshot dependencies are invalid")))
    {
        return;
    }

    // 重建装备动画事实避免残留上一帧数据
    FBBBEquipmentAnimationFacts &Facts = Data.Facts;
    Facts = FBBBEquipmentAnimationFacts();
    Facts.CurrentWorldTimeSeconds = World->GetTimeSeconds();
    Facts.LastFireTimeSeconds = Fire.LastFireTimeSeconds;
    Facts.FireSequence = Fire.FireSequence;
    Facts.LoadedAmmo = Fire.LoadedAmmo;
    Facts.AmmoCapacity = Fire.AmmoCapacity;

    // 根据右手骨骼和枪口插槽计算瞄准来源
    const FName RightHandBone = TEXT("hand_r");
    const bool bHasRightHandBone = CharacterMesh.GetBoneIndex(RightHandBone) != INDEX_NONE;
    Facts.bHasValidAimSource = bHasRightHandBone
        && WeaponMesh.DoesSocketExist(FireFragment.GetMuzzleSocketName());
    if (Facts.bHasValidAimSource)
    {
        Facts.AimSourceLocalTransform = WeaponMesh.GetSocketTransform(
            FireFragment.GetMuzzleSocketName(), RTS_World).GetRelativeTransform(
                CharacterMesh.GetBoneTransform(RightHandBone, RTS_World));
    }

    // 根据左手插槽计算双手瞄准目标
    Facts.bHasLeftHandTarget = bHasRightHandBone
        && WeaponMesh.DoesSocketExist(EquipFragment.GetLeftHandSocketName());
    if (Facts.bHasLeftHandTarget)
    {
        const FVector SocketPosition = WeaponMesh.GetSocketTransform(
            EquipFragment.GetLeftHandSocketName(), RTS_Component).GetLocation();
        const FVector TargetWorld = WeaponMesh.GetComponentTransform().TransformPosition(
            SocketPosition + EquipFragment.GetLeftHandSocketOffset());
        Facts.LeftHandTargetHandRSpace = CharacterMesh.GetBoneTransform(
            RightHandBone, RTS_World).InverseTransformPosition(TargetWorld) + EquipFragment.GetLeftHandIKOffset();
    }

    // 缺少动画所需骨骼或插槽时触发防呆报警
    ensureMsgf(Facts.bHasValidAimSource, TEXT("[UBBBE]Equipment muzzle or character hand_r bone is missing"));
    ensureMsgf(Facts.bHasLeftHandTarget, TEXT("[UBBBE]Weapon left hand socket or character hand_r bone is missing"));
    AnimInstance->PublishAnimationFacts(Facts);
}
