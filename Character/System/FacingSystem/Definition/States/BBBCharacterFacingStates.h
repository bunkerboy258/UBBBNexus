#pragma once

#include "CoreMinimal.h"
#include "BBBCharacterFacingStates.generated.h"

/**
 * 表示角色当前原地转身动画应使用的水平方向
 */
UENUM(BlueprintType)
enum class EBBBCharacterTurnDirection : uint8
{
    /** 当前没有原地转身 */
    None,

    /** 角色正在向左侧原地转身 */
    Left,

    /** 角色正在向右侧原地转身 */
    Right
};

/**
 * 保存角色朝向系统跨帧维护的原地转身状态
 */
USTRUCT(BlueprintType)
struct FBBBCharacterFacingState
{
    GENERATED_BODY()

    /** 当前是否正在执行原地转身 */
    UPROPERTY(BlueprintReadOnly)
    bool bIsTurningInPlace = false;

    /** 当前原地转身的水平方向 */
    UPROPERTY(BlueprintReadOnly)
    EBBBCharacterTurnDirection TurnDirection = EBBBCharacterTurnDirection::None;
};
