#pragma once
#include "CoreMinimal.h"
/** 当前网格隐藏结果的公共契约 */
template<typename TDerived = void>
struct FBBBEquipmentVisibilityAuthorityFactPacket
{
    /** 已成立的网格隐藏结果 */
    TArray<bool> Hidden;
    /** 对应结果版本 */
    TArray<uint64> Revisions;
    /** @return 由具体包验证格式 */
    bool IsValid() const;
    /** @param State 具体装备动作状态 @return 是否允许还原 */
    template<typename TState>
    bool CanApply(const TState &State) const;
    /** @param State 具体装备动作状态 @return 无 */
    template<typename TState>
    void Apply(TState &State) const;
};
