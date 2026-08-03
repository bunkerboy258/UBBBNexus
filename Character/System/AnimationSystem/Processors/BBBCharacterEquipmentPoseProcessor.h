
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

    /**
     * 计算装备相关的IK变换与权重 提交瞄准来源与左手IK表现数据
     * @param CharacterMesh	角色骨骼网格组件
     * @param AimSourceBoneName	瞄准来源骨骼名
     * @param EquipmentState	装备状态
     * @param AnimationCommands	动画命令
     * @param AnimationData	动画运行时数据
     * @param AnimationState	输出的动画状态
     */
    void Update(
        USkeletalMeshComponent &CharacterMesh,
        FName AimSourceBoneName,
        const FBBBCharacterEquipmentState &EquipmentState,
        const FBBBCharacterAnimationCommands &AnimationCommands,
        const FBBBAnimationRuntimeData &AnimationData,
        FBBBCharacterAnimationState &AnimationState) const;
};
