#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/LifeSystem/DomainData/Definitions/BBBCharacterLifePhase.h"
#include "BBBCharacterLifeState.generated.h"

/** 生命处理器维护的当前生命结果 */
USTRUCT()
struct FBBBCharacterLifeState final
{
    GENERATED_BODY()

    /** 当前生命阶段 */
    EBBBCharacterLifePhase Phase = EBBBCharacterLifePhase::Alive;

    /** 当前阶段剩余生命 */
    float Health = 150.0f;

    /** 当前剩余护甲 优先承受伤害并脱战自然回复 */
    float Armor = 250.0f;

    /** 距上次受到伤害已经经过的秒数 仅本机控制端用于护甲自然回复 */
    float SecondsSinceDamage = 0.0f;
    /** 控制端成立并分发的环境移动倍率 */
    float MovementSpeedScale = 1;

    /** 已成立结果的递增标识 */
    uint64 Revision = 0;

    /** 当前倒地轮次 在该轮全部伤害期间保持不变 */
    uint64 DownedRevision = 0;

    /** 最近救援恢复时的碰撞姿态 只用于阶段交接 */
    bool bRecoveryCrouched = false;

    /** 生命领域批准的正常角色操作许可 */
    bool bActionsAllowed = true;

    /** 当前生命结果是否已经建立 */
    bool bInitialized = false;
};
