#pragma once

#include "CoreMinimal.h"
#include "MassEntityTypes.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Behavior/BBBMonsterBehavior.h"

#include "BBBMonsterPresentationStateFragment.generated.h"

/** 小怪表现层只读状态 */
USTRUCT(BlueprintType)
struct ABBB_EVAC_API FBBBMonsterPresentationStateFragment final : public FMassFragment
{
    GENERATED_BODY()

    /** 提供给表现层的状态 */
    EBBBMonsterBehavior State = EBBBMonsterBehavior::Idle;

    /** 提供给表现层的移动速度 */
    float Speed = 0.0f;

    /** 提供给表现层的状态进入时间 */
    float StateEnteredTime = 0.0f;

    /** 对应逻辑动作的独立编号 */
    uint32 ActionId = 0;

    /** 非循环动作的归一化动画位置 */
    float ActionProgress = 0.0f;

    /** 当前循环相位 不保留动作历史 */
    float LoopPhase = 0.0f;

    /** 已应用到当前循环的出生身份种子 */
    uint32 LoopSeed = 0;
};
