#pragma once

#include "CoreMinimal.h"

class USkeletalMeshComponent;
class ABBBCharacter;
struct FBBBCharacterEquipmentCommands;
struct FBBBCharacterEquipmentEvents;
struct FBBBCharacterEquipmentState;

/** 维护角色期望装备与当前装备的一致性 */
class FBBBCharacterEquipmentSelectionProcessor final
{
public:
    /**
     * 更新当前装备实例
     * @param CharacterMesh		角色骨骼网格
     * @param AttachmentSocketName	装备挂接插槽
     * @param EquipmentOuter		装备实例生命周期所有者
     * @param EquipmentCommands	角色装备命令
     * @param EquipmentState		角色装备状态
     * @param CharacterAPI		角色能力接口
     */
    void Update(
        ABBBCharacter &Character,
        USkeletalMeshComponent &CharacterMesh,
        FName AttachmentSocketName,
        float WorldTimeSeconds,
        FBBBCharacterEquipmentCommands &EquipmentCommands,
        FBBBCharacterEquipmentState &EquipmentState,
        FBBBCharacterEquipmentEvents &EquipmentEvents) const;
};
