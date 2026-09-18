#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Input/Events/BBBEquipmentActionEvent.h"
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
    EBBBCharacterEquipmentPhase Phase = EBBBCharacterEquipmentPhase::Performed;

    UPROPERTY()
    int32 LoadedAmmo = 0;
};
