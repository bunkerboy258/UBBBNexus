#pragma once
#include "CoreMinimal.h"
class ABBBMeleeEquipment;
class ABBBCharacter;
class USkeletalMeshComponent;
class UBBBMeleeDefinition;
class UWorld;
struct FBBBMeleeRuntimeData;
/** 本次 Action 领域依赖 */
struct FBBBMeleeActionUpdateContext final
{
    /** 本次 Equipment 依赖 */
    ABBBMeleeEquipment &Equipment;
    /** 本次 Character 依赖 */
    ABBBCharacter &Character;
    /** 本次 WeaponMesh 依赖 */
    USkeletalMeshComponent &WeaponMesh;
    /** 本次 Definition 依赖 */
    const UBBBMeleeDefinition &Definition;
    /** 本次 RuntimeData 依赖 */
    FBBBMeleeRuntimeData &RuntimeData;
    /** 本次 World 依赖 */
    UWorld &World;
    /** 本次 bCausal 依赖 */
    bool bCausal;
};
