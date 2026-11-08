#pragma once
#include "CoreMinimal.h"
#include "BBBMonsterDirectorSpawnMode.generated.h"

/** 副本小怪的生成方式 */
UENUM(BlueprintType)
enum class EBBBMonsterDirectorSpawnMode : uint8
{
    /** 有限数量的初始布置 */
    Once UMETA(DisplayName = "布置一次"),
    /** 按间隔申请完整批次 */
    Waves UMETA(DisplayName = "分批增援"),
    /** 按间隔补充当前缺额 */
    Refill UMETA(DisplayName = "持续补充")
};
