#pragma once
#include "BBBMonsterDirectorEventWaveMode.generated.h"

/** 预制事件波次的刷怪模式 */
UENUM(BlueprintType)
enum class EBBBMonsterDirectorEventWaveMode : uint8
{
    /** 单波爆发 刷满总数量即自动结束 */
    Burst UMETA(DisplayName = "单波爆发"),
    /** 持续补员 按间隔补怪直到手动停止或达到上限 */
    Sustained UMETA(DisplayName = "持续补员")
};
