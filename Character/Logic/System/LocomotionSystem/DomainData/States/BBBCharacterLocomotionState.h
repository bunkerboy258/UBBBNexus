#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/LifeSystem/DomainData/Definitions/BBBCharacterLifePhase.h"
#include "BBBCharacterLocomotionState.generated.h"

/** 角色移动系统持久运行事实 */
USTRUCT()
struct FBBBCharacterLocomotionState final
{
    GENERATED_BODY()

    /** 当前本地计算或网络恢复后的跑步状态 */
    bool bRun = false;

    /** 控制者产生的加速度事实 镜像不通过移动输入重新计算 */
    FVector RestoredAcceleration = FVector::ZeroVector;

    /** 与加速度同版本的持续移动输入 不驱动镜像 CMC */
    FVector RestoredMovementInput = FVector::ZeroVector;

    /** 已还原的加速度版本用于拒绝迟到状态 */
    uint64 AccelerationRevision = 0;

    /** 移动处理器是否已经进入根运动控制模式 */
    bool bTraversalControlled = false;

    /** 接管前的水平速度用于交还控制时延续移动惯性 */
    float TraversalEntrySpeed = 0.0f;
    /** 胶囊与移动组件最后应用的生命阶段 */
    EBBBCharacterLifePhase AppliedLifePhase = EBBBCharacterLifePhase::Alive;
};
