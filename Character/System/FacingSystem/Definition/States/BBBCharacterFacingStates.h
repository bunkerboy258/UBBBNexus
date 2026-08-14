#pragma once

#include "CoreMinimal.h"
#include "BBBCharacterFacingStates.generated.h"

/**
 * 保存角色朝向系统跨帧维护的原地转身状态
 */
USTRUCT(BlueprintType)
struct FBBBCharacterFacingState
{
    GENERATED_BODY()

    /** 当前是否正在向左侧原地转身 */
    UPROPERTY(BlueprintReadOnly)
    bool bIsTurningLeft = false;

    /** 当前是否正在向右侧原地转身 */
    UPROPERTY(BlueprintReadOnly)
    bool bIsTurningRight = false;
};
