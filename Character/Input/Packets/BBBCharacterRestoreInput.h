#pragma once
#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Input/Packets/BBBCharacterEquipmentInput.h"
#include "BBBWork/UBBBNexus/Character/Instance/System/AimSystem/Definition/States/BBBAimStates.h"
#include "BBBWork/UBBBNexus/Character/Instance/System/LocomotionSystem/Definition/BBBCharacterLocomotionRuntimeData.h"
#include "BBBCharacterRestoreInput.generated.h"
class UBBBEquipmentDefinition;

/** 已完成因果计算的还原事实 */
USTRUCT()
struct FBBBCharacterRestoreInput
{
    GENERATED_BODY()

    UPROPERTY()
    TObjectPtr<UBBBEquipmentDefinition> Equipment = nullptr;

    UPROPERTY()
    TArray<FBBBEquipmentActionEvent> Actions;

    bool bEquipmentChanged = false;
    TOptional<FBBBAimRuntimeState> Aim;
    TOptional<EBBBCharacterGait> Gait;
};
