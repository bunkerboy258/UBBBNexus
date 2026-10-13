#pragma once
#include "CoreMinimal.h"

/** 消耗品释放使用输入 */
struct FBBBConsumableReleaseLocalControlPacket final
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
        Context.Inputs.bRelease = true;
    }
};
