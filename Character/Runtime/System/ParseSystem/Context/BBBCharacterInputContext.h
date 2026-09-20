#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Runtime/Controller/AimController/Definition/BBBAimRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/Runtime/Controller/EquipmentController/Definition/BBBCharacterEquipmentRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/Runtime/Controller/LocomotionController/Definition/BBBCharacterLocomotionRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/Runtime/System/AnimationSystem/State/BBBAnimationState.h"
#include "BBBWork/UBBBNexus/Character/Runtime/System/ParseSystem/State/BBBCharacterParseState.h"
#include "BBBWork/UBBBNexus/PlayerCamera/Input/BBBPlayerCameraInput.h"

class UBBBEquipmentCatalog;

/**
 * 输入包应用上下文
 *
 * CanApply只读运行时事实，Apply负责写入角色黑板
 */
struct FBBBCharacterInputContext
{
    FBBBCharacterParseState &Operation;

    FBBBCharacterEquipmentInventoryState &Inventory;

    FBBBCharacterEquipmentState &Equipment;

    FBBBCharacterEquipmentCommands &Commands;

    FBBBCharacterEquipmentEvents &Events;

    FBBBAnimationState &Animation;

    FBBBAimRuntimeData &Aim;

    FBBBCharacterLocomotionRuntimeData &Locomotion;

    TOptional<FBBBPlayerCameraInput> &Camera;

    const UBBBEquipmentCatalog &Catalog;
};
