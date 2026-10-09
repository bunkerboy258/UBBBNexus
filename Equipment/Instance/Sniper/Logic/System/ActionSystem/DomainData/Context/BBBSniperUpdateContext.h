#pragma once

#include "CoreMinimal.h"

class ABBBCharacter;
class ABBBSniperEquipment;
class UBBBSniperDefinition;
class USkeletalMeshComponent;
class UWorld;
struct FBBBSniperRuntimeData;

/** 只在装备根本次更新期间存活的调用上下文 */
struct FBBBSniperUpdateContext final
{
    /** 当前狙击枪 */
    ABBBSniperEquipment &Equipment;

    /** 当前持有者 */
    ABBBCharacter &Character;

    /** 当前装备网格 */
    USkeletalMeshComponent &WeaponMesh;

    /** 当前静态配置 */
    const UBBBSniperDefinition &Definition;

    /** 当前唯一运行时黑板 */
    FBBBSniperRuntimeData &RuntimeData;

    /** 当前世界 */
    UWorld &World;
    /** 是否负责本机玩法因果 */
    bool bCausal = true;
};
