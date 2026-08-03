
#pragma once
struct FBBBCharacterEquipmentState;
struct FBBBCharacterItemActionResults;
struct FBBBCharacterItemCommands;

class FBBBCharacterItemActionProcessor final
{
public:

    /**
     * 消费本帧物品指令 驱动当前装备执行开火换弹及其表现 并记录动作结果
     * @param ItemCommands	角色物品指令
     * @param EquipmentState	角色装备状态
     * @param ActionResults	装备动作结果
     */
    void Update(
        FBBBCharacterItemCommands &ItemCommands,
        const FBBBCharacterEquipmentState &EquipmentState,
        FBBBCharacterItemActionResults &ActionResults) const;
};
