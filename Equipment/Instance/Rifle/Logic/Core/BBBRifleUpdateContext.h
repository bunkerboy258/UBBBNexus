#pragma once

#include "CoreMinimal.h"

class ABBBCharacter;
class ABBBRifleEquipment;
class UBBBRifleDefinition;
class USkeletalMeshComponent;
class UWorld;
struct FBBBRifleRuntimeData;

/** 只在装备根本次更新期间存活的调用上下文 */
struct FBBBRifleUpdateContext final
{
    /** 当前步枪 */
    ABBBRifleEquipment &Equipment;

    /** 当前持有者 */
    ABBBCharacter &Character;

    /** 当前装备网格 */
    USkeletalMeshComponent &WeaponMesh;

    /** 当前静态配置 */
    const UBBBRifleDefinition &Definition;

    /** 当前唯一运行时黑板 */
    FBBBRifleRuntimeData &RuntimeData;

    /** 当前世界 */
    UWorld &World;
};
