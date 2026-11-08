#pragma once

#include "CoreMinimal.h"
#include "BBBCharacterLifePhase.generated.h"

/** 角色生命阶段 */
UENUM(BlueprintType)
enum class EBBBCharacterLifePhase : uint8
{
    /** 正常行动 */
    Alive UMETA(DisplayName = "正常"),
    /** 倒地并保留受限移动 */
    Downed UMETA(DisplayName = "倒地"),
    /** 生命耗尽 */
    Dead UMETA(DisplayName = "死亡")
};
