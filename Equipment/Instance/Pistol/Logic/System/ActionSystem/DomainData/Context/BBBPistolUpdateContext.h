#pragma once

#include "CoreMinimal.h"

class ABBBCharacter;
class ABBBPistolEquipment;
class UBBBPistolDefinition;
class USkeletalMeshComponent;
class UWorld;
struct FBBBPistolRuntimeData;

/** 只在装备根本次更新期间存活的调用上下文 */
struct FBBBPistolUpdateContext final
{
    /** 当前手枪 */
    ABBBPistolEquipment &Equipment;

    /** 当前持有者 */
    ABBBCharacter &Character;

    /** 当前装备网格 */
    USkeletalMeshComponent &WeaponMesh;

    /** 当前静态配置 */
    const UBBBPistolDefinition &Definition;

    /** 当前唯一运行时黑板 */
    FBBBPistolRuntimeData &RuntimeData;

    /** 当前世界 */
    UWorld &World;
    /** 是否负责本机玩法因果 */
    bool bCausal = true;
};
