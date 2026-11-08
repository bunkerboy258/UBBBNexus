#pragma once
#include "CoreMinimal.h"

/** 公共输入契约 具体包以自身类型继承并完整实现接口 */
template<typename TDerived = void>
struct FBBBEquipmentPrimaryLocalControlPacket
{
    /** 本次主行为中独立按下的次数 持续按住时为零 */
    int32 PressCount = 1;

    /** @return 由具体包验证格式 */
    bool IsValid() const;
    /** @param Context 具体实例依赖 @return 由具体包判断许可 */
    template<typename TContext>
    bool CanApply(const TContext &Context) const;
    /** @param Context 具体实例输入状态 @return 无 */
    template<typename TContext>
    void Apply(TContext &Context) const;
};
