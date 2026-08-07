
#pragma once
#include "CoreMinimal.h"
#include "BBBCharacterLocomotionStates.generated.h"

UENUM(BlueprintType)
//角色装备姿态与速度档位动画表现状态
enum class EBBBLocomotionState : uint8
{
    /** 空手待机 */
    EmptyHandIdle UMETA(DisplayName = "EmptyHandIdle"),

    /** 空手行走 */
    EmptyHandWalk UMETA(DisplayName = "EmptyHandWalk"),

    /** 空手奔跑 */
    EmptyHandRun UMETA(DisplayName = "EmptyHandRun"),

    /** 空手跳跃 */
    EmptyHandJump UMETA(DisplayName = "EmptyHandJump"),

    /** 空手落地 */
    EmptyHandLand UMETA(DisplayName = "EmptyHandLand"),

    /** 步枪待机 */
    RifleIdle UMETA(DisplayName = "RifleIdle"),

    /** 步枪行走 */
    RifleWalk UMETA(DisplayName = "RifleWalk"),

    /** 步枪奔跑 */
    RifleRun UMETA(DisplayName = "RifleRun"),

    /** 步枪瞄准横移行走 */
    RifleStrafeWalk UMETA(DisplayName = "RifleStrafeWalk"),

    /** 步枪瞄准横移奔跑 */
    RifleStrafeRun UMETA(DisplayName = "RifleStrafeRun"),

    /** 步枪原地瞄准待机 */
    RifleIdleAim UMETA(DisplayName = "RifleIdleAim"),

    /** 步枪原地瞄准向左转身 */
    RifleIdleAimTurnLeft UMETA(DisplayName = "RifleIdleAimTurnLeft"),

    /** 步枪原地瞄准向右转身 */
    RifleIdleAimTurnRight UMETA(DisplayName = "RifleIdleAimTurnRight"),

    /** 步枪行走起跳 */
    RifleWalkJump UMETA(DisplayName = "RifleWalkJump"),

    /** 步枪行走落地 */
    RifleWalkLand UMETA(DisplayName = "RifleWalkLand"),

    /** 步枪奔跑起跳 */
    RifleRunJump UMETA(DisplayName = "RifleRunJump"),

    /** 步枪奔跑落地 */
    RifleRunLand UMETA(DisplayName = "RifleRunLand")
};
