#pragma once

struct FBBBCharacterAnimationUpdateContext;

/** 采集当前装备枪口在角色右手骨骼空间中的变换 */
class FBBBCharacterMuzzleProcessor final
{
public:
    /**
     * 更新枪口动画事实 获取失败时清除上一帧结果
     * @param Context	角色动画更新上下文
     * @return 无
     */
    void Update(FBBBCharacterAnimationUpdateContext &Context) const;
};
