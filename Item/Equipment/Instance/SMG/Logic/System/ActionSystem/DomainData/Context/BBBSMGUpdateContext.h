#pragma once

#include "CoreMinimal.h"

class ABBBCharacter;
class ABBBSMGEquipment;
class UBBBSMGDefinition;
class USkeletalMeshComponent;
class UWorld;
struct FBBBSMGRuntimeData;

/** 只在装备根本次更新期间存活的调用上下文 */
struct FBBBSMGUpdateContext final
{
    /** 当前冲锋枪 */
    ABBBSMGEquipment &Equipment;

    /** 当前持有者 */
    ABBBCharacter &Character;

    /** 当前装备网格 */
    USkeletalMeshComponent &WeaponMesh;

    /** 当前静态配置 */
    const UBBBSMGDefinition &Definition;

    /** 当前唯一运行时黑板 */
    FBBBSMGRuntimeData &RuntimeData;

    /** 当前世界 */
    UWorld &World;
    /** 是否负责本机玩法因果 */
    bool bCausal = true;
};
