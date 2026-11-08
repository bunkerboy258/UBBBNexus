#pragma once
#include "CoreMinimal.h"
/** 装备网格显示输入契约 */
template<typename TDerived = void>
struct FBBBEquipmentShowLocalControlPacket
{
    /** @return 由具体包验证格式 */
    bool IsValid() const;
    /** @param State 具体装备动作状态 @return 是否允许写入 */
    template<typename TState>
    bool CanApply(const TState &State) const;
    /** @param State 具体装备动作状态 @return 无 */
    template<typename TState>
    void Apply(TState &State) const;
};
