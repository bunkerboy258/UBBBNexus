#pragma once

struct FBBBCharacterAnimationUpdateContext;

/** 更新逐发后坐力姿态 不修改瞄准目标 */
class FBBBCharacterRecoilProcessor final
{
public:
    /**
     * 读取已成立的开火事实并更新角色后坐力快照
     * @param Context	角色动画上下文
     * @return 无
     */
    void Update(FBBBCharacterAnimationUpdateContext &Context) const;
};
