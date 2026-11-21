#pragma once

struct FBBBCharacterStaminaUpdateContext;

/** 只负责体力初始化 持续与离散消耗以及回复 */
class FBBBCharacterStaminaProcessor final
{
  public:
    /**
     * @param Context	本次体力上下文
     * @return 无
     */
    void Update(FBBBCharacterStaminaUpdateContext &Context) const;
};
