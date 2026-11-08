#pragma once
#include "CoreMinimal.h"
#include "BBBMonsterDirectorActivation.generated.h"

/** 副本阶段的启动条件 */
UENUM(BlueprintType)
enum class EBBBMonsterDirectorActivation : uint8
{
    /** 前一阶段完成后自动启动 */
    Automatic UMETA(DisplayName = "自动开始"),
    /** 有有效玩家进入阶段绑定区域 */
    PlayerEnter UMETA(DisplayName = "玩家进入区域"),
    /** 收到当前阶段的明确触发输入 */
    External UMETA(DisplayName = "外部触发")
};
