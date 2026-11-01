#pragma once

struct FBBBCharacterLifeUpdateContext;

/** 仅受控角色采样世界环境并通过固定伤害入口递交持续伤害 */
class FBBBCharacterEnvironmentProcessor final
{
public:
    /** @param Context	当前生命处理依赖 @return 无 */
    void Update(FBBBCharacterLifeUpdateContext &Context) const;
};
