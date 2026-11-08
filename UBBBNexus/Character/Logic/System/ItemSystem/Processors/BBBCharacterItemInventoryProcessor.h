#pragma once

struct FBBBCharacterItemUpdateContext;

/** 维护统一背包槽位与移动交换 */
class FBBBCharacterItemInventoryProcessor final
{
public:
    /**
     * 维护本处理器负责的物品领域状态
     * @param Context	本次物品更新上下文
     * @return 无
     */
    void Update(FBBBCharacterItemUpdateContext &Context) const;

};
