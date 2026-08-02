
#pragma once
#include "CoreMinimal.h"
class USkeletalMeshComponent;
struct FBBBCharacterAnimationCommands;
struct FBBBAnimationRuntimeData;
struct FBBBCharacterAnimationState;
struct FBBBCharacterEquipmentState;

//计算瞄准来源与左手IK数据
class FBBBCharacterEquipmentPoseProcessor final
{
public:

    //计算装备相关的IK变换与权重
    void Update(
        USkeletalMeshComponent &CharacterMesh,
        FName AimSourceBoneName,
        const FBBBCharacterEquipmentState &EquipmentState,
        const FBBBCharacterAnimationCommands &AnimationCommands,
        const FBBBAnimationRuntimeData &AnimationData,
        FBBBCharacterAnimationState &AnimationState) const;
};
