#pragma once
#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/LifeSystem/DomainData/Definitions/BBBCharacterLifePhase.h"

/** 已发送的生命与护甲结果版本 */
struct FBBBCharacterLifeNetworkObservationState final
{
    /** 生命结果的发送基准 */
    uint64 Revision = 0;
    /** 上次发送的生命阶段 */
    EBBBCharacterLifePhase LastSentPhase = EBBBCharacterLifePhase::Alive;
    /** 上次发送的生命值 */
    float LastSentHealth = -1.0f;
    /** 上次发送的护甲值 */
    float LastSentArmor = -1.0f;
    /** 上次发送的环境速度倍率 */
    float LastSentSpeedScale = 1.0f;
    /** 上次发送的命中序号 */
    uint64 LastSentHitSerial = 0;
    /** 上次发送的倒地轮次 */
    uint64 LastSentDownedRevision = 0;
    /** 上次发送的恢复蹲姿 */
    bool bLastSentRecoveryCrouched = false;
    /** 上次发送的世界时间 用于护甲连续自然回复节流 */
    double LastSentTimeSeconds = -10.0;
};
