#pragma once
#include "CoreMinimal.h"
#include "BBBPlayerExplorationState.generated.h"
class UWorld;

/** 当前本地玩家在本关卡的唯一探索进度 */
USTRUCT()
struct FBBBPlayerExplorationState final
{
    GENERATED_BODY()

    /** 所属关卡世界 重开关卡时重新计数 */
    UPROPERTY(Transient)
    TWeakObjectPtr<UWorld> World;

    /** 编号一至十的发现位集 同一编号只成立一次 */
    uint16 DiscoveredMask = 0;

    /** 下次允许进行距离与遮挡查询的实际时间 */
    double NextScanAt = 0.0;

    /** 首次全部发现的实际时间 未完成为负值 */
    double CompletedAt = -1.0;
};
