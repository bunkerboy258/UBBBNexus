#pragma once

struct FBBBCharacterEquipmentCommands;
struct FBBBCharacterEquipmentEvents;
struct FBBBCharacterEquipmentState;

/** 驱动装备持续动作并产生本帧事件 */
class FBBBCharacterEquipmentActionProcessor final
{
public:
    /**
     * 更新当前装备动作
     * @param WorldTimeSeconds   当前世界时间
     * @param EquipmentCommands 角色装备命令
     * @param EquipmentState    角色装备状态
     * @param EquipmentEvents   角色装备事件
     */
    void Update(
        float WorldTimeSeconds,
        FBBBCharacterEquipmentCommands &EquipmentCommands,
        FBBBCharacterEquipmentState &EquipmentState,
        FBBBCharacterEquipmentEvents &EquipmentEvents) const;
};
