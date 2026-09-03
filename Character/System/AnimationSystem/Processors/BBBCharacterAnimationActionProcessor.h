#pragma once

#include "CoreMinimal.h"

class UBBBAnimInstance;
struct FBBBCharacterAnimationConfig;
struct FBBBCharacterEquipmentEvents;

/** 消费角色已经仲裁完成的瞬时动画动作 */
class ABBB_EVAC_API FBBBCharacterAnimationActionProcessor final
{
public:
    /**
     * 逐条发布当前帧的装备动画动作
     * @param AnimInstance		动画实例
     * @param EquipmentEvents	装备事件
     * @param AnimationConfig	角色动画配置
     * @return 无
     */
    void Update(
        UBBBAnimInstance &AnimInstance,
        const FBBBCharacterEquipmentEvents &EquipmentEvents,
        const FBBBCharacterAnimationConfig &AnimationConfig) const;
};
