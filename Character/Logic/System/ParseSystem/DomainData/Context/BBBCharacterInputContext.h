#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/AimSystem/DomainData/States/BBBAimState.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/LocomotionSystem/DomainData/States/BBBCharacterLocomotionState.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/ParseSystem/DomainData/States/BBBCharacterControlState.h"
#include "BBBWork/UBBBNexus/PlayerCamera/Input/BBBPlayerCameraInput.h"

struct FBBBCharacterAnimationMontageState;
struct FBBBCharacterEquipmentSelectionState;
struct FBBBCharacterEquipmentInventoryState;
struct FBBBCharacterAimImpulseState;

/**
 * 输入包应用上下文
 *
 * CanApply只读运行时事实，Apply负责写入角色黑板
 */
struct FBBBCharacterInputContext final
{
    /** 动画系统待消费的蒙太奇请求状态 */
    FBBBCharacterAnimationMontageState &AnimationMontageState;

    /** 角色瞄准状态 */
    FBBBAimState &Aim;

    /** 角色移动状态 */
    FBBBCharacterLocomotionState &Locomotion;

    /** 输入系统最终控制状态 */
    FBBBCharacterControlState &Control;

    /** 装备选择状态 */
    FBBBCharacterEquipmentSelectionState &EquipmentSelection;

    /** 装备容器只读状态 */
    const FBBBCharacterEquipmentInventoryState &EquipmentInventory;

    /** 等待相机系统消费的表现输入 */
    TOptional<FBBBPlayerCameraInput> &Camera;

    /** 动画系统维护的额外瞄准冲击 */
    FBBBCharacterAimImpulseState &AimImpulse;

};
