#pragma once
#include "BBBWork/UBBBNexus/Character/Logic/System/LocomotionSystem/Processors/BBBCharacterRecoveryMovementProcessor.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/LocomotionSystem/Processors/BBBCharacterRagdollFollowProcessor.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/LocomotionSystem/Processors/BBBCharacterRescueMovementProcessor.h"

#include "CoreMinimal.h"
#include "Curves/CurveFloat.h"
#include "UObject/StrongObjectPtr.h"

#include "BBBWork/UBBBNexus/Character/Logic/System/LocomotionSystem/Processors/BBBCharacterLocomotionProcessor.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/LocomotionSystem/Processors/BBBCharacterLifeMovementProcessor.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/LocomotionSystem/Processors/BBBCharacterMovementFactProcessor.h"

class ACharacter;
struct FBBBCharacterRuntimeData;
class UCharacterMovementComponent;
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

    /** 仅由主管线在 CMC 后和动画事实发布前采集最终移动事实 */
    void CaptureMovementFacts();
    /** 仅由主管线在物理采集之后跟随布娃娃锚点 */
    void FollowRagdoll();

private:
    /** 起身的胶囊与移动交接 */
    FBBBCharacterRecoveryMovementProcessor RecoveryMovementProcessor;
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
        FBBBCharacterRuntimeData &InCharacterData);

    ACharacter *Character = nullptr;

    UCharacterMovementComponent *Movement = nullptr;

    FBBBCharacterLocomotionDomainState *RuntimeData = nullptr;

    const FBBBCharacterControlState *ControlData = nullptr;

    const FBBBCharacterLocomotionConfig *Config = nullptr;

    /** 在移动系统存续期间强持有软加载曲线 防止地图旅行或回放垃圾回收释放该依赖 */
    TStrongObjectPtr<UCurveFloat> StrafeSpeedMapCurve;

    FBBBCharacterRuntimeData *CharacterData = nullptr;
    FBBBCharacterLocomotionProcessor LocomotionProcessor;
    FBBBCharacterLifeMovementProcessor LifeMovementProcessor;
    FBBBCharacterRescueMovementProcessor RescueMovementProcessor;
    FBBBCharacterMovementFactProcessor MovementFactProcessor;
    /** 身体锚点跟随 */
    FBBBCharacterRagdollFollowProcessor RagdollFollowProcessor;

};
