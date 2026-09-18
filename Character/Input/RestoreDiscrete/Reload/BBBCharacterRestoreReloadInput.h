#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Input/Events/BBBEquipmentActionEvent.h"
#include "BBBCharacterRestoreReloadInput.generated.h"

/** 网络还原换弹事实 */
USTRUCT()
struct FBBBCharacterRestoreReloadInput
{
    GENERATED_BODY()

    UPROPERTY()
    FName EquipmentId = NAME_None;

    UPROPERTY()
    int32 Sequence = INDEX_NONE;

    UPROPERTY()
    EBBBCharacterEquipmentPhase Phase = EBBBCharacterEquipmentPhase::Performed;

    UPROPERTY()
    int32 LoadedAmmo = 0;
};
