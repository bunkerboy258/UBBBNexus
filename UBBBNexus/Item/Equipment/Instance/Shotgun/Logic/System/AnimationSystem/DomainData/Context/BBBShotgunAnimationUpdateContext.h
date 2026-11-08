#pragma once
#include "CoreMinimal.h"
class ABBBCharacter;
class ABBBShotgunEquipment;
class UBBBShotgunDefinition;
class USkeletalMeshComponent;
class UWorld;
struct FBBBShotgunRuntimeData;


/** Animation 系统根本次调用的临时依赖 */
struct FBBBShotgunAnimationUpdateContext final
{
    /** 本次 Equipment 依赖 */
    ABBBShotgunEquipment &Equipment;
    /** 本次 Character 依赖 */
    ABBBCharacter &Character;
    /** 本次 WeaponMesh 依赖 */
    USkeletalMeshComponent &WeaponMesh;
    /** 本次 Definition 依赖 */
    const UBBBShotgunDefinition &Definition;
    /** 本次 RuntimeData 依赖 */
    FBBBShotgunRuntimeData &RuntimeData;
    /** 本次 World 依赖 */
    UWorld &World;
    /** 本次 bCausal 依赖 */
    bool bCausal = true;
};
