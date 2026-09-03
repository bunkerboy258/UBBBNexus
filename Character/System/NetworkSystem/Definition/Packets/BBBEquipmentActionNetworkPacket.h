#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Pipeline/Request/Definition/BBBDecisionRuntimeData.h"
#include "BBBEquipmentActionNetworkPacket.generated.h"

/** 单次装备动作网络包 */
USTRUCT()
struct FBBBEquipmentActionNetworkPacket
{
    GENERATED_BODY()

    UPROPERTY()
    EBBBCharacterActionType ActionType = EBBBCharacterActionType::None;

    UPROPERTY()
    FName EquipmentId = NAME_None;

    UPROPERTY()
    int32 Sequence = 0;

    UPROPERTY()
    float DurationSeconds = 0.0f;
};
