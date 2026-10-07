#pragma once

#include "CoreMinimal.h"

class ABBBCharacter;
class USkeletalMeshComponent;
class ABBBEquipment;
struct FBBBCharacterRuntimeData;

/** 本次角色装备更新使用的栈上上下文 */
struct FBBBCharacterEquipmentUpdateContext final
{
    /** 装备实例生命周期所属角色 */
    ABBBCharacter &Character;

    /** 角色骨骼网格 */
    USkeletalMeshComponent &CharacterMesh;

    /** 角色唯一聚合黑板 */
    FBBBCharacterRuntimeData &RuntimeData;

    /** 右手装备挂接插槽 */
    FName RightHandWeaponSocketName = NAME_None;

    /** 是否只能执行镜像还原 */
    bool bIsMirror = true;

    /** 本次待挂接装备 不跨帧保存第二份目标 */
    ABBBEquipment *DesiredEquipment = nullptr;

    /** 本次镜像请求创建的装备类型 */
    TSubclassOf<ABBBEquipment> PendingEquipmentClass = nullptr;

    /** 本次是否存在有效选择结果 */
    bool bHasSelectionResult = false;
    /** 本次镜像目标的持有实例标识 */
    uint64 DesiredGeneration = 0;
};
