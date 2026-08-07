
#pragma once
#include "CoreMinimal.h"
#include "BBBCharacterLocomotionStates.generated.h"

UENUM(BlueprintType)
//角色移动动画表现状态
enum class EBBBLocomotionState : uint8
{
    /** 待机 */
    Idle UMETA(DisplayName = "Idle"),

    /** 行走 */
    Walk UMETA(DisplayName = "Walk"),

    /** 奔跑 */
    Run UMETA(DisplayName = "Run"),

    /** 瞄准横移行走 */
    StrafeWalk UMETA(DisplayName = "StrafeWalk"),

    /** 瞄准横移奔跑 */
    StrafeRun UMETA(DisplayName = "StrafeRun"),

    /** 原地瞄准待机 */
    IdleAim UMETA(DisplayName = "IdleAim"),

    /** 原地瞄准向左转身 */
    IdleAimTurnLeft UMETA(DisplayName = "IdleAimTurnLeft"),

    /** 原地瞄准向右转身 */
    IdleAimTurnRight UMETA(DisplayName = "IdleAimTurnRight"),

    /** 跳跃 */
    Jump UMETA(DisplayName = "Jump"),

    /** 落地 */
    Land UMETA(DisplayName = "Land")
};
