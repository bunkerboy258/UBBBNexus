#pragma once

#include "CoreMinimal.h"
#include "BBBMonsterNavigationCondition.generated.h"

/** 当前路线执行情况 不保存逐帧移动历史 */
UENUM()
enum class EBBBMonsterNavigationCondition : uint8
{
    Following UMETA(DisplayName = "沿路前进"),
    Suspended UMETA(DisplayName = "行动暂停"),
    WaitingForPath UMETA(DisplayName = "等待路径"),
    WaitingForNeighbor UMETA(DisplayName = "同伴短时让行"),
    Blocked UMETA(DisplayName = "身体通行受阻"),
    NoProgress UMETA(DisplayName = "沿路没有进展"),
    Arrived UMETA(DisplayName = "已到达"),
    Unreachable UMETA(DisplayName = "目标暂时不可达"),
    TargetMoved UMETA(DisplayName = "目标位置变化"),
    PathExpired UMETA(DisplayName = "路线复用到期"),
    PostureChanged UMETA(DisplayName = "身体姿态变化"),
    PathInvalidated UMETA(DisplayName = "导航走廊失效"),
};
