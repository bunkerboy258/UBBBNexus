#pragma once
#include "BBBWork/UBBBNexus/Character/Logic/System/AppearanceSystem/Processors/BBBCharacterAppearanceInputProcessor.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/AppearanceSystem/Processors/BBBCharacterAppearanceSelectionProcessor.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/AppearanceSystem/Processors/BBBCharacterAppearanceDisplayProcessor.h"
class ABBBCharacter;
struct FBBBCharacterRuntimeData;
struct FBBBCharacterAppearanceConfig;

/** 角色外观的固定调度根 */
class ABBB_EVAC_API FBBBCharacterAppearanceSystem final
{
private:
    friend class FBBBCharacterInitializer;
    friend class FBBBCharacterUpdatePipeline;
    friend class FBBBCharacterShutdown;
    /** @param InCharacter 所属角色 @param InData 角色黑板 @param InConfig 基础配置 @return 无 */
    void Initialize(ABBBCharacter &InCharacter, FBBBCharacterRuntimeData &InData,
        const FBBBCharacterAppearanceConfig &InConfig);
    /** @return 无 顺序消费输入 生成事实并执行显示 */
    void Update();
    /** @return 无 清理本领域持有的显示与输入 */
    void Shutdown();
    /** 所属角色 */
    ABBBCharacter *Character = nullptr;
    /** 唯一聚合黑板 */
    FBBBCharacterRuntimeData *Data = nullptr;
    /** 基础静态资源 */
    const FBBBCharacterAppearanceConfig *Config = nullptr;
    FBBBCharacterAppearanceInputProcessor InputProcessor;
    FBBBCharacterAppearanceSelectionProcessor SelectionProcessor;
    FBBBCharacterAppearanceDisplayProcessor DisplayProcessor;
};
