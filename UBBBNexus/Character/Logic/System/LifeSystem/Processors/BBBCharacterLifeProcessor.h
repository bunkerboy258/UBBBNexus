#pragma once

struct FBBBCharacterLifeUpdateContext;

/** 只负责当前生命结算及伤害事实整理 */
class FBBBCharacterLifeProcessor final
{
  public:
    /**
     * @param Context	本次生命上下文
     * @return 无
     */
    void Initialize(FBBBCharacterLifeUpdateContext &Context) const;

    /**

     * @param Context	本次生命上下文

     * @return 无

     */
    void Update(FBBBCharacterLifeUpdateContext &Context) const;
};
