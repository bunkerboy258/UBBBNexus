#pragma once

#include "BBBWork/UBBBNexus/Character/Pipeline/Execution/Executors/BBBEquipmentExecutor.h"
#include "BBBWork/UBBBNexus/Character/Pipeline/Execution/Executors/BBBItemActionExecutor.h"

class FBBBCharacterInitializer;
struct FBBBCharacterBackpackState;
struct FBBBCharacterEquipmentState;
struct FBBBCharacterItemCommands;
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
     * @param InItemCommands	物品动作命令
     * @param InEquipmentState	角色装备状态
     * @param InBackpackState	角色唯一背包
     */
    void Initialize(
        FBBBDecisionRuntimeData &InDecisionData,
        FBBBCharacterItemCommands &InItemCommands,
        FBBBCharacterEquipmentState &InEquipmentState,
        const FBBBCharacterBackpackState &InBackpackState);

    /** 角色仲裁数据 */
    FBBBDecisionRuntimeData *DecisionData = nullptr;

    /** 物品动作命令 */
    FBBBCharacterItemCommands *ItemCommands = nullptr;

    /** 角色唯一背包 */
    const FBBBCharacterBackpackState *BackpackState = nullptr;

    /** 角色装备状态 */
    FBBBCharacterEquipmentState *EquipmentState = nullptr;

    /** 装备请求执行器 */
    FBBBEquipmentExecutor EquipmentExecutor;

    /** 物品动作请求执行器 */
    FBBBItemActionExecutor ItemExecutor;
};
