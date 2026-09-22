#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Input/Packets/Event/Equipment/BBBEquipmentActionFact.h"
#include "BBBCharacterEquipmentEventState.generated.h"

/** 角色装备领域本帧已经成立的事件状态 */
USTRUCT(BlueprintType)
struct FBBBCharacterEquipmentEventState final
{
    GENERATED_BODY()

    /** 本帧成功动作 */
    UPROPERTY()
    TArray<FBBBEquipmentActionFact> ActionEvents;
};
