#pragma once

#include "CoreMinimal.h"

class USkeletalMeshComponent;
class ABBBCharacter;
class ABBBEquipment;
struct FBBBCharacterEquipmentUpdateContext;

/** 维护角色期望装备与当前装备的一致性 */
class FBBBCharacterEquipmentSelectionProcessor final
{
public:
    /**
     * 更新当前装备实例
     * @param CharacterMesh		角色骨骼网格
     * @param AttachmentSocketName	装备挂接插槽
     * @param EquipmentOuter		装备实例生命周期所有者
     * @param EquipmentState		角色装备状态
     * @param bIsMirror           是否只能执行事实还原
     * @return 无
     */
    /**
     * 更新当前装备实例
     * @param Context 本次装备更新上下文
     * @return 无
     */
    void Update(FBBBCharacterEquipmentUpdateContext &Context) const;

};
