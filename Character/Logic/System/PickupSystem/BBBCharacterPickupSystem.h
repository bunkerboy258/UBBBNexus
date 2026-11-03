#pragma once
#include "BBBWork/UBBBNexus/Character/Logic/System/PickupSystem/Processors/BBBCharacterPickupProcessor.h"
class ABBBCharacter;
struct FBBBCharacterRuntimeData;
struct FBBBCharacterPickupConfig;
/** 角色世界拾取系统根 只组装依赖与调度处理器 */
class FBBBCharacterPickupSystem final
{
public:
    /** @param InCharacter\t角色运行时根 @param InData\t唯一黑板 @param InConfig\t静态配置 @return 无 */
    void Initialize(ABBBCharacter &InCharacter, FBBBCharacterRuntimeData &InData, const FBBBCharacterPickupConfig &InConfig);
    /** @return 无 调度本次世界拾取 */
    void Update();
    /** @return 无 解除本角色的交付跟踪 */
    void Shutdown();
private:
    /** 角色依赖 */
    ABBBCharacter *Character = nullptr;
    /** 唯一运行时黑板依赖 */
    FBBBCharacterRuntimeData *Data = nullptr;
    /** 静态拾取配置依赖 */
    const FBBBCharacterPickupConfig *Config = nullptr;
    /** 初版唯一拾取处理器 */
    FBBBCharacterPickupProcessor Processor;
};
