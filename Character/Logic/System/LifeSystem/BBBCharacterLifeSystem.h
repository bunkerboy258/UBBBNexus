#pragma once

#include "BBBWork/UBBBNexus/Character/Logic/System/LifeSystem/Processors/BBBCharacterLifeProcessor.h"

#include "BBBWork/UBBBNexus/Character/Logic/System/LifeSystem/Processors/BBBCharacterRescueCandidateProcessor.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/LifeSystem/Processors/BBBCharacterRescueHelperProcessor.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/LifeSystem/Processors/BBBCharacterRescueTargetProcessor.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/LifeSystem/Processors/BBBCharacterRescueMirrorProcessor.h"

class ABBBCharacter;
class UBBBCharacterConfig;
struct FBBBCharacterRuntimeData;

/** 装配生命上下文并固定调度生命处理器 */
class FBBBCharacterLifeSystem final
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

    /** 生命结算处理器 */
    FBBBCharacterLifeProcessor Processor;
    FBBBCharacterRescueCandidateProcessor CandidateProcessor;
    FBBBCharacterRescueHelperProcessor HelperProcessor;
    FBBBCharacterRescueTargetProcessor TargetProcessor;
    FBBBCharacterRescueMirrorProcessor MirrorProcessor;

};
