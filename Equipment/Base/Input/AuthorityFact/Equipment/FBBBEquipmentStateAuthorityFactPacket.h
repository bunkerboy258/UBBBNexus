#pragma once

#include "CoreMinimal.h"

/** 装备传输层收到的当前状态字节流 */
struct FBBBEquipmentStateAuthorityFactPacket final
{
    /** 当前装备状态编码 */
    TArray<uint8> Data;

    /** @return 字节流是否处于传输容量范围内 */
    bool IsValid() const
    {
        return !Data.IsEmpty() && Data.Num() <= 128;
    }

    /** @return 是否允许提交权威结果 */
    bool CanApply() const
    {
        return true;
    }

    /** @return 无 */
    void Apply() const
    {
    }
};
