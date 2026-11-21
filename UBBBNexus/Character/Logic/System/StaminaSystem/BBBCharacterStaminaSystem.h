#pragma once

#include "BBBWork/UBBBNexus/Character/Logic/System/StaminaSystem/Processors/BBBCharacterStaminaProcessor.h"

class ABBBCharacter;
class UBBBCharacterConfig;
struct FBBBCharacterRuntimeData;

/** 装配体力上下文并固定调度体力处理器 */
class FBBBCharacterStaminaSystem final
{
  private:
    friend class FBBBCharacterInitializer;
    friend class FBBBCharacterUpdatePipeline;

    /**
     * @param InCharacter	角色
     * @param InData	聚合黑板
     * @param InConfig	配置
     * @return 无
     */
    void Initialize(ABBBCharacter &InCharacter, FBBBCharacterRuntimeData &InData, const UBBBCharacterConfig &InConfig);

    /** @return 无 */
    void Update();

    /** 当前角色 */
    ABBBCharacter *Character = nullptr;

    /** 聚合黑板 */
    FBBBCharacterRuntimeData *Data = nullptr;

    /** 静态配置 */
    const UBBBCharacterConfig *Config = nullptr;

    /** 体力结算处理器 */
    FBBBCharacterStaminaProcessor Processor;
};
