#pragma once

#include "CoreMinimal.h"

class ABBBCharacter;
class USkeletalMeshComponent;
struct FBBBCharacterEquipmentInventoryState;
struct FBBBCharacterEquipmentSelectionState;

/** 本次角色装备更新使用的栈上上下文 */
struct FBBBCharacterEquipmentUpdateContext final
{
    /** 装备实例生命周期所属角色 */
    ABBBCharacter &Character;

    /** 角色骨骼网格 */
    USkeletalMeshComponent &CharacterMesh;

    /** 右手装备挂接插槽 */
    FName RightHandWeaponSocketName = NAME_None;

    /** 角色装备容器状态 */
    FBBBCharacterEquipmentInventoryState &InventoryState;

    /** 角色装备选择状态 */
    FBBBCharacterEquipmentSelectionState &SelectionState;

    /** 是否只能执行镜像还原 */
    bool bIsMirror = true;
};
