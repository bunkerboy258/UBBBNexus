#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Input/Packets/BBBApprovedPackets.h"
#include "BBBWork/UBBBNexus/Character/Runtime/Controller/AimController/Definition/BBBAimRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/Runtime/Controller/EquipmentController/Definition/BBBCharacterEquipmentRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/Runtime/Controller/LocomotionController/Definition/BBBCharacterLocomotionRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/Runtime/System/AnimationSystem/Definition/BBBAnimationRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/Runtime/System/ParseSystem/Definition/BBBCharacterParseState.h"
#include "BBBWork/UBBBNexus/PlayerCamera/Input/BBBPlayerCameraInput.h"

class UBBBEquipmentCatalog;

/**
 * 输入包执行上下文 由仲裁器在友元作用域内组装
 * CanExecute 约定只读 Execute 允许写入
 */
struct FBBBCharacterPacketContext
{
    FBBBCharacterParseState &Operation;

    FBBBCharacterEquipmentInventoryState &Inventory;

    FBBBCharacterEquipmentState &Equipment;

    FBBBCharacterEquipmentCommands &Commands;

    FBBBCharacterEquipmentEvents &Events;

    FBBBAnimationRuntimeData &Animation;

    FBBBAimRuntimeData &Aim;

    FBBBCharacterLocomotionRuntimeData &Locomotion;

    TArray<FBBBPlayerCameraInput> &Camera;

    const UBBBEquipmentCatalog &Catalog;

    const FBBBApprovedPackets &Approved;
};
