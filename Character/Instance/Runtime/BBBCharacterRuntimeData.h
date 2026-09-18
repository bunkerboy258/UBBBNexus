#pragma once
#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Instance/Pipeline/Input/Definition/BBBInputRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/Instance/System/LocomotionSystem/Definition/BBBCharacterControlState.h"
#include "BBBWork/UBBBNexus/Character/Instance/Pipeline/Arbitration/Definition/BBBCharacterOperationState.h"
#include "BBBWork/UBBBNexus/Character/Instance/Runtime/Definition/BBBCharacterWorldRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/Instance/System/AimSystem/Definition/BBBAimRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/Instance/System/AnimationSystem/Definition/BBBAnimationRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/Instance/System/EquipmentSystem/Definition/BBBCharacterEquipmentRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/Instance/System/LocomotionSystem/Definition/BBBCharacterLocomotionRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/Instance/System/NetworkSystem/Definition/BBBNetworkRuntimeData.h"
#include "BBBCharacterRuntimeData.generated.h"
class FBBBCharacterInitializer;
class FBBBCharacterShutdown;
class FBBBCharacterAnimationFactProcessor;
class ABBBCharacter;
class UBBBAnimInstance;

USTRUCT()
//角色全部运行数据的唯一根
struct FBBBCharacterRuntimeData
{
    GENERATED_BODY()

    /**
     * 在帧末清除禁止跨帧保留的数据
     */
    void Clean();

private:
    friend class ABBBCharacter;
    friend class ABBBPlayerCameraSystem;
    friend class FBBBInputPipeline;
    friend class FBBBArbitrationPipeline;
    friend class FBBBBehaviorRuleDispatcher;
    friend class FBBBCharacterAnimationFactProcessor;
    friend class FBBBCharacterInitializer;
    friend class FBBBCharacterShutdown;
    friend class UBBBAnimInstance;
    friend struct FBBBMoveBehavior;
    friend struct FBBBViewBehavior;
    friend struct FBBBAimBehavior;
    friend struct FBBBWalkBehavior;
    friend struct FBBBCrouchBehavior;
    friend struct FBBBJumpBehavior;
    friend struct FBBBSprintBehavior;
    friend struct FBBBEquipBehavior;
    friend struct FBBBFireBehavior;
    friend struct FBBBReloadBehavior;
    friend struct FBBBEquipmentActionEvent;
    friend struct FBBBCharacterMontagePacket;
    friend struct FBBBCharacterRestoreInput;
    friend struct FBBBPlayerCameraInput;

    //保存当前帧世界时间快照
    UPROPERTY(Transient)
    FBBBCharacterWorldRuntimeData WorldData;

    //保存原始输入与处理后输入
    UPROPERTY(Transient)
    FBBBInputRuntimeData Input;

    //保存角色行为意图
    UPROPERTY(Transient)
    FBBBCharacterControlState Control;

    //保存动作请求与仲裁结果
    UPROPERTY(Transient)
    FBBBCharacterOperationState Operation;

    //保存角色瞄准状态
    UPROPERTY(Transient)
    FBBBAimRuntimeData Aim;

    //保存动画命令与处理状态
    UPROPERTY(Transient)
    FBBBAnimationRuntimeData Animation;

    //保存相机命令与状态
    UPROPERTY(Transient)
    TArray<FBBBPlayerCameraInput> CameraContributions;

    //保存角色物品状态与命令
    UPROPERTY(Transient)
    FBBBCharacterEquipmentRuntimeData Equipment;

    //保存角色本地计算或网络恢复后的移动步态
    UPROPERTY(Transient)
    FBBBCharacterLocomotionRuntimeData Locomotion;

    //保存角色网络队列与观测状态
    UPROPERTY(Transient)
    FBBBNetworkRuntimeData Network;
};
