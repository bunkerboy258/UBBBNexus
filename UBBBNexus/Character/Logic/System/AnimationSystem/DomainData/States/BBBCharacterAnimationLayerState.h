#pragma once

#include "CoreMinimal.h"
#include "BBBCharacterAnimationLayerState.generated.h"

class UAnimInstance;

/** 角色当前链接动画层的独立状态 */
USTRUCT()
struct FBBBCharacterAnimationLayerState final
{
    GENERATED_BODY()

    /** 当前已经链接到角色主动画蓝图的动画层类 */
    UPROPERTY(Transient)
    TSubclassOf<UAnimInstance> LinkedAnimationLayerClass;
};
