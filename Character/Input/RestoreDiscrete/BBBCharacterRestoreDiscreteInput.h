#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Input/Events/BBBEquipmentActionEvent.h"
#include "BBBWork/UBBBNexus/Character/Runtime/Controller/AimController/Definition/States/BBBAimStates.h"
#include "BBBWork/UBBBNexus/Character/Runtime/Controller/LocomotionController/Definition/BBBCharacterLocomotionRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/Input/RestoreDiscrete/Aim/BBBCharacterRestoreAimInput.h"
#include "BBBWork/UBBBNexus/Character/Input/RestoreDiscrete/Equipment/BBBCharacterRestoreEquipmentInput.h"
#include "BBBWork/UBBBNexus/Character/Input/RestoreDiscrete/Fire/BBBCharacterRestoreFireInput.h"
#include "BBBWork/UBBBNexus/Character/Input/RestoreDiscrete/Locomotion/BBBCharacterRestoreLocomotionInput.h"
#include "BBBWork/UBBBNexus/Character/Input/RestoreDiscrete/Reload/BBBCharacterRestoreReloadInput.h"
#include "BBBCharacterRestoreDiscreteInput.generated.h"

/** 网络带来的已确认离散事实 */
USTRUCT()
struct FBBBCharacterRestoreDiscreteInput
{
    GENERATED_BODY()

    TOptional<FBBBCharacterRestoreEquipmentInput> Equipment;

    TOptional<FBBBCharacterRestoreFireInput> Fire;

    TOptional<FBBBCharacterRestoreReloadInput> Reload;

    TOptional<FBBBCharacterRestoreAimInput> Aim;

    TOptional<FBBBCharacterRestoreLocomotionInput> Locomotion;
};
