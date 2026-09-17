
#pragma once
struct FBBBCharacterEquipmentCommands;
struct FBBBDecisionRuntimeData;

class FBBBEquipmentActionExecutor final
{
public:

    /**
     * 执行批准的开火与换弹请求并写入物品命令
     * @param DecisionData	角色仲裁结果
     * @param EquipmentCommands	物品动作命令
     */
    void Update(
        FBBBDecisionRuntimeData &DecisionData,
        FBBBCharacterEquipmentCommands &EquipmentCommands) const;
};
