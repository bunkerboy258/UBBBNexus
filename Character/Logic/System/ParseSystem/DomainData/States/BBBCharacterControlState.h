#pragma once
#include "CoreMinimal.h"
#include "BBBCharacterControlState.generated.h"

/** 已裁决的角色控制状态 */
USTRUCT()
struct FBBBCharacterControlState final
{
    GENERATED_BODY()

    /** 世界空间移动输入 */
    FVector MoveWorld = FVector::ZeroVector;

    /** 世界空间角色朝向 */
    FRotator FacingWorld = FRotator::ZeroRotator;

    /** 世界空间瞄准目标 */
    FVector AimTargetWorld = FVector::ZeroVector;

    /** 是否保持瞄准 */
    bool bAim = false;

    /** 是否请求步行 */
    bool bWalk = false;

    /** 是否请求冲刺 */
    bool bSprint = false;

    /** 是否请求蹲伏 */
    bool bCrouch = false;

    /** 是否请求跳跃 */
    bool bJump = false;
};
