#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Runtime/Controller/AimController/DomainData/States/BBBAimState.h"
#include "BBBWork/UBBBNexus/Character/Runtime/Controller/EquipmentController/DomainData/States/BBBCharacterEquipmentCommandState.h"
#include "BBBWork/UBBBNexus/Character/Runtime/Controller/EquipmentController/DomainData/States/BBBCharacterEquipmentEventState.h"
#include "BBBWork/UBBBNexus/Character/Runtime/Controller/EquipmentController/DomainData/States/BBBCharacterEquipmentInventoryState.h"
#include "BBBWork/UBBBNexus/Character/Runtime/Controller/EquipmentController/DomainData/States/BBBCharacterEquipmentSelectionState.h"
#include "BBBWork/UBBBNexus/Character/Runtime/Controller/LocomotionController/DomainData/States/BBBCharacterLocomotionState.h"
#include "BBBWork/UBBBNexus/Character/Runtime/System/AnimationSystem/DomainData/States/BBBCharacterAnimationState.h"
#include "BBBWork/UBBBNexus/Character/Runtime/System/ParseSystem/DomainData/States/BBBCharacterOperationState.h"
#include "BBBWork/UBBBNexus/Character/Runtime/System/ParseSystem/DomainData/States/BBBCharacterControlState.h"
#include "BBBWork/UBBBNexus/PlayerCamera/Input/BBBPlayerCameraInput.h"

class UBBBEquipmentCatalog;

/**
 * 输入包应用上下文
 *
 * CanApply只读运行时事实，Apply负责写入角色黑板
 */
struct FBBBCharacterInputContext final
{
    /** 输入解析跨帧操作状态 */
    FBBBCharacterOperationState &Operation;

    /** 角色装备容器状态 */
    FBBBCharacterEquipmentInventoryState &Inventory;

    /** 角色装备选择状态 */
    FBBBCharacterEquipmentSelectionState &Equipment;

    /** 本帧装备命令状态 */
    FBBBCharacterEquipmentCommandState &Commands;

    /** 本帧已经成立的装备事件 */
    FBBBCharacterEquipmentEventState &Events;

    /** 角色动画状态 */
    FBBBCharacterAnimationState &Animation;

    /** 角色瞄准状态 */
    FBBBAimState &Aim;

    /** 角色移动状态 */
    FBBBCharacterLocomotionState &Locomotion;

    /** 输入系统最终控制状态 */
    FBBBCharacterControlState &Control;

    /** 等待相机系统消费的表现输入 */
    TOptional<FBBBPlayerCameraInput> &Camera;

    /** 装备标识到配置的只读目录 */
    const UBBBEquipmentCatalog &Catalog;
};
