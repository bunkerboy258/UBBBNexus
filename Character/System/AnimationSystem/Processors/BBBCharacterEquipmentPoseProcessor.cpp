
#include "BBBWork/UBBBNexus/Character/System/AnimationSystem/Processors/BBBCharacterEquipmentPoseProcessor.h"
#include "BBBWork/UBBBNexus/Character/System/AnimationSystem/Definition/Commands/BBBCharacterAnimationCommands.h"
#include "BBBWork/UBBBNexus/Character/System/AnimationSystem/Definition/States/BBBCharacterAnimationStates.h"
#include "BBBWork/UBBBNexus/Character/System/EquipmentSystem/Definition/States/BBBCharacterEquipmentStates.h"
#include "BBBWork/UBBBNexus/Equipment/Base/BBBEquipmentDefinition.h"
#include "BBBWork/UBBBNexus/Equipment/Base/BBBEquipmentInstance.h"
#include "BBBWork/UBBBNexus/Equipment/System/BBBEquipmentSystem.h"

//计算装备瞄准来源与左手IK表现数据
void FBBBCharacterEquipmentPoseProcessor::Update(
    const FBBBCharacterEquipmentState &EquipmentState,
    const FBBBCharacterAnimationCommands &AnimationCommands,
    FBBBCharacterAnimationState &AnimationState) const
{
    AnimationState.HoldingUpperBodyAnimation = nullptr;
    AnimationState.AimingUpperBodyAnimation = nullptr;
    AnimationState.AimIKAlpha = 0.0f;
    AnimationState.LeftHandIKAlpha = 0.0f;
    AnimationState.AimSourceLocalTransform = FTransform::Identity;
    AnimationState.LeftHandTargetRightHandBoneSpace = FTransform::Identity;
    AnimationState.bHasValidAimSource = false;
    AnimationState.bHasValidLeftHandTarget = false;

    UBBBEquipmentInstance *ActiveInstance = EquipmentState.GetActiveMainHandInstance();
    if (!ActiveInstance)
    {
        return;
    }

    const FBBBEquipmentUpperBodyAnimationConfig &AnimationConfig = ActiveInstance->GetUpperBodyAnimationConfig();
    AnimationState.HoldingUpperBodyAnimation = AnimationConfig.HoldingAnimation;
    AnimationState.AimingUpperBodyAnimation = AnimationConfig.AimingAnimation
        ? AnimationConfig.AimingAnimation
        : AnimationConfig.HoldingAnimation;

    UBBBEquipmentSystem *EquipmentSystem = ActiveInstance->GetEquipmentSystem();
    if (!ensureMsgf(EquipmentSystem, TEXT("[UBBBC]Equipment pose update failed because equipment system is null")))
    {
        return;
    }

    AnimationState.bHasValidAimSource = EquipmentSystem->TryGetAimSourceRightHandBoneSpace(
        AnimationState.AimSourceLocalTransform);

    AnimationState.bHasValidLeftHandTarget = EquipmentSystem->TryGetLeftHandTargetRightHandBoneSpace(
        AnimationState.LeftHandTargetRightHandBoneSpace);

    if (AnimationState.bHasValidAimTarget
        && AnimationState.bHasValidAimSource
        && !AnimationCommands.IsAimIKBlockedRequested())
    {
        AnimationState.AimIKAlpha = AnimationState.AimPresentationAlpha
            * AnimationState.AimIKDistanceAlpha;
    }

    const bool bEnableLeftHandIK = EquipmentSystem->IsLeftHandIKEnabled();
    if (AnimationState.bHasValidLeftHandTarget
        && bEnableLeftHandIK
        && !AnimationCommands.IsLeftHandIKBlockedRequested())
    {
        AnimationState.LeftHandIKAlpha = AnimationCommands.GetEquipmentLeftHandIKAlpha();
    }
    if (!AnimationState.bHasValidLeftHandTarget
        || !bEnableLeftHandIK
        || AnimationCommands.IsLeftHandIKBlockedRequested())
    {
        AnimationState.LeftHandIKAlpha = 0.0f;
    }
}
