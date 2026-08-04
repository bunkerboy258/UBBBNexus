#pragma once

class UObject;
struct FBBBCharacterEquipmentConfig;
struct FBBBCharacterEquipmentRuntimeData;

/** 根据角色配置建立出生装备与快捷绑定 */
class FBBBCharacterDefaultEquipmentInitializer final
{
public:
    /**
     * 建立角色出生装备
     * @param EquipmentData	角色装备黑板
     * @param EquipmentOuter	装备实例生命周期所有者
     * @param EquipmentConfig	角色装备配置
     */
    void Initialize(
        FBBBCharacterEquipmentRuntimeData &EquipmentData,
        UObject &EquipmentOuter,
        const FBBBCharacterEquipmentConfig &EquipmentConfig) const;
};
