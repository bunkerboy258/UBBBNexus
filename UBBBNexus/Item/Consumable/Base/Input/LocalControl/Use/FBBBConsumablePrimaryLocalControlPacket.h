#pragma once
#include "CoreMinimal.h"
/** 公共使用请求契约 具体输入完整实现许可与应用 */
template<typename TDerived = void>
struct FBBBConsumablePrimaryLocalControlPacket
{
    /** @return 由具体包验证格式 */
    bool IsValid() const;
    /** @param Context 具体实例依赖 @return 由具体包判断许可 */
    template<typename TContext>
    bool CanApply(const TContext &Context) const;
    /** @param Context 具体实例状态 @return 无 */
    template<typename TContext>
    void Apply(TContext &Context) const;
};
