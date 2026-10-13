#pragma once
#include "CoreMinimal.h"

/** 消耗品结束使用输入 */
struct FBBBConsumableFinishLocalControlPacket final
{
    /** @return 输入格式是否有效 */
    bool IsValid() const
    {
        return true;
    }

    /** @param Context	解析上下文 @return 是否拥有本机因果权限 */
    template<typename TContext>
    bool CanApply(const TContext &Context) const
    {
        return !Context.bIsMirror;
    }

    /** @param Context	解析上下文 @return 无 */
    template<typename TContext>
    void Apply(TContext &Context) const
    {
        Context.Inputs.bFinish = true;
    }
};
