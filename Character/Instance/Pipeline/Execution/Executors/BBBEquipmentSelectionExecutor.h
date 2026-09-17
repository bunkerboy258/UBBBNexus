#pragma once

struct FBBBCharacterEquipmentInventoryState;
struct FBBBCharacterEquipmentState;
struct FBBBDecisionRuntimeData;

/** 根据批准的装备请求选择快捷访问装备 */
class FBBBEquipmentSelectionExecutor final
{
public:

    /**
     * 执行批准的装备请求
     * @param DecisionData	角色仲裁结果
     * @param InventoryState	角色唯一背包
     * @param EquipmentState	角色装备状态
     */
    void Update(
        FBBBDecisionRuntimeData &DecisionData,
        const FBBBCharacterEquipmentInventoryState &InventoryState,
        FBBBCharacterEquipmentState &EquipmentState) const;
};
