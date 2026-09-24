#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/AimSystem/DomainData/States/BBBAimState.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/LocomotionSystem/DomainData/States/BBBCharacterLocomotionState.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/ParseSystem/DomainData/States/BBBCharacterControlState.h"
#include "BBBWork/UBBBNexus/PlayerCamera/Input/BBBPlayerCameraInput.h"

class UBBBAnimInstance;

/**
 * 输入包应用上下文
 *
 * CanApply只读运行时事实，Apply负责写入角色黑板
 */
struct FBBBCharacterInputContext final
{
    /** 当前角色动画实例 */
    UBBBAnimInstance *AnimationInstance;

    /** 角色瞄准状态 */
    FBBBAimState &Aim;

    /** 角色移动状态 */
    FBBBCharacterLocomotionState &Locomotion;

    /** 输入系统最终控制状态 */
    FBBBCharacterControlState &Control;

    /** 等待相机系统消费的表现输入 */
    TOptional<FBBBPlayerCameraInput> &Camera;

};
