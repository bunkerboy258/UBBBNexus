#pragma once
#include "CoreMinimal.h"
class ABBBCharacter;
class ABBBPistolEquipment;
class UBBBPistolDefinition;
class USkeletalMeshComponent;
class UWorld;
struct FBBBPistolRuntimeData;


/** Animation 系统根本次调用的临时依赖 */
struct FBBBPistolAnimationUpdateContext final
{
    /** 本次 Equipment 依赖 */
    ABBBPistolEquipment &Equipment;
    /** 本次 Character 依赖 */
    ABBBCharacter &Character;
    /** 本次 WeaponMesh 依赖 */
    USkeletalMeshComponent &WeaponMesh;
    /** 本次 Definition 依赖 */
    const UBBBPistolDefinition &Definition;
    /** 本次 RuntimeData 依赖 */
    FBBBPistolRuntimeData &RuntimeData;
    /** 本次 World 依赖 */
    UWorld &World;
    /** 本次 bCausal 依赖 */
    bool bCausal = true;
};
