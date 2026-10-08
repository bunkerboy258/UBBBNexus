#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/LifeSystem/DomainData/States/BBBCharacterRescueInputState.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/LifeSystem/DomainData/States/BBBCharacterRescueState.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/NetworkSystem/DomainData/States/BBBCharacterRescueInboxState.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/AimSystem/DomainData/States/BBBAimState.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/LocomotionSystem/DomainData/States/BBBCharacterLocomotionState.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/ParseSystem/DomainData/States/BBBCharacterControlState.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/ParseSystem/DomainData/States/BBBCharacterCameraState.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/AnimationSystem/DomainData/States/BBBCharacterAnimationFactState.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/LifeSystem/DomainData/States/BBBCharacterLifeState.h"

struct FBBBCharacterAnimationMontageState;
struct FBBBCharacterTraversalState;
struct FBBBCharacterEquipmentSelectionState;
struct FBBBCharacterEquipmentAnimationInputState;
struct FBBBCharacterItemOperationState;
struct FBBBCharacterAimImpulseState;
struct FBBBCharacterLifeInputState;
struct FBBBCharacterEquipmentUseInputState;
struct FBBBCharacterDamageInboxState;
struct FBBBCharacterAppearanceInputState;

/**
 * 输入包应用上下文
 *
 * CanApply只读运行时事实 Apply负责写入角色黑板
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
    FBBBCharacterItemOperationState &ItemOperations;

    /** 主管线已确定的执行模式 */
    bool bIsMirror = true;

    /** 等待相机系统消费的表现输入 */
    FBBBCharacterCameraState &Camera;

    /** 动画系统维护的额外瞄准冲击 */
    FBBBCharacterAimImpulseState &AimImpulse;

    /** 输入解析时可读取的角色动画事实 */
    const FBBBCharacterAnimationFactState &AnimationFacts;
    /** 解析器批准的翻越事实 */
    FBBBCharacterTraversalState &Traversal;

    /** 待转交给装备的动画通知 */
    FBBBCharacterEquipmentAnimationInputState &EquipmentAnimationInputs;

    /** 本帧依次处理的独立伤害输入 */
    FBBBCharacterLifeInputState &LifeInputs;

    /** 当前生命阶段 仅用于输入许可 */
    const FBBBCharacterLifeState &Life;
    /** 待还原的装备独立使用结果 */
    FBBBCharacterEquipmentUseInputState &EquipmentUseInputs;
    /** 待投送的独立命中消息 */
    FBBBCharacterDamageInboxState &DamageInbox;

    /** 救援待消费输入 */
    FBBBCharacterRescueInputState &RescueInputs;
    /** 救援操作许可的只读事实 */
    const FBBBCharacterRescueState &Rescue;
    /** 网络路由待投送数据 */
    FBBBCharacterRescueInboxState &RescueInbox;
    /** 待消费的外观输入 */
    FBBBCharacterAppearanceInputState &AppearanceInputs;
};
