#pragma once

#include "BBBWork/UBBBNexus/Character/Logic/System/ItemSystem/Processors/BBBCharacterItemAcquisitionProcessor.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/ItemSystem/Processors/BBBCharacterItemInventoryProcessor.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/ItemSystem/Processors/BBBCharacterItemBarProcessor.h"

class ABBBCharacter;
struct FBBBCharacterRuntimeData;
struct FBBBCharacterItemConfig;

/** 统一背包与上层快捷选择的固定调度根 */
class ABBB_EVAC_API FBBBCharacterItemSystem final
{
private:
    friend class FBBBCharacterInitializer;
    friend class FBBBCharacterUpdatePipeline;
    friend class FBBBCharacterShutdown;
    friend class FBBBCharacterItemSystemTest;

    /**
     * 初始化角色物品领域
     * @param InCharacter	物品所属角色
     * @param InRuntimeData	角色聚合黑板
     * @param Config	物品配置
     * @return 无
     */
    void Initialize(ABBBCharacter &InCharacter, FBBBCharacterRuntimeData &InRuntimeData,
        const FBBBCharacterItemConfig &Config);

    /** @return 无 按固定顺序维护物品领域 */
    void Update();

    /** @return 无 装备使用关系解除后清理物品实例 */
    void Shutdown();

    /** 物品所属角色 */
    ABBBCharacter *Character = nullptr;

    /** 唯一聚合黑板 */
    FBBBCharacterRuntimeData *RuntimeData = nullptr;

    /** 静态物品配置 */
    const FBBBCharacterItemConfig *ItemConfig = nullptr;

    /** 创建与最终销毁处理器 */
    FBBBCharacterItemAcquisitionProcessor AcquisitionProcessor;

    /** 统一槽位维护处理器 */
    FBBBCharacterItemInventoryProcessor InventoryProcessor;

    /** 上层快捷选择处理器 */
    FBBBCharacterItemBarProcessor BarProcessor;
};
