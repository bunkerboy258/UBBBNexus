#pragma once

#include "BBBWork/UBBBNexus/Character/System/FacingSystem/Processors/BBBCharacterFacingProcessor.h"

class ACharacter;
class FBBBCharacterInitializer;
class UCharacterMovementComponent;
struct FBBBCharacterFacingConfig;
struct FBBBCharacterFacingRuntimeData;
struct FBBBIntentRuntimeData;

/**
 * 维护本地角色身体朝向并发布稳定的原地转身状态
 */
class ABBB_EVAC_API FBBBCharacterFacingSystem final
{
public:
    /**
     * 按固定规则更新角色朝向
     */
    void Update();

private:
    friend class FBBBCharacterInitializer;

    /**
     * 注入角色朝向系统的永久依赖
     * @param InCharacter	所属角色
     * @param InMovement		角色移动组件
     * @param InIntentData	只读角色意图
     * @param InFacingData	角色朝向运行数据
     * @param InConfig		角色朝向配置
     */
    void Initialize(
        ACharacter &InCharacter,
        UCharacterMovementComponent &InMovement,
        const FBBBIntentRuntimeData &InIntentData,
        FBBBCharacterFacingRuntimeData &InFacingData,
        const FBBBCharacterFacingConfig &InConfig);

    /** 所属角色 */
    ACharacter *Character = nullptr;

    /** 角色移动组件 */
    UCharacterMovementComponent *Movement = nullptr;

    /** 只读角色意图 */
    const FBBBIntentRuntimeData *IntentData = nullptr;

    /** 角色朝向运行数据 */
    FBBBCharacterFacingRuntimeData *FacingData = nullptr;

    /** 角色朝向配置 */
    const FBBBCharacterFacingConfig *Config = nullptr;

    /** 角色朝向规则处理器 */
    FBBBCharacterFacingProcessor FacingProcessor;
};
