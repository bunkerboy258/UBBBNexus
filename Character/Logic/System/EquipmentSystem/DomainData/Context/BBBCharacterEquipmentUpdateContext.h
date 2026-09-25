#pragma once

#include "CoreMinimal.h"

class ABBBCharacter;
class USkeletalMeshComponent;
class ABBBEquipment;
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

    /** 本次请求创建的装备类型 */
    TSubclassOf<ABBBEquipment> PendingEquipmentClass = nullptr;

    /** 本次是否存在有效选择结果 */
    bool bHasSelectionResult = false;

    /** 本次是否请求创建或清空装备 */
    bool bCreateRequested = false;
};
