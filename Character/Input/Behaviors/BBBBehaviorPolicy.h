#pragma once
#include "CoreMinimal.h"

/** 固定行为的提交节奏 */
enum class EBBBBehaviorDelivery : uint8
{
    Snapshot,
    Request
};

/** 当前角色行为之间的固定互斥分组 */
namespace BBBBehaviorGroup
{
    inline constexpr uint8 Reload = 1 << 0;
    inline constexpr uint8 Equip = 1 << 1;
    inline constexpr uint8 Aim = 1 << 2;
    inline constexpr uint8 Fire = 1 << 3;
}

/** 编译期声明的行为调度规则 */
struct FBBBBehaviorPolicy
{
    EBBBBehaviorDelivery Delivery;
    int32 Priority;
    uint8 BlockedBy = 0;
    uint8 Cancels = 0;

    /** @return 当前活跃行为是否阻止本行为 */
    constexpr bool IsBlocked(const uint8 ActiveGroups) const
    {
        return (ActiveGroups & BlockedBy) != 0;
    }
};
