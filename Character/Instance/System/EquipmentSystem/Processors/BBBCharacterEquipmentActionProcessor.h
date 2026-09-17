#pragma once
struct FBBBCharacterEquipmentCommands;
struct FBBBCharacterEquipmentState;

/** 转发角色装备命令并整理装备已确认结果 */
class FBBBCharacterEquipmentActionProcessor final
{
public:
    /**
     * 转发已批准输入
     * @param Commands	角色命令
     * @param State	角色装备选择
     * @return 无
     */
    void Update(FBBBCharacterEquipmentCommands &Commands, FBBBCharacterEquipmentState &State) const;

};
