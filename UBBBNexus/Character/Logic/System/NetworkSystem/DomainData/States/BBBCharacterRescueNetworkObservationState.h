#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "BBBCharacterRescueNetworkObservationState.generated.h"

/** 角色救援领域的独立状态 */
USTRUCT()
struct FBBBCharacterRescueNetworkObservationState final
{
    GENERATED_BODY()

    /** 已发送离散投送事实 */
    uint64 DeliverySerial = 0;

    /** 已发送救援状态版本 */
    uint64 Revision = 0;

};
