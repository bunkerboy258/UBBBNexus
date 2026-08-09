
#include "BBBWork/UBBBNexus/Character/System/AnimationSystem/Processors/BBBCharacterEquipmentPoseProcessor.h"
#include "BBBWork/UBBBNexus/Character/System/AnimationSystem/Definition/BBBAnimationRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/System/AnimationSystem/Definition/Commands/BBBCharacterAnimationCommands.h"
#include "BBBWork/UBBBNexus/Character/System/AnimationSystem/Definition/States/BBBCharacterAnimationStates.h"
#include "BBBWork/UBBBNexus/Character/System/EquipmentSystem/Definition/States/BBBCharacterEquipmentStates.h"
#include "Components/SceneComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "BBBWork/UBBBNexus/Equipment/Base/BBBEquipmentDefinition.h"
#include "BBBWork/UBBBNexus/Equipment/Fragments/Aim/BBBAimDomin.h"
#include "BBBWork/UBBBNexus/Equipment/Presentation/BBBEquipmentPresentationActor.h"
#include "BBBWork/UBBBNexus/Equipment/Fragments/Equip/BBBEquipDomin.h"
#include "BBBWork/UBBBNexus/Equipment/Base/BBBEquipmentInstance.h"

//计算装备瞄准来源与左手IK表现数据
void FBBBCharacterEquipmentPoseProcessor::Update(
    USkeletalMeshComponent &CharacterMesh,
    FName AimSourceBoneName,
    bool bIsAiming,
    const FBBBCharacterEquipmentState &EquipmentState,
    const FBBBCharacterAnimationCommands &AnimationCommands,
    const FBBBAnimationRuntimeData &AnimationData,
    FBBBCharacterAnimationState &AnimationState) const
{
    //读取当前装备与物品实例
    UBBBEquipmentInstance *ActiveInstance = EquipmentState.GetActiveMainHandInstance();
    const UBBBEquipmentDefinition *ActiveDefinition = ActiveInstance
        ? ActiveInstance->GetDefinition()
        : nullptr;
    const FBBBEquipDomin *EquipDomin = ActiveDefinition
        ? ActiveDefinition->EquipDomin.GetPtr()
        : nullptr;
    const FBBBAimDomin *AimDomin = ActiveDefinition
        ? ActiveDefinition->AimDomin.GetPtr()
        : nullptr;
    ABBBEquipmentPresentationActor *PresentationActor = ActiveInstance
        ? ActiveInstance->GetPresentationActor()
        : nullptr;
    AnimationState.EquippedUpperBodyAnimation = nullptr;
    //缺少装备时保持重置状态
    if (!EquipDomin || !PresentationActor)
    {
        AnimationState.AimIKAlpha = 0.0f;
        AnimationState.LeftHandIKAlpha = 0.0f;
        AnimationState.AimSourceLocalTransform = FTransform::Identity;
        AnimationState.LeftHandTargetRightHandBoneSpace = FTransform::Identity;
        AnimationState.bHasValidAimSource = false;
        AnimationState.bHasValidLeftHandTarget = false;
        return;
    }
    //读取姿势配置与武器网格
    AnimationState.EquippedUpperBodyAnimation = EquipDomin->GetEquippedUpperBodyAnimation();

    if (bIsAiming && AimDomin && AimDomin->GetUpperBodyAnimation())
    {
        AnimationState.EquippedUpperBodyAnimation = AimDomin->GetUpperBodyAnimation();
    }

    UStaticMeshComponent *WeaponMesh = PresentationActor->GetEquipmentMesh();
    //缺少姿势配置或网格时保持重置状态
    if (!WeaponMesh)
    {
        AnimationState.AimIKAlpha = 0.0f;
        AnimationState.LeftHandIKAlpha = 0.0f;
        AnimationState.AimSourceLocalTransform = FTransform::Identity;
        AnimationState.LeftHandTargetRightHandBoneSpace = FTransform::Identity;
        AnimationState.bHasValidAimSource = false;
        AnimationState.bHasValidLeftHandTarget = false;
        return;
    }

    //计算枪口相对瞄准骨骼的本地变换
    const FName AimSourceSocketName = EquipDomin->GetAimSourceSocketName();
    const bool bHasValidAimSource = WeaponMesh->DoesSocketExist(AimSourceSocketName)
        && CharacterMesh.GetBoneIndex(AimSourceBoneName) != INDEX_NONE;
    if (bHasValidAimSource)
    {
        const FTransform MuzzleWorld = WeaponMesh->GetSocketTransform(AimSourceSocketName, RTS_World);
        const FTransform BoneWorld = CharacterMesh.GetBoneTransform(AimSourceBoneName, RTS_World);
        AnimationState.AimSourceLocalTransform = MuzzleWorld.GetRelativeTransform(BoneWorld);
        AnimationState.bHasValidAimSource = true;
    }
    if (!bHasValidAimSource)
    {
        AnimationState.AimSourceLocalTransform = FTransform::Identity;
        AnimationState.bHasValidAimSource = false;
    }

    //校验左手握持插槽
    const FName LeftHandGripSocketName = EquipDomin->GetLeftHandGripSocketName();
    AnimationState.bHasValidLeftHandTarget = WeaponMesh->DoesSocketExist(LeftHandGripSocketName)
        && CharacterMesh.GetBoneIndex(AimSourceBoneName) != INDEX_NONE;
    //装备动画期间屏蔽左手IK
    if (AnimationState.bHasValidLeftHandTarget)
    {
        const FTransform GripWorld = EquipDomin->GetLeftHandGripSocketLocalOffset()
            * WeaponMesh->GetSocketTransform(LeftHandGripSocketName, RTS_World);
        const FTransform RightHandWorld = CharacterMesh.GetSocketTransform(AimSourceBoneName, RTS_World);

        AnimationState.LeftHandTargetRightHandBoneSpace = GripWorld.GetRelativeTransform(RightHandWorld);
    }
    if (!AnimationState.bHasValidLeftHandTarget)
    {
        AnimationState.LeftHandTargetRightHandBoneSpace = FTransform::Identity;
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
    if (!AnimationState.bHasValidAimTarget || !AnimationState.bHasValidAimSource)
    {
        AnimationState.AimIKAlpha = 0.0f;
    }

    if (AnimationState.bHasValidLeftHandTarget
        && EquipDomin->IsLeftHandIKEnabled()
        && !AnimationCommands.IsEquipmentIKBlockedRequested())
    {
        AnimationState.LeftHandIKAlpha = AnimationCommands.GetEquipmentLeftHandIKAlpha();
    }
    if (!AnimationState.bHasValidLeftHandTarget
        || !EquipDomin->IsLeftHandIKEnabled()
        || AnimationCommands.IsEquipmentIKBlockedRequested())
    {
        AnimationState.LeftHandIKAlpha = 0.0f;
    }
}
