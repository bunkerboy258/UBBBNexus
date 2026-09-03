#pragma once

#include "CoreMinimal.h"

class ACharacter;
class UCharacterMovementComponent;
class UCurveFloat;
class FBBBCharacterInitializer;
struct FBBBCharacterLocomotionConfig;
struct FBBBCharacterLocomotionRuntimeData;
struct FBBBIntentRuntimeData;

/** 按官方运动样例规则驱动角色移动组件 */
class ABBB_EVAC_API FBBBCharacterLocomotionSystem final
{
public:
    /** 逐帧更新步态、移动参数、蹲跳请求和移动输入 */
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
        FBBBCharacterLocomotionRuntimeData &InRuntimeData,
        const FBBBIntentRuntimeData &InIntentData,
        const FBBBCharacterLocomotionConfig &InConfig);

    ACharacter *Character = nullptr;

    UCharacterMovementComponent *Movement = nullptr;

    FBBBCharacterLocomotionRuntimeData *RuntimeData = nullptr;

    const FBBBIntentRuntimeData *IntentData = nullptr;

    const FBBBCharacterLocomotionConfig *Config = nullptr;

    const UCurveFloat *StrafeSpeedMapCurve = nullptr;
};
