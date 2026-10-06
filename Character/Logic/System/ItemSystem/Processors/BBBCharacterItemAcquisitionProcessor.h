#pragma once

struct FBBBCharacterItemUpdateContext;

/** 创建与清理真实背包物品实例 */
class FBBBCharacterItemAcquisitionProcessor final
{
public:
    /**
     * 维护本处理器负责的物品领域状态
     * @param Context	本次物品更新上下文
     * @return 无
     */
    void Update(FBBBCharacterItemUpdateContext &Context) const;

    /**
     * 销毁已登记的真实物品并清空背包
     * @param Context	本次清理上下文
     * @return 无
     */
    static void Shutdown(FBBBCharacterItemUpdateContext &Context);

};
