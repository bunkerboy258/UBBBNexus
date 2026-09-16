#pragma once
struct FBBBCharacterEquipmentCommands;
struct FBBBCharacterEquipmentEvents;
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

    /**
     * 在本帧输入仲裁之前接收上一轮装备更新的结果
     * @param Events	装备已执行事件
     * @param State	角色换弹状态
     * @return 无
     */
    void ApplyOutcomes(const FBBBCharacterEquipmentEvents &Events, FBBBCharacterEquipmentState &State) const;
};
