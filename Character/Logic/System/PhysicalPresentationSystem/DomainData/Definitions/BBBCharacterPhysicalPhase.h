#pragma once
#include "CoreMinimal.h"
#include "BBBCharacterPhysicalPhase.generated.h"

/** 全身物理与起身控制的互斥阶段 */
UENUM(BlueprintType)
enum class EBBBCharacterPhysicalPhase : uint8
{
    /** 常规动画与移动 */
    Animated,
    /** 自由布娃娃 */
    Ragdoll,
    /** 已批准的起身动作等待完成通知 */
    Recovering
};
