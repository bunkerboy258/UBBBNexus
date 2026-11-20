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

    /** 当前本地计算或网络恢复后的滑铲状态 */
    bool bSlide = false;
    /** 上一次移动更新是否收到持续蹲伏意图 用于识别按下的瞬间 */
    bool bWasCrouchRequested = false;
    /** 滑铲期间上一次有效地面速度是否低于退出阈值 */
    bool bSlideBelowExitSpeed = false;
    /** 本次滑铲在 CMC 更新后记录的最高水平速度 */
    float SlidePeakSpeed = 0.0f;

    /** 本次滑铲已经持续的时间 单位秒 */
    float SlideElapsed = 0.0f;
    /** 胶囊与移动模式已经还原的全身物理阶段版本 */
    uint64 AppliedPhysicalTransition = 0;

    /** CMC 完成后的有效加速度 下游不再判断事实来源 */
    FVector Acceleration = FVector::ZeroVector;

    /** 本帧有效移动输入 保留原始输入强度 */
    FVector MovementInput = FVector::ZeroVector;

    /** 本帧有效世界朝向输入 */
    FRotator FacingWorld = FRotator::ZeroRotator;

    /** 控制者产生的加速度事实 镜像不通过移动输入重新计算 */
    FVector RestoredAcceleration = FVector::ZeroVector;

    /** 与加速度同版本的持续移动输入 不驱动镜像 CMC */
    FVector RestoredMovementInput = FVector::ZeroVector;

    /** 已还原的加速度版本用于拒绝迟到状态 */
    uint64 AccelerationRevision = 0;

    /** 胶囊与移动组件最后应用的生命阶段 */
    EBBBCharacterLifePhase AppliedLifePhase = EBBBCharacterLifePhase::Alive;
};
