#pragma once

struct FBBBCharacterEquipmentCommands;
struct FBBBCharacterEquipmentEvents;
struct FBBBCharacterEquipmentState;
struct FBBBEquipmentActionEvent;
struct FBBBEquipmentActionResult;

/** 驱动装备持续动作并产生本帧事件 */
class FBBBCharacterEquipmentActionProcessor final
{
public:
    /**
     * 在动作仲裁前推进装备持续状态
     * @param WorldTimeSeconds	当前世界时间
     * @param EquipmentState	角色装备选择状态
     * @return 无
     */
    void Advance(float WorldTimeSeconds, FBBBCharacterEquipmentState &EquipmentState) const;

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

private:
    void PublishAction(
        FBBBCharacterEquipmentEvents &EquipmentEvents,
        FBBBEquipmentActionEvent Event,
        const FBBBEquipmentActionResult &Result) const;
};
