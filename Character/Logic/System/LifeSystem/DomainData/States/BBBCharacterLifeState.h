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
    float Health = 500.0f;

    /** 已成立结果的递增标识 */
    uint64 Revision = 0;

    /** 当前生命结果是否已经建立 */
    bool bInitialized = false;
};
