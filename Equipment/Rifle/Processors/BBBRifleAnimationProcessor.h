#pragma once

struct FBBBRifleInputContext;
struct FBBBEquipmentAnimationFacts;

/** 步枪左手握持动画事实计算 */
struct FBBBRifleAnimationProcessor final
{
    /**
     * 将武器握持位置整理为角色右手骨骼空间目标
     * @param Context	本次步枪更新上下文
     * @param Facts	待统一发布的动画事实
     * @return 无
     */
    static void CalculateLeftHandTarget(
        const FBBBRifleInputContext &Context,
        FBBBEquipmentAnimationFacts &Facts);
};
