#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/ParseSystem/DomainData/States/BBBCharacterInputState.h"

struct FBBBCharacterInputContext;
struct FBBBCharacterInputState;

/**
 * 按源码中明确声明的顺序原地解析固定输入槽位
 *
 * 处理器不理解具体业务规则，数据合法性、通过条件和状态效果全部由对应包负责
 */
class FBBBCharacterInputProcessor final
{
public:
    /**
     * 解析本次更新可见的固定输入槽位并发布角色控制
     * @param InputState 角色固定输入状态
     * @param Context 本次解析上下文
     * @return 无
     */
    void Update(FBBBCharacterInputState &InputState, FBBBCharacterInputContext &Context) const;

private:
    /**
     * 处理一个固定槽位并在处理后失活
     * @param Slot      待处理槽位
     * @param Context   角色输入上下文
     */
    template<typename TPacket>
    static void Process(
        TBBBCharacterInputSlot<TPacket> &Slot,
        FBBBCharacterInputContext &Context)
    {
        if (!Slot.bActive)
        {
            return;
        }

        if (Slot.Data.CanApply(Context))
        {
            Slot.Data.Apply(Context);
        }

        Slot.bActive = false;
    }

};
