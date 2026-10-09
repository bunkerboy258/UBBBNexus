#pragma once

#include "CoreMinimal.h"

class ABBBCharacter;
class ABBBMinigunEquipment;
class UBBBMinigunDefinition;
class USkeletalMeshComponent;
class UWorld;
struct FBBBMinigunRuntimeData;

/** 只在装备根本次更新期间存活的调用上下文 */
struct FBBBMinigunUpdateContext final
{
    /** 当前转管机枪 */
    ABBBMinigunEquipment &Equipment;

    /** 当前持有者 */
    ABBBCharacter &Character;

    /** 当前装备网格 */
    USkeletalMeshComponent &WeaponMesh;

    /** 当前静态配置 */
    const UBBBMinigunDefinition &Definition;

    /** 当前唯一运行时黑板 */
    FBBBMinigunRuntimeData &RuntimeData;

    /** 当前世界 */
    UWorld &World;
    /** 是否负责本机玩法因果 */
    bool bCausal = true;
};
