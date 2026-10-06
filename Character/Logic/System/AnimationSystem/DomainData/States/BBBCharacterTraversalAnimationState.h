#pragma once
#include "CoreMinimal.h"
#include "BBBCharacterTraversalAnimationState.generated.h"

/** 翻越动画请求的独立去重生命周期 */
USTRUCT()
struct FBBBCharacterTraversalAnimationState final
{
    GENERATED_BODY()
    /** 上次已交给蓝图选择动画的动作序号 */
    uint32 LastActionId = 0;
};
