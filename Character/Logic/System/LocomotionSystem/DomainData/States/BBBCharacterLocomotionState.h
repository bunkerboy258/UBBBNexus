#pragma once

#include "CoreMinimal.h"
#include "BBBCharacterLocomotionState.generated.h"

/** 角色移动系统持久运行事实 */
USTRUCT()
struct FBBBCharacterLocomotionState final
{
    GENERATED_BODY()

    /** 当前本地计算或网络恢复后的跑步状态 */
    bool bRun = false;

    /** 移动处理器是否已经进入根运动控制模式 */
    bool bTraversalControlled = false;
};
