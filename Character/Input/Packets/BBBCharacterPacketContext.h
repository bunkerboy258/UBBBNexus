#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Runtime/Controller/AimController/Definition/BBBAimRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/Runtime/Controller/EquipmentController/Definition/BBBCharacterEquipmentRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/Runtime/Controller/LocomotionController/Definition/BBBCharacterLocomotionRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/Runtime/System/AnimationSystem/Definition/BBBAnimationRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/Runtime/System/ParseSystem/Definition/BBBCharacterParseState.h"
#include "BBBWork/UBBBNexus/PlayerCamera/Input/BBBPlayerCameraInput.h"

class UBBBEquipmentCatalog;

/**
 * 输入包应用上下文
 *
 * CanApply只读运行时事实，Apply负责写入角色黑板；网络身份由上下文明确提供，
 * 数据包不需要反向查询角色或网络组件
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

    TOptional<FBBBPlayerCameraInput> &Camera;

    const UBBBEquipmentCatalog &Catalog;

    /** 当前角色实例是否拥有玩法权威 */
    bool bAuthority = false;

    /** 当前角色实例是否由本机玩家控制 */
    bool bLocallyControlled = false;
};
