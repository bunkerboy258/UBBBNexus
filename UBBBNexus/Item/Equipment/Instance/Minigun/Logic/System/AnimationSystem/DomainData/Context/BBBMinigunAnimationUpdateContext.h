#pragma once
#include "CoreMinimal.h"
class ABBBCharacter;
class ABBBMinigunEquipment;
class UBBBMinigunDefinition;
class USkeletalMeshComponent;
class UWorld;
struct FBBBMinigunRuntimeData;


/** Animation 系统根本次调用的临时依赖 */
struct FBBBMinigunAnimationUpdateContext final
{
    /** 本次 Equipment 依赖 */
    ABBBMinigunEquipment &Equipment;
    /** 本次 Character 依赖 */
    ABBBCharacter &Character;
    /** 本次 WeaponMesh 依赖 */
    USkeletalMeshComponent &WeaponMesh;
    /** 本次 Definition 依赖 */
    const UBBBMinigunDefinition &Definition;
    /** 本次 RuntimeData 依赖 */
    FBBBMinigunRuntimeData &RuntimeData;
    /** 本次 World 依赖 */
    UWorld &World;
    /** 本次 bCausal 依赖 */
    bool bCausal = true;
};
