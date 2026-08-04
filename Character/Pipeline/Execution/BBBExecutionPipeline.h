#pragma once

#include "BBBWork/UBBBNexus/Character/Pipeline/Execution/Executors/BBBEquipmentSelectionExecutor.h"
#include "BBBWork/UBBBNexus/Character/Pipeline/Execution/Executors/BBBEquipmentActionExecutor.h"

class FBBBCharacterInitializer;
struct FBBBCharacterEquipmentInventoryState;
struct FBBBCharacterEquipmentState;
struct FBBBCharacterEquipmentCommands;
struct FBBBDecisionRuntimeData;

/** 执行所有通过仲裁的角色请求 */
class FBBBExecutionPipeline final
{
public:

    /** 执行当前帧批准请求 */
    void Update() const;

private:

    friend class FBBBCharacterInitializer;

    /**
     * 注入执行管线固定依赖
     * @param InDecisionData	角色仲裁数据
     * @param InEquipmentCommands	物品动作命令
     * @param InEquipmentState	角色装备状态
     * @param InInventoryState	角色唯一背包
     */
    void Initialize(
        FBBBDecisionRuntimeData &InDecisionData,
        FBBBCharacterEquipmentCommands &InEquipmentCommands,
        FBBBCharacterEquipmentState &InEquipmentState,
        const FBBBCharacterEquipmentInventoryState &InInventoryState);

    /** 角色仲裁数据 */
    FBBBDecisionRuntimeData *DecisionData = nullptr;

    /** 物品动作命令 */
    FBBBCharacterEquipmentCommands *EquipmentCommands = nullptr;

    /** 角色唯一背包 */
    const FBBBCharacterEquipmentInventoryState *InventoryState = nullptr;

    /** 角色装备状态 */
    FBBBCharacterEquipmentState *EquipmentState = nullptr;

    /** 装备请求执行器 */
    FBBBEquipmentSelectionExecutor SelectionExecutor;

    /** 物品动作请求执行器 */
    FBBBEquipmentActionExecutor ActionExecutor;
};
