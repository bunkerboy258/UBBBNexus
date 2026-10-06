#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/LocomotionSystem/Processors/BBBCharacterTraversalProbeProcessor.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/LocomotionSystem/Processors/BBBCharacterTraversalLifeProcessor.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/LocomotionSystem/Processors/BBBCharacterTraversalWarpProcessor.h"

#include "BBBWork/UBBBNexus/Character/Logic/System/LocomotionSystem/Processors/BBBCharacterLocomotionProcessor.h"

class ACharacter;
class UMotionWarpingComponent;
struct FBBBCharacterRuntimeData;
struct FBBBTraversalConfig;
class UCharacterMovementComponent;
class UCurveFloat;
class FBBBCharacterInitializer;
struct FBBBCharacterLocomotionConfig;
struct FBBBCharacterLocomotionDomainState;
struct FBBBCharacterControlState;

/** 按官方运动样例规则驱动角色移动组件 */
class ABBB_EVAC_API FBBBCharacterLocomotionSystem final
{
public:
    /** 逐帧更新步态 移动参数 蹲跳请求和移动输入 */
    void Update();

private:
    friend class FBBBCharacterInitializer;

    /**
     * 初始化移动系统依赖
     * @param InCharacter	受控角色
     * @param InMovement	角色移动组件
     * @param InRuntimeData	移动运行时数据
     * @param InIntentData	移动意图运行时数据
     * @param InConfig	移动配置
     */
    void Initialize(
        ACharacter &InCharacter,
        UCharacterMovementComponent &InMovement,
        FBBBCharacterLocomotionDomainState &InRuntimeData,
        const FBBBCharacterControlState &InIntentData,
        const FBBBCharacterLocomotionConfig &InConfig,
        const FBBBTraversalConfig &InTraversalConfig,
        const FBBBCharacterRuntimeData &InCharacterData,
        UMotionWarpingComponent &InWarping);

    ACharacter *Character = nullptr;

    UCharacterMovementComponent *Movement = nullptr;

    FBBBCharacterLocomotionDomainState *RuntimeData = nullptr;

    const FBBBCharacterControlState *ControlData = nullptr;

    const FBBBCharacterLocomotionConfig *Config = nullptr;

    const UCurveFloat *StrafeSpeedMapCurve = nullptr;

    const FBBBTraversalConfig *TraversalConfig = nullptr;
    const FBBBCharacterRuntimeData *CharacterData = nullptr;
    UMotionWarpingComponent *Warping = nullptr;
    FBBBCharacterTraversalProbeProcessor ProbeProcessor;
    FBBBCharacterTraversalLifeProcessor LifeProcessor;
    FBBBCharacterTraversalWarpProcessor WarpProcessor;
    FBBBCharacterLocomotionProcessor LocomotionProcessor;

};
