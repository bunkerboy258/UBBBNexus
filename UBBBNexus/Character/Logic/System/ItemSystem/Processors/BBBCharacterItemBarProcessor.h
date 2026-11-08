#pragma once

struct FBBBCharacterItemUpdateContext;

/** 维护前序槽位的快捷选择与目标主手结果 */
class FBBBCharacterItemBarProcessor final
{
public:
    /**
     * 维护本处理器负责的物品领域状态
     * @param Context	本次物品更新上下文
     * @return 无
     */
    void Update(FBBBCharacterItemUpdateContext &Context) const;

    /** @param Context	本次清理上下文 @return 无 */
    static void Shutdown(FBBBCharacterItemUpdateContext &Context);

};
