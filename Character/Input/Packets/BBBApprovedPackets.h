#pragma once

#include "CoreMinimal.h"

/** 本帧已批准输入包的身份集合 冲突由失败方查询并让步 */
struct FBBBApprovedPackets
{
    /**
     * 登记一个已批准包
     * @tparam TPacket	已批准的包类型
     */
    template<typename TPacket>
    void Add()
    {
        Mask |= TPacket::ApprovedBit;
    }

    /** @return 指定包类型本帧是否已批准 */
    template<typename TPacket>
    bool Has() const
    {
        return (Mask & TPacket::ApprovedBit) != 0;
    }

    /** @return 列出的包类型是否存在本帧已批准者 */
    template<typename TPacket>
    bool HasAny() const
    {
        return Has<TPacket>();
    }

    /** @return 列出的包类型是否存在本帧已批准者 */
    template<typename TFirst, typename TSecond, typename... TRest>
    bool HasAny() const
    {
        return Has<TFirst>() || HasAny<TSecond, TRest...>();
    }

private:
    uint64 Mask = 0;
};
