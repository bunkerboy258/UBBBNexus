#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/LifeSystem/DomainData/Definitions/BBBCharacterLifePhase.h"
#include "BBBWork/UBBBNexus/Combat/FBBBHitContribution.h"
#include "BBBCharacterLifeInputState.generated.h"

class APawn;

/** 本次生命更新待消费的伤害与结果输入 */
USTRUCT()
struct FBBBCharacterLifeInputState final
{
    GENERATED_BODY()

    /** 同帧已经成立的恢复量 */
    TArray<float> Recoveries;
    /** 按提交顺序保存每个完整命中 */
    TArray<FBBBHitContribution> Hits;

    /** 本机环境采样是否有新的当前结果 */
    bool bHasEnvironmentResult = false;
    /** 本机接触产生的当前速度倍率 */
    float EnvironmentSpeedScale = 1;
    /** 镜像待还原的当前速度倍率 */
    float ResultMovementSpeedScale = 1;

    /** 是否存在待还原生命结果 */
    bool bHasResult = false;

    /** 待还原的生命阶段 */
    EBBBCharacterLifePhase ResultPhase = EBBBCharacterLifePhase::Alive;

    /** 待还原的生命数值 */
    float ResultHealth = 0.0f;

    /** 待还原结果标识 */
    uint64 ResultRevision = 0;

    /** 待还原的倒地轮次 */
    uint64 ResultDownedRevision = 0;
    /** 待还原的恢复碰撞姿态 */
    bool bResultRecoveryCrouched = false;

    /** 待还原命中编号 */
    uint64 ResultHitSerial = 0;

    /** 待还原命中骨骼 */
    FName ResultBone = NAME_None;

    /** 待还原命中位置 */
    FVector ResultPosition = FVector::ZeroVector;

    /** 待还原命中方向 */
    FVector ResultDirection = FVector::ForwardVector;
};
