
#pragma once
struct FBBBCharacterItemCommands;
struct FBBBDecisionRuntimeData;

class FBBBItemActionExecutor final
{
public:

    /**
     * 执行批准的开火与换弹请求并写入物品命令
     * @param DecisionData	角色仲裁结果
     * @param ItemCommands	物品动作命令
     */
    void Update(
        FBBBDecisionRuntimeData &DecisionData,
        FBBBCharacterItemCommands &ItemCommands) const;
};
