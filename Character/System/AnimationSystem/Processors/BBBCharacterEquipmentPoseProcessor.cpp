
#include "BBBWork/UBBBNexus/Character/System/AnimationSystem/Processors/BBBCharacterEquipmentPoseProcessor.h"
#include "BBBWork/UBBBNexus/Character/System/AnimationSystem/Definition/BBBAnimationRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/System/AnimationSystem/Definition/Commands/BBBCharacterAnimationCommands.h"
#include "BBBWork/UBBBNexus/Character/System/AnimationSystem/Definition/States/BBBCharacterAnimationStates.h"
#include "BBBWork/UBBBNexus/Character/System/ItemSystem/Definition/States/BBBCharacterItemStates.h"
#include "Components/SceneComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "BBBWork/UBBBNexus/Item/Base/BBBItemDefinition.h"
#include "BBBWork/UBBBNexus/Item/Template/Equipment/BBBEquipmentActor.h"
#include "BBBWork/UBBBNexus/Item/Template/Equipment/BBBEquipmentDefinition.h"
#include "BBBWork/UBBBNexus/Item/Template/Equipment/BBBEquipmentInstance.h"
#include "BBBWork/UBBBNexus/Item/Template/Equipment/EquipmentPose/BBBEquipmentPoseFragment.h"

//计算装备瞄准来源与左手IK表现数据
void FBBBCharacterEquipmentPoseProcessor::Update(
    USkeletalMeshComponent &CharacterMesh,
    FName AimSourceBoneName,
    const FBBBCharacterEquipmentState &EquipmentState,
    const FBBBCharacterAnimationCommands &AnimationCommands,
    const FBBBAnimationRuntimeData &AnimationData,
    FBBBCharacterAnimationState &AnimationState) const
{
    //每帧先重置输出状态
    AnimationState.AimIKAlpha = 0.0f;
    AnimationState.LeftHandIKAlpha = 0.0f;
    AnimationState.AimSourceLocalTransform = FTransform::Identity;
    AnimationState.LeftHandTargetRightHandSocketSpace = FTransform::Identity;
    AnimationState.bHasValidAimSource = false;
    AnimationState.bHasValidLeftHandTarget = false;
    //读取当前装备与物品实例
    UBBBEquipmentInstance *ActiveInstance = EquipmentState.GetActiveMainHandInstance();
    const UBBBEquipmentDefinition *ActiveDefinition = ActiveInstance
        ? ActiveInstance->GetEquipmentDefinition()
        : nullptr;
    ABBBEquipmentActor *EquippedItemActor = ActiveInstance
        ? ActiveInstance->GetModelActor()
        : nullptr;
    //缺少装备时保持重置状态
    if (!ActiveDefinition || !EquippedItemActor)
    { return; }
    //读取姿势配置与武器网格
    const UBBBEquipmentPoseFragment *Pose = ActiveDefinition->EquipmentPoseFragment;
    UStaticMeshComponent *WeaponMesh = EquippedItemActor->FindComponentByClass<UStaticMeshComponent>();
    //缺少姿势配置或网格时保持重置状态
    if (!Pose || !WeaponMesh)
    { return; }

    //计算枪口相对瞄准骨骼的本地变换
    if (WeaponMesh->DoesSocketExist(Pose->AimSourceSocketName) && CharacterMesh.GetBoneIndex(AimSourceBoneName) != INDEX_NONE)
    {
        const FTransform MuzzleWorld = WeaponMesh->GetSocketTransform(Pose->AimSourceSocketName, RTS_World);
        const FTransform BoneWorld = CharacterMesh.GetBoneTransform(AimSourceBoneName, RTS_World);
        AnimationState.AimSourceLocalTransform = MuzzleWorld.GetRelativeTransform(BoneWorld);
        AnimationState.bHasValidAimSource = true;
    }

    //校验左手握持插槽
    AnimationState.bHasValidLeftHandTarget = WeaponMesh->DoesSocketExist(Pose->LeftHandGripSocketName);
    //装备动画期间屏蔽左手IK
    if (AnimationState.bHasValidLeftHandTarget && EquippedItemActor->GetRootComponent())
    {
        //计算左手目标在右手插槽空间的变换
        const FTransform GripActor = WeaponMesh->GetSocketTransform(Pose->LeftHandGripSocketName, RTS_Actor);
        AnimationState.LeftHandTargetRightHandSocketSpace = Pose->LeftHandGripSocketLocalOffset
            * GripActor
            * EquippedItemActor->GetRootComponent()->GetRelativeTransform();
    }

    //目标与来源均有效时合成瞄准IK权重
    if (AnimationState.bHasValidAimTarget && AnimationState.bHasValidAimSource)
    {
        AnimationState.AimIKAlpha = FMath::Clamp(
            AnimationData.AimPresentation.SmoothedAimIKAlpha,
            0.0f,
            1.0f)
            * AnimationState.AimIKDistanceAlpha;
    }

    if (AnimationState.bHasValidLeftHandTarget
        && Pose->bEnableLeftHandIK
        && !AnimationCommands.IsItemIKBlockedRequested())
    {
        AnimationState.LeftHandIKAlpha = 1.0f;
    }
}
