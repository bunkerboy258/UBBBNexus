#pragma once

#include "CoreMinimal.h"

class FBBBCharacterExternalAPI;
class USkeletalMeshComponent;
struct FBBBCharacterEquipmentState;

/** 维护角色期望装备与当前装备的一致性 */
class FBBBCharacterEquipmentProcessor final
{
public:

    /**
     * 更新当前装备实例
     * @param CharacterMesh\t角色骨骼网格
     * @param WorldTimeSeconds\t当前世界时间
     * @param AttachmentSocketName\t装备挂接插槽
     * @param EquipmentState\t角色装备状态
     * @param CharacterAPI\t角色能力接口
     */
    void Update(
        USkeletalMeshComponent &CharacterMesh,
        FName AttachmentSocketName,
        FBBBCharacterEquipmentState &EquipmentState,
        FBBBCharacterExternalAPI &CharacterAPI) const;
};
