#pragma once
#include "CoreMinimal.h"

/** 持有者发布的通用使用许可 */
struct FBBBConsumablePermissionLocalControlPacket final
{
    /** 同帧许可按提交顺序累积 */
    TArray<bool> Values;

    /** @return 输入是否包含许可 */
    bool IsValid() const
    {
        return !Values.IsEmpty();
    }

    /** @param Context	解析上下文 @return 是否拥有因果权限 */
    template<typename TContext>
    bool CanApply(const TContext &Context) const
    {
        return !Context.bIsMirror;
    }

    /** @param Context	解析上下文 @return 无 */
    template<typename TContext>
    void Apply(TContext &Context) const
    {
        Context.Inputs.bHasPermission = true;
        Context.Inputs.bAllowed = Values.Last();
    }
};
