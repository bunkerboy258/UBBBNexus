#pragma once

#include "CoreMinimal.h"
#include "BBBCharacterStaminaState.generated.h"

/** 体力处理器维护的当前体力结果与边沿基准 */
USTRUCT()
struct FBBBCharacterStaminaState final
{
    GENERATED_BODY()

    /** 当前体力 */
    float Current = 0.0f;

    /** 当前体力上限 */
    float Maximum = 0.0f;

    /** 已成立结果的递增标识 */
    uint64 Revision = 0;

    /** 距上次消耗的累计秒数 用于延迟回复 */
    float SecondsSinceDrain = 0.0f;

    /** 已结算过的快速近战使用序号 */
    int32 LastQuickMeleeSerial = 0;

    /** 已结算过的装备近战攻击序号 负值表示尚未建立边沿基准 */
    int32 LastEquippedAttackSequence = -1;

    /** 当前体力结果是否已经建立 */
    bool bInitialized = false;
};
