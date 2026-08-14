#pragma once

#include "CoreMinimal.h"
#include "BBBCharacterFacingStates.generated.h"

/**
 * 保存角色朝向系统执行双区间滞回所需的内部控制状态
 */
USTRUCT()
struct FBBBCharacterFacingState
{
    GENERATED_BODY()

    /** 是否已经越过启动阈值并正在追赶本地控制器朝向 */
    UPROPERTY(Transient)
    bool bIsIdleTurnActive = false;
};
