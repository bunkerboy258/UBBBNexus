
#pragma once
#include "CoreMinimal.h"
struct FBBBCharacterAnimationCommands;
struct FBBBCharacterAnimationState;
struct FBBBCharacterEquipmentState;

//计算瞄准来源与左手IK数据
class FBBBCharacterEquipmentPoseProcessor final
{
public:

    /**
     * 计算装备相关的IK变换与权重 提交瞄准来源与左手IK表现数据
     * @param EquipmentState	装备状态
     * @param AnimationCommands	动画命令
     * @param AnimationState	输出的动画状态
     */
    void Update(
        const FBBBCharacterEquipmentState &EquipmentState,
        const FBBBCharacterAnimationCommands &AnimationCommands,
        FBBBCharacterAnimationState &AnimationState) const;
};
