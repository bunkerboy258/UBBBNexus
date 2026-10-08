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
    /** 动作结束裁决时确定的交权速度 镜像只还原同一结果 */
    FVector TraversalExitVelocity = FVector::ZeroVector;
    /** 当前动作的交权速度已经由控制端确定或由镜像接收 */
    bool bTraversalExitPrepared = false;
    /** 移动处理器已经应用攀爬期间的纠正策略 */
    bool bTraversalCorrectionOverride = false;
    /** 进入攀爬前的服务器误差检查策略 */
    bool bSavedIgnoreMovementError = false;
    /** 进入攀爬前的客户端纠正策略 */
    bool bSavedIgnoreMovementCorrection = false;
    /** 进入攀爬前的客户端位置接收策略 */
    bool bSavedAcceptClientPosition = false;
    /** 胶囊与移动组件最后应用的生命阶段 */
    EBBBCharacterLifePhase AppliedLifePhase = EBBBCharacterLifePhase::Alive;
};
