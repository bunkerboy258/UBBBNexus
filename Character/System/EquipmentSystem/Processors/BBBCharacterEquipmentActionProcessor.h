#pragma once

class FBBBCharacterExternalAPI;
struct FBBBCharacterEquipmentCommands;
struct FBBBCharacterEquipmentResults;
struct FBBBCharacterEquipmentState;

/** 驱动当前装备领域并消费装备命令 */
class FBBBCharacterEquipmentActionProcessor final
{
public:
    /**
     * 更新当前装备并消费本帧命令
     * @param EquipmentCommands	角色装备命令
     * @param EquipmentState	角色装备状态
     * @param EquipmentResults	装备动作结果
     * @param CharacterAPI	角色能力接口
     */
    void Update(
        FBBBCharacterEquipmentCommands &EquipmentCommands,
        const FBBBCharacterEquipmentState &EquipmentState,
        FBBBCharacterEquipmentResults &EquipmentResults,
        FBBBCharacterExternalAPI &CharacterAPI) const;
};
