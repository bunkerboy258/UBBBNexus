#pragma once

struct FBBBCharacterAnimationUpdateContext;

/** 由角色与装备事实生成通用持枪动画参数 */
class FBBBCharacterWeaponHandlingProcessor final
{
public:
    /**
     * 更新持枪动画领域结果 不访问姿势图或反写玩法状态
     * @param Context	本帧动画上下文
     * @return 无
     */
    void Update(FBBBCharacterAnimationUpdateContext &Context) const;
};
