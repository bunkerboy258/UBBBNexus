#pragma once
#include "CoreMinimal.h"
class ABBBCharacter;
class ABBBSniperEquipment;
class UBBBSniperDefinition;
class USkeletalMeshComponent;
class UWorld;
struct FBBBSniperRuntimeData;


/** Action 系统根本次调用的临时依赖 */
struct FBBBSniperActionUpdateContext final
{
    /** 本次 Equipment 依赖 */
    ABBBSniperEquipment &Equipment;
    /** 本次 Character 依赖 */
    ABBBCharacter &Character;
    /** 本次 WeaponMesh 依赖 */
    USkeletalMeshComponent &WeaponMesh;
    /** 本次 Definition 依赖 */
    const UBBBSniperDefinition &Definition;
    /** 本次 RuntimeData 依赖 */
    FBBBSniperRuntimeData &RuntimeData;
    /** 本次 World 依赖 */
    UWorld &World;
    /** 本次 bCausal 依赖 */
    bool bCausal = true;
};
