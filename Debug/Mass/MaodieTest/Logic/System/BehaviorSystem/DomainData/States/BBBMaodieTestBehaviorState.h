#pragma once
#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Debug/Mass/MaodieTest/Logic/System/BehaviorSystem/DomainData/Definitions/BBBMaodieTestSnapshot.h"
#include "BBBMaodieTestBehaviorState.generated.h"
class APawn;
/** 耄耋Behavior状态 */
USTRUCT()
struct FBBBMaodieTestBehaviorState final
{
    GENERATED_BODY()
    /** 当前行为事实 */
    UPROPERTY()
    FBBBMaodieTestSnapshot Snapshot;
    /** 出生位置 */
    FVector Home = FVector::ZeroVector;
    /** 下次行为时间 */
    double NextActionAt = 0.0;
    /** 当前哈气结束时间 */
    double HissEndsAt = 0.0;
    /** 目标最后可见时间 */
    double LastSeenAt = 0.0;
    /** 已成功投掷的总次数 */
    int32 ThrownCount = 0;
    /** 当前观察玩家 */
    TWeakObjectPtr<APawn> Target;
    /** 配置是否可运行 */
    bool bEnabled = false;
};
