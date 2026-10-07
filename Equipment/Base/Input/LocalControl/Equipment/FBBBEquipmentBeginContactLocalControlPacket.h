#pragma once
#include "CoreMinimal.h"
/** 装备动画BeginContact事实请求 */
struct FBBBEquipmentBeginContactLocalControlPacket final
{
    /** 当前动画通知实例标识 */
    TArray<int32> Tokens;
    /** @return 通知标识是否有效 */
    bool IsValid() const
    {
        return !Tokens.IsEmpty() && !Tokens.ContainsByPredicate([](int32 Value)
        {
            return Value < 0;
        });
    }
    /** @return 是否可贡献通知事实 */
    bool CanApply() const
    {
        return IsValid();
    }
    /** @return 无 */
    void Apply() const
    {
    }
};
