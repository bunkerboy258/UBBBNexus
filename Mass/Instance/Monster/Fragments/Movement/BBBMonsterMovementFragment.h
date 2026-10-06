#pragma once

#include "CoreMinimal.h"
#include "MassEntityTypes.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Movement/BBBMonsterGait.h"

#include "BBBMonsterMovementFragment.generated.h"

/** 小怪移动配置与运行状态 */
USTRUCT(BlueprintType)
struct ABBB_EVAC_API FBBBMonsterMovementFragment final : public FMassFragment
{
    GENERATED_BODY()

    /** 巡逻与近距离接近速度 */
    float WalkSpeed = 100.0f;

    /** 中距离追击速度 */
    float RunSpeed = 300.0f;

    /** 远距离追击速度 */
    float SprintSpeed = 500.0f;

    /** 正常移动的加速度 */
    float Acceleration = 600.0f;

    /** 正常移动的减速度 */
    float Deceleration = 900.0f;

    /** 剩余接近距离进入冲刺的阈值 */
    float SprintDistance = 900.0f;

    /** 当前追击档位 仅由移动处理器维护 */
    EBBBMonsterGait Gait = EBBBMonsterGait::Walk;

};
